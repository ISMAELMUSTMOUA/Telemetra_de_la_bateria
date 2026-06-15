/*!
 *****************************************************************************
  @file:  ad7124_console_app.c

  @brief: Implementation for the menu functions that handle the AD7124

  @details:
 -----------------------------------------------------------------------------
Copyright (c) 2019 Analog Devices, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
  - Modified versions of the software must be conspicuously marked as such.
  - This software is licensed solely and exclusively for use with processors/
    products manufactured by or for Analog Devices, Inc.
  - This software may not be combined or merged with other code in any manner
    that would cause the software to become subject to terms and conditions
	which differ from those listed here.
  - Neither the name of Analog Devices, Inc. nor the names of its contributors
    may be used to endorse or promote products derived from this software without
    specific prior written permission.
  - The use of this software may or may not infringe the patent rights of one
    or more patent holders.  This license does not release you from the
	requirement that you obtain separate licenses from these patent holders
	to use this software.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-
INFRINGEMENT, TITLE, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ANALOG DEVICES, INC. OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, PUNITIVE OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, DAMAGES ARISING OUT OF
CLAIMS OF INTELLECTUAL PROPERTY RIGHTS INFRINGEMENT; PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

20180927-7CBSD SLA

*****************************************************************************/

/* includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "main.h"
#include "ad7124.h"
#include "ad7124_support.h"
#include "ad7124_console_app.h"

/* =======================================================
 * DEFINICIÓN FÍSICA DE LOS RELÉS
 * ======================================================= */
#define RELAY_PORT           GPIOC
#define RELAY_PRECARGA_Pin   GPIO_PIN_10  // PC10
#define RELAY_MAIN_POS_Pin   GPIO_PIN_11  // PC11
#define RELAY_MAIN_NEG_Pin   GPIO_PIN_12  // PC12

#define AD7124_CHANNEL_COUNT 15

/* Variables globales vinculadas a tu main.c */
extern FDCAN_HandleTypeDef hfdcan1;
extern ADC_HandleTypeDef hadc1;
extern IWDG_HandleTypeDef hiwdg;
extern UART_HandleTypeDef huart1; // RS485 del JK BMS

static struct ad7124_st_reg ad7124_register_map[AD7124_REG_NO];
static struct ad7124_dev * pAd7124_dev = NULL;
extern const struct ad7124_st_reg ad7124_regs_config_c[AD7124_REG_NO];


int32_t ad7124_app_initialize(uint8_t configID)
{
	if (configID == AD7124_CONFIG_C) {
		memcpy(ad7124_register_map, ad7124_regs_config_c, sizeof(ad7124_register_map));
	} else {
		return(-1);
	}

    struct ad7124_init_param sAd7124_init = {
  		{ 2500000, 0, SPI_MODE_3, NULL },
  		ad7124_register_map,
  		10000
  	};

    return(ad7124_setup(&pAd7124_dev, sAd7124_init));
}

/* =======================================================
 * MÁQUINA DE ESTADOS: RELÉS Y MASTER
 * ======================================================= */
uint8_t target_relay_state = 0;  // Orden del Master: 1 (Encender), 0 (Apagar)
uint8_t current_relay_state = 0; // Estado real de los pines

/**
 * @brief Ejecuta la secuencia de precarga de alta tensión sin bloquear la CPU
 */
void Run_Relay_StateMachine(void)
{
    static uint8_t relay_step = 0;
    static uint32_t timer_precarga = 0;

    /* --- SECUENCIA DE ENCENDIDO --- */
    if (target_relay_state == 1 && current_relay_state == 0) {
        switch (relay_step) {
            case 0: // 1. Cerrar Relé de Precarga (PC10)
                HAL_GPIO_WritePin(RELAY_PORT, RELAY_PRECARGA_Pin, GPIO_PIN_SET);
                timer_precarga = HAL_GetTick();
                relay_step = 1;
                printf("RELES: PC10 (Precarga) cerrado. Cargando...\r\n");
                break;

            case 1: // 2. Esperar 500 ms (Ajustable según el coche)
                if (HAL_GetTick() - timer_precarga >= 500) {
                    relay_step = 2;
                }
                break;

            case 2: // 3. Cerrar Relés Principales (PC11 y PC12)
                HAL_GPIO_WritePin(RELAY_PORT, RELAY_MAIN_POS_Pin | RELAY_MAIN_NEG_Pin, GPIO_PIN_SET);
                timer_precarga = HAL_GetTick();
                relay_step = 3;
                printf("RELES: PC11 y PC12 cerrados. Alta tension activa.\r\n");
                break;

            case 3: // 4. Esperar 50ms mecánicos y soltar Precarga (PC10)
                if (HAL_GetTick() - timer_precarga >= 50) {
                    HAL_GPIO_WritePin(RELAY_PORT, RELAY_PRECARGA_Pin, GPIO_PIN_RESET);
                    current_relay_state = 1;
                    relay_step = 0;
                    printf("RELES: PC10 abierto. Secuencia completada.\r\n");
                }
                break;
        }
    }
    /* --- SECUENCIA DE APAGADO DE EMERGENCIA --- */
    else if (target_relay_state == 0 && current_relay_state == 1) {

        // El coche pide apagar: Abrimos PC10, PC11 y PC12 de golpe por seguridad
        HAL_GPIO_WritePin(RELAY_PORT, RELAY_PRECARGA_Pin | RELAY_MAIN_POS_Pin | RELAY_MAIN_NEG_Pin, GPIO_PIN_RESET);
        current_relay_state = 0;
        relay_step = 0;
        printf("RELES: AISLAMIENTO ACTIVO. Todos los reles abiertos.\r\n");
    }
}

/**
 * @brief Lee el bus CAN buscando comandos del Master ECU
 */
void Process_Master_Commands(void)
{
    FDCAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];

    /* Comprobamos si hay algún mensaje en el buzón FIFO0 */
    if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0) > 0) {
        if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK) {

            /* ID 0x200 asignado hipotéticamente al Master ECU para control */
            if (RxHeader.Identifier == 0x200) {
                if (RxData[0] == 0x01) {
                    target_relay_state = 1; // Pide encender
                } else if (RxData[0] == 0x00) {
                    target_relay_state = 0; // Pide apagar
                }
            }
        }
    }
}

/* =======================================================
 * BMS JK RS485 MODBUS PARSER
 * ======================================================= */

/**
 * @brief Calcula el CRC16 Modbus
 */
uint16_t Modbus_CRC16(uint8_t *buf, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos];
        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/**
 * @brief Pregunta al BMS alternando entre Datos Generales y Voltaje de Celdas
 */
void Request_And_Parse_JK_BMS(void)
{
    static uint8_t bms_req_state = 0; // 0 = Generales, 1 = Celdas
    uint8_t rx_buf[80]; // Buffer para la trama de celdas

    FDCAN_TxHeaderTypeDef TxHeader;
    uint8_t TxData[8];

    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_DLC_BYTES_8;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    /* --- ESTADO 0: PEDIR DATOS GENERALES (Voltaje, Corriente, SOC) --- */
    if (bms_req_state == 0) {
        uint8_t tx_req[8] = {0x01, 0x03, 0x00, 0x90, 0x00, 0x1A, 0x00, 0x00};
        uint16_t crc_tx = Modbus_CRC16(tx_req, 6);
        tx_req[6] = crc_tx & 0xFF; tx_req[7] = (crc_tx >> 8) & 0xFF;

        HAL_UART_Transmit(&huart1, tx_req, 8, 10);

        if (HAL_UART_Receive(&huart1, rx_buf, 57, 50) == HAL_OK) {
            if (rx_buf[0] == 0x01 && rx_buf[1] == 0x03 && rx_buf[2] == 0x34) {

                uint32_t bms_vol_mv = (rx_buf[3] << 24) | (rx_buf[4] << 16) | (rx_buf[5] << 8) | rx_buf[6];
                float bms_voltage = bms_vol_mv / 1000.0f;

                int32_t bms_cur_ma = (rx_buf[11] << 24) | (rx_buf[12] << 16) | (rx_buf[13] << 8) | rx_buf[14];
                float bms_current = bms_cur_ma / 1000.0f;

                uint8_t bms_soc = rx_buf[25];
                uint32_t bms_alarms = (rx_buf[19] << 24) | (rx_buf[20] << 16) | (rx_buf[21] << 8) | rx_buf[22];

                // CAN ID 0x120: Voltaje y Corriente
                TxHeader.Identifier = 0x120;
                memcpy(&TxData[0], &bms_voltage, 4);
                memcpy(&TxData[4], &bms_current, 4);
                HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData);

                // CAN ID 0x121: SOC y Alarmas
                TxHeader.Identifier = 0x121;
                TxData[0] = bms_soc;
                TxData[1] = (bms_alarms >> 16) & 0xFF;
                TxData[2] = (bms_alarms >> 8) & 0xFF;
                TxData[3] = bms_alarms & 0xFF;
                TxData[4] = 0; TxData[5] = 0; TxData[6] = 0; TxData[7] = 0;
                HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData);
            }
        }
        bms_req_state = 1;
    }
    /* --- ESTADO 1: PEDIR VOLTAJE INDIVIDUAL DE LAS CELDAS --- */
    else if (bms_req_state == 1) {
        uint8_t tx_req[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00};
        uint16_t crc_tx = Modbus_CRC16(tx_req, 6);
        tx_req[6] = crc_tx & 0xFF; tx_req[7] = (crc_tx >> 8) & 0xFF;

        HAL_UART_Transmit(&huart1, tx_req, 8, 10);

        if (HAL_UART_Receive(&huart1, rx_buf, 69, 50) == HAL_OK) {
            if (rx_buf[0] == 0x01 && rx_buf[1] == 0x03 && rx_buf[2] == 0x40) {

                // Agrupamos 4 celdas por mensaje (8 mensajes CAN)
                for (int i = 0; i < 8; i++) {
                    TxHeader.Identifier = 0x130 + i;
                    TxData[0] = rx_buf[3 + (i * 8) + 0]; TxData[1] = rx_buf[3 + (i * 8) + 1];
                    TxData[2] = rx_buf[3 + (i * 8) + 2]; TxData[3] = rx_buf[3 + (i * 8) + 3];
                    TxData[4] = rx_buf[3 + (i * 8) + 4]; TxData[5] = rx_buf[3 + (i * 8) + 5];
                    TxData[6] = rx_buf[3 + (i * 8) + 6]; TxData[7] = rx_buf[3 + (i * 8) + 7];
                    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData);
                }
            }
        }
        bms_req_state = 0;
    }
}

/* =======================================================
 * SUPER-BUCLE DE TELEMETRÍA AUTOMÁTICO
 * ======================================================= */

/**
 * @brief Super-bucle de telemetría automático y no bloqueante
 * @note Utiliza la configuración base (canales apagados) y activa dinámicamente
 * cada canal por SPI. Utiliza fórmula ratiométrica pura.
 */
void ad7124_run_automated_telemetry(void)
{
    int32_t sample_data;
    uint8_t TxData[8];

    static uint8_t seq_state = 0;
    static uint8_t current_channel = 0;
    static uint32_t timer_estabilizacion = 0;

    /* --- MATRICES DE DEPURACIÓN PARA HÉRCULES --- */
    static float debug_temperatures[15] = {0};
    static float debug_voltages[15] = {0};
    static int32_t debug_sample_data[15] = {0};
    static uint32_t last_debug_print = 0;

    float current_traccion = 0.0f;

    FDCAN_TxHeaderTypeDef TxHeader;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_DLC_BYTES_8;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    /* Forzamos el ADC a MODO CONTINUO (Mode = 0) en el arranque */
    static uint8_t init_done = 0;
    if (!init_done) {
        ad7124_register_map[AD7124_ADC_Control].value &= ~(AD7124_ADC_CTRL_REG_MODE(0xF));
        ad7124_write_register(pAd7124_dev, ad7124_register_map[AD7124_ADC_Control]);
        init_done = 1;
    }

    while (1) {
    	HAL_IWDG_Refresh(&hiwdg);

        /* =========================================================
         * 1. GESTIÓN DE RELÉS Y ORDENES DEL MASTER
         * ========================================================= */
        Process_Master_Commands();
        Run_Relay_StateMachine();

        /* =========================================================
         * 2. LECTURA Y PARSEO DEL JK BMS (RS485 a CAN)
         * ========================================================= */
        static uint32_t timer_bms = 0;
        if(HAL_GetTick() - timer_bms >= 250) {
            Request_And_Parse_JK_BMS();
            timer_bms = HAL_GetTick();
        }

        /* =========================================================
         * 3. SENSOR DE CORRIENTE DE TRACCIÓN (HASS)
         * ========================================================= */
        HAL_ADC_Start(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, 1) == HAL_OK) {
            uint32_t raw_current = HAL_ADC_GetValue(&hadc1);
            float v_hass = ((float)raw_current / 4095.0f) * 3.3f;
            current_traccion = (v_hass - 2.5f) / 0.003125f;

            TxHeader.Identifier = 0x110;
            TxData[0] = 0xFF;
            memcpy(&TxData[1], &current_traccion, sizeof(float));
            TxData[5] = 0; TxData[6] = 0; TxData[7] = 0;
            HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData);
        }
        HAL_ADC_Stop(&hadc1);

        /* =========================================================
         * 4. MÁQUINA DE ESTADOS SECUENCIAL AD7124
         * ========================================================= */
        switch (seq_state) {

            case 0: /* FASE 0: ENRUTAR CORRIENTE Y CAMBIAR DE CANAL */
                // Apagar el canal anterior
                uint8_t prev_channel = (current_channel == 0) ? 14 : current_channel - 1;
                ad7124_register_map[AD7124_Channel_0 + prev_channel].value &= ~0x8000;
                ad7124_write_register(pAd7124_dev, ad7124_register_map[AD7124_Channel_0 + prev_channel]);

                // Mover 500 µA (IOUT0) al pin correcto
                ad7124_register_map[AD7124_IOCon1].value = 0x000400 | (current_channel & 0x0F);
                ad7124_write_register(pAd7124_dev, ad7124_register_map[AD7124_IOCon1]);

                // Encender canal actual. (El ADC reinicia el filtro Sinc4 automáticamente)
                ad7124_register_map[AD7124_Channel_0 + current_channel].value = 0x8000 | (current_channel << 5) | 0x0F;
                ad7124_write_register(pAd7124_dev, ad7124_register_map[AD7124_Channel_0 + current_channel]);

                timer_estabilizacion = HAL_GetTick();
                seq_state = 1;
                break;

            case 1: /* FASE 1: ESPERAR ESTABILIZACIÓN (Tiempo de filtro + circuito RC) */
                // 100ms da margen de sobra para que el Sinc4 haga sus 4 lecturas
                if ((HAL_GetTick() - timer_estabilizacion) >= 100) {
                    seq_state = 2;
                }
                break;

            case 2: /* FASE 2: EXTRACCIÓN Y FILTRADO */
                if (ad7124_wait_for_conv_ready(pAd7124_dev, 2) >= 0) {
                    if (ad7124_read_data(pAd7124_dev, &sample_data) >= 0) {

                        /* Guardamos en matrices para depurar en Hércules */
                        debug_sample_data[current_channel] = sample_data;
                        debug_voltages[current_channel] = ad7124_convert_sample_to_voltage(pAd7124_dev, current_channel, sample_data);

                        /* Cálculo de Competición (Ratiométrico puro) */
                        //esto debería calcular completamente las resistencias de los sensores y obtener la resistencia del cable
                        float r_offeset[15] = {0};
					    float r_sensor_real = ((float)sample_data / 16777216.0f) * (5110.0f / 8.0f);
					    float r_sensor_con_off = r_sensor_real - r_offeset[current_channel];
                        float temp = (r_sensor_con_off - 100.0f) / 0.385f;

                        bool enviar_can = false;

                        /* Filtro de validación */
                        if (r_sensor_con_off > 145.0f) {
                            temp = 999.0f;       // Cable Roto
                            enviar_can = true;
                        }
                        else if (r_sensor_con_off < 10.0f) {
                            temp = -999.0f;      // Cortocircuito
                            enviar_can = true;
                        }
                        else if (temp >= -10.0f && temp <= 95.0f) {
                            enviar_can = true;   // Dato Válido
                        }

                        if (enviar_can) {
                            debug_temperatures[current_channel] = temp;

                            TxHeader.Identifier = 0x100 + current_channel;
                            TxData[0] = current_channel;
                            memcpy(&TxData[1], &temp, sizeof(float));
                            TxData[5] = 0; TxData[6] = 0; TxData[7] = 0;
                            HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData);
                        }
                    }

                    // Siguiente canal
                    current_channel++;
                    if (current_channel >= 15) current_channel = 0;
                    seq_state = 0;
                }
                else if ((HAL_GetTick() - timer_estabilizacion) > 200) {
                    // Si se bloquea por ruido, forzar reinicio de canal
                    seq_state = 0;
                }
                break;
        }

        /* =========================================================
         * 3. TERMINAL HÉRCULES (1 Hz)
         * ========================================================= */
        if (HAL_GetTick() - last_debug_print >= 1000) {
            last_debug_print = HAL_GetTick();
            printf("\r\n--- MONITOR DE TELEMETRIA ---\r\n");
            for (uint8_t i = 0; i < 4; i++) {
                // Imprimimos el canal, temperatura, el RAW en formato entero y el voltaje
                printf("Ch%d: Temp %4.1f C | RAW: %10ld | Volt: %1.4f V\r\n",
                       i, debug_temperatures[i], (long)debug_sample_data[i], debug_voltages[i]);
            }
        }

        HAL_Delay(1);
    }
}

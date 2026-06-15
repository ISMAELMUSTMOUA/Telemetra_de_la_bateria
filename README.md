# Telemetría de la batería — README

Este README describe la estructura y los ficheros principales dentro de la carpeta `Core`, junto con instrucciones básicas para compilar y flashear el proyecto.

## Contenido

- Estructura rápida de `Core`
- Enlaces rápidos a ficheros
- Descripción breve de ficheros en `Core/Inc` y `Core/Src`
- Guía rápida de compilación y flasheo

---

## Estructura rápida (Carpeta `Core`)

```text
Core/
├─ Inc/
│  ├─ ad7124_console_app.h
│  ├─ ad7124.h
│  ├─ ad7124_regs.h
│  ├─ ad7124_regs_configs.h
│  ├─ ad7124_support.h
│  ├─ adi_console_menu.h
│  ├─ delay.h
│  ├─ error.h
│  ├─ main.h
│  ├─ platform_support.h
│  ├─ spi.h
│  └─ stm32g4xx_hal_conf.h
├─ Src/
│  ├─ ad7124.c
│  ├─ ad7124_console_app.c
│  ├─ ad7124_regs.c
│  ├─ ad7124_regs_config_a.c
│  ├─ ad7124_regs_config_b.c
│  ├─ ad7124_regs_config_c.c
│  ├─ ad7124_support.c
│  ├─ adi_console_menu.c
│  ├─ delay.c
│  ├─ main.c
│  ├─ platform_support.c
│  ├─ spi.c
│  ├─ stm32g4xx_hal_msp.c
│  ├─ stm32g4xx_it.c
│  ├─ syscalls.c
│  ├─ sysmem.c
│  └─ system_stm32g4xx.c
└─ Startup/
   └─ startup_stm32g474retx.s
```
## Descripción breve (Core/Inc)

- `ad7124_console_app.h` — Declaraciones del menú/ejemplo que orquesta la adquisición, telemetría y utilidades (CRC Modbus, lectura de comandos CAN/UART).
- `ad7124.h` — Macros y mapa de registros del AD7124; dependencias de SPI y delay.
- `ad7124_regs.h` — Declaración del array/estructura que describe los registros del AD7124.
- `ad7124_regs_configs.h` — Prototipos de diferentes arrays de configuración (A/B/C y variantes por entrada).
- `ad7124_support.h` — Funciones auxiliares para convertir muestras a voltaje y consultar propiedades de canales.
- `adi_console_menu.h` — Definición de estructuras y funciones del gestor de menús de consola.
- `delay.h` — Prototipos de `udelay` y `mdelay`.
- `error.h` — Macros de retorno (`SUCCESS`, `FAILURE`).
- `main.h` — Defines generados por CubeMX: pines, periféricos y `Error_Handler()`.
- `platform_support.h` — Retarget y helpers de plataforma (`__io_putchar`, `__io_getchar`, `toggle_activity_led`).
- `spi.h` — Abstracción y estructuras para el periférico SPI.

## Descripción breve (Core/Src)

- `ad7124.c` — Driver de bajo nivel para AD7124 (lectura/escritura de registros, inicialización).
- `ad7124_console_app.c` — Lógica de alto nivel: control de muestreo, telemetría y manejo de relés.
- `ad7124_regs*.c` — Definición de los arrays de configuración para cada perfil (A/B/C).
- `ad7124_support.c` — Implementaciones de utilidades: conversión de muestras, consultas de canal.
- `adi_console_menu.c` — Implementación del gestor de menús de consola.
- `delay.c` — Implementación de `udelay` y `mdelay`.
- `main.c` — `main()` del firmware: inicialización y bucle principal.
- `platform_support.c` — Retarget de E/S y funciones específicas de la placa.
- `spi.c` — Implementación concreta de la capa SPI usada por el driver.
- `stm32g4xx_hal_msp.c` — Inicialización a bajo nivel (MSP) generada por CubeMX.
- `stm32g4xx_it.c` — Rutinas de interrupción.
- `syscalls.c`, `sysmem.c` — Stubs/soporte para newlib y memoria.
- `system_stm32g4xx.c` — Configuración de reloj y SystemInit.

---
## Enlaces rápidos (Core)

- [Core/Inc/ad7124_console_app.h](Core/Inc/ad7124_console_app.h)
- [Core/Inc/ad7124.h](Core/Inc/ad7124.h)
- [Core/Inc/ad7124_regs.h](Core/Inc/ad7124_regs.h)
- [Core/Inc/ad7124_regs_configs.h](Core/Inc/ad7124_regs_configs.h)
- [Core/Inc/ad7124_support.h](Core/Inc/ad7124_support.h)
- [Core/Inc/adi_console_menu.h](Core/Inc/adi_console_menu.h)
- [Core/Inc/delay.h](Core/Inc/delay.h)
- [Core/Inc/error.h](Core/Inc/error.h)
- [Core/Inc/main.h](Core/Inc/main.h)
- [Core/Inc/platform_support.h](Core/Inc/platform_support.h)
- [Core/Inc/spi.h](Core/Inc/spi.h)
- [Core/Inc/stm32g4xx_hal_conf.h](Core/Inc/stm32g4xx_hal_conf.h)
- [Core/Src/ad7124.c](Core/Src/ad7124.c)
- [Core/Src/ad7124_console_app.c](Core/Src/ad7124_console_app.c)
- [Core/Src/ad7124_regs.c](Core/Src/ad7124_regs.c)
- [Core/Src/ad7124_regs_config_a.c](Core/Src/ad7124_regs_config_a.c)
- [Core/Src/ad7124_regs_config_b.c](Core/Src/ad7124_regs_config_b.c)
- [Core/Src/ad7124_regs_config_c.c](Core/Src/ad7124_regs_config_c.c)
- [Core/Src/ad7124_support.c](Core/Src/ad7124_support.c)
- [Core/Src/adi_console_menu.c](Core/Src/adi_console_menu.c)
- [Core/Src/delay.c](Core/Src/delay.c)
- [Core/Src/main.c](Core/Src/main.c)
- [Core/Src/platform_support.c](Core/Src/platform_support.c)
- [Core/Src/spi.c](Core/Src/spi.c)
- [Core/Src/stm32g4xx_hal_msp.c](Core/Src/stm32g4xx_hal_msp.c)
- [Core/Src/stm32g4xx_it.c](Core/Src/stm32g4xx_it.c)
- [Core/Src/syscalls.c](Core/Src/syscalls.c)
- [Core/Src/sysmem.c](Core/Src/sysmem.c)
- [Core/Src/system_stm32g4xx.c](Core/Src/system_stm32g4xx.c)
- [Core/Startup/startup_stm32g474retx.s](Core/Startup/startup_stm32g474retx.s)

---


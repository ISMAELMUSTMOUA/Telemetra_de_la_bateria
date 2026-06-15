#include "ad7124_regs.h"

const struct ad7124_st_reg ad7124_regs_config_c[AD7124_REG_NO] = {
    {0x00, 0x00, 1, 2},       /* Status */
    {0x01, 0x0580, 2, 1},     /* ADC_Control: Modo continuo, ref interna */
    {0x02, 0x000000, 3, 2},   /* Data */
    {0x03, 0x000000, 3, 1},   /* IOCon1: Iniciamos en 0, el C lo moverá dinámicamente */
    {0x04, 0x0000, 2, 1},     /* IOCon2 */
    {0x05, 0x00, 1, 2},       /* ID */
    {0x06, 0x000000, 3, 2},   /* Error */
    {0x07, 0x000044, 3, 1},   /* Error_En */
    {0x08, 0x00, 1, 2},       /* Mclk_Count */

    /* * TODOS LOS CANALES APAGADOS (Bit 15 = 0). 
     * El software C se encargará de poner el bit 15 a 1 (0x80...) 
     * solo cuando toque medir ese canal específico. 
     */
    {0x09, 0x000F, 2, 1},     /* Ch0: AIN0-AIN15 */
    {0x0A, 0x002F, 2, 1},     /* Ch1: AIN1-AIN15 */
    {0x0B, 0x004F, 2, 1},     /* Ch2: AIN2-AIN15 */
    {0x0C, 0x006F, 2, 1},     /* Ch3: AIN3-AIN15 */
    {0x0D, 0x008F, 2, 1},     /* Ch4: AIN4-AIN15 */
    {0x0E, 0x00AF, 2, 1},     /* Ch5: AIN5-AIN15 */
    {0x0F, 0x00CF, 2, 1},     /* Ch6: AIN6-AIN15 */
    {0x10, 0x00EF, 2, 1},     /* Ch7: AIN7-AIN15 */
    {0x11, 0x010F, 2, 1},     /* Ch8: AIN8-AIN15 */
    {0x12, 0x012F, 2, 1},     /* Ch9: AIN9-AIN15 */
    {0x13, 0x014F, 2, 1},     /* Ch10: AIN10-AIN15 */
    {0x14, 0x016F, 2, 1},     /* Ch11: AIN11-AIN15 */
    {0x15, 0x018F, 2, 1},     /* Ch12: AIN12-AIN15 */
    {0x16, 0x01AF, 2, 1},     /* Ch13: AIN13-AIN15 */
    {0x17, 0x01CF, 2, 1},     /* Ch14: AIN14-AIN15 */

    {0x18, 0x0001, 2, 1},     /* Channel_15 -> DESHABILITADO */

    /* Setup 0 (Configuración única para todos) */
    {0x19, 0x0503, 2, 1}, {0x1A, 0x0860, 2, 1}, {0x1B, 0x0860, 2, 1}, {0x1C, 0x0860, 2, 1},
    {0x1D, 0x0860, 2, 1}, {0x1E, 0x0860, 2, 1}, {0x1F, 0x0860, 2, 1}, {0x20, 0x0860, 2, 1},

    /* Filtros y Calibración (Mismos que ya tienes) */
    {0x21, 0x060180, 3, 1}, {0x22, 0x060180, 3, 1}, {0x23, 0x060180, 3, 1}, {0x24, 0x060180, 3, 1},
    {0x25, 0x060180, 3, 1}, {0x26, 0x060180, 3, 1}, {0x27, 0x060180, 3, 1}, {0x28, 0x060180, 3, 1},
    {0x29, 0x800000, 3, 1}, {0x2A, 0x800000, 3, 1}, {0x2B, 0x800000, 3, 1}, {0x2C, 0x800000, 3, 1},
    {0x2D, 0x800000, 3, 1}, {0x2E, 0x800000, 3, 1}, {0x2F, 0x800000, 3, 1}, {0x30, 0x800000, 3, 1},
    {0x31, 0x500000, 3, 1}, {0x32, 0x500000, 3, 1}, {0x33, 0x500000, 3, 1}, {0x34, 0x500000, 3, 1},
    {0x35, 0x500000, 3, 1}, {0x36, 0x500000, 3, 1}, {0x37, 0x500000, 3, 1}, {0x38, 0x500000, 3, 1},
};

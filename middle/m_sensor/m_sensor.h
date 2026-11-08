/* TCS34725 RGBC Color Sensor driver */
#ifndef __M_TCS34_H__
#define __M_TCS34_H__

#include "h_i2c.h"
#include <stdint.h>

/* ============================== Device ====================================== */
#define TCS34_ADDR 0x29U

#define TCS34_ID_34725 0x44U
#define TCS34_ID_34727 0x4DU

#define TCS34_STATUS_AVALID 0x01U /* RGBC data valid  */
#define TCS34_STATUS_AINT 0x10U   /* RGBC interrupt   */

/* ============================== Gain options ================================ */
typedef enum
{
    TCS34_GAIN_1X = 0x00,
    TCS34_GAIN_4X = 0x01,
    TCS34_GAIN_16X = 0x02,
    TCS34_GAIN_60X = 0x03
} TCS34_Gain;

/* ============================== RGBC result ================================= */
typedef struct
{
    uint16_t c; /* clear  */
    uint16_t r; /* red    */
    uint16_t g; /* green  */
    uint16_t b; /* blue   */
} TCS34_Data;

/* ============================== Public API ================================== */

/*
 * TCS34_Init  - power on, set integration time and gain
 *   atime      : integration time register (0xFF=2.4ms .. 0x00=614ms)
 *   gain       : TCS34_GAIN_1X / 4X / 16X / 60X
 * Returns I2C_OK on success. Must be called from a task (delays 3 ms).
 */
I2C_Status TCS34_Init(volatile I2C_TypeDef *i2c, uint8_t atime, TCS34_Gain gain);

/* TCS34_ReadID - read chip ID register (TCS34_ID_34725 or TCS34_ID_34727) */
I2C_Status TCS34_ReadID(volatile I2C_TypeDef *i2c, uint8_t *id);

/* TCS34_Read - burst read all 4 channels into TCS34_Data */
I2C_Status TCS34_Read(volatile I2C_TypeDef *i2c, TCS34_Data *out);

I2C_Status TCS34_ReadStatus(volatile I2C_TypeDef *i2c, uint8_t *status);

/* TCS34_EnableInterrupt - assert INT (active low) after every RGBC cycle */
I2C_Status TCS34_EnableInterrupt(volatile I2C_TypeDef *i2c);

/* TCS34_ClearInterrupt - release INT for the next RGBC cycle */
I2C_Status TCS34_ClearInterrupt(volatile I2C_TypeDef *i2c);

#endif /*<! __M_TCS34_H__ */

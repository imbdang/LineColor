#include "m_sensor.h"
#include "m_i2c.h"
#include "m_time.h"

/* ============================== Registers =================================== */
#define TCS34_CMD 0x80U       /* command bit - must be set on every access */
#define TCS34_CMD_AUTO 0x20U  /* auto-increment protocol for burst access  */
#define TCS34_CMD_CLEAR_INT 0xE6U /* special function: clear RGBC interrupt */

#define TCS34_REG_ENABLE 0x00U
#define TCS34_REG_ATIME 0x01U
#define TCS34_REG_AILTL 0x04U /* AILTL, AILTH, AIHTL, AIHTH */
#define TCS34_REG_PERS 0x0CU
#define TCS34_REG_CONTROL 0x0FU
#define TCS34_REG_ID 0x12U
#define TCS34_REG_STATUS 0x13U
#define TCS34_REG_CDATA 0x14U /* C_L, C_H, R_L, R_H, G_L, G_H, B_L, B_H  */

#define TCS34_ENABLE_PON 0x01U  /* power on         */
#define TCS34_ENABLE_AEN 0x02U  /* RGBC enable      */
#define TCS34_ENABLE_AIEN 0x10U /* RGBC int enable  */

/* ============================== Public API ================================== */

I2C_Status TCS34_Init(volatile I2C_TypeDef *i2c, uint8_t atime, TCS34_Gain gain)
{
    I2C_Status s;

    /* chip may still be running after an MCU-only reset */
    s = M_I2C_WriteReg(i2c, TCS34_ADDR, TCS34_CMD | TCS34_REG_ENABLE, 0U);
    if (s)
        return s;

    /* integration time */
    s = M_I2C_WriteReg(i2c, TCS34_ADDR, TCS34_CMD | TCS34_REG_ATIME, atime);
    if (s)
        return s;

    /* gain */
    s = M_I2C_WriteReg(i2c, TCS34_ADDR, TCS34_CMD | TCS34_REG_CONTROL, (uint8_t)gain);
    if (s)
        return s;

    /* power on first, wait for oscillator (> 2.4 ms) */
    s = M_I2C_WriteReg(i2c, TCS34_ADDR, TCS34_CMD | TCS34_REG_ENABLE, TCS34_ENABLE_PON);
    if (s)
        return s;

    M_TIME_DelayMs(3U);

    /* enable RGBC ADC */
    return M_I2C_WriteReg(i2c, TCS34_ADDR, TCS34_CMD | TCS34_REG_ENABLE,
                          TCS34_ENABLE_PON | TCS34_ENABLE_AEN);
}

I2C_Status TCS34_ReadID(volatile I2C_TypeDef *i2c, uint8_t *id)
{
    if (id == 0)
        return I2C_ERR_BUS;

    return M_I2C_ReadReg(i2c, TCS34_ADDR, TCS34_CMD | TCS34_REG_ID, id);
}

I2C_Status TCS34_Read(volatile I2C_TypeDef *i2c, TCS34_Data *out)
{
    uint8_t raw[8];
    I2C_Status s;

    s = M_I2C_ReadRegs(i2c, TCS34_ADDR,
                       TCS34_CMD | TCS34_CMD_AUTO | TCS34_REG_CDATA,
                       raw, 8);
    if (s)
        return s;

    out->c = (uint16_t)((raw[1] << 8) | raw[0]);
    out->r = (uint16_t)((raw[3] << 8) | raw[2]);
    out->g = (uint16_t)((raw[5] << 8) | raw[4]);
    out->b = (uint16_t)((raw[7] << 8) | raw[6]);

    return I2C_OK;
}

I2C_Status TCS34_ReadStatus(volatile I2C_TypeDef *i2c, uint8_t *status)
{
    return M_I2C_ReadReg(i2c, TCS34_ADDR, TCS34_CMD | TCS34_REG_STATUS, status);
}

I2C_Status TCS34_EnableInterrupt(volatile I2C_TypeDef *i2c)
{
    /* Low threshold 0, high threshold 65535. PERS = 0 fires on every cycle
     * regardless of the thresholds. */
    const uint8_t thresholds[5] = {
        TCS34_CMD | TCS34_CMD_AUTO | TCS34_REG_AILTL,
        0x00U, 0x00U, 0xFFU, 0xFFU};
    I2C_Status s;

    s = M_I2C_Write(i2c, TCS34_ADDR, thresholds, sizeof(thresholds));
    if (s)
        return s;

    s = M_I2C_WriteReg(i2c, TCS34_ADDR, TCS34_CMD | TCS34_REG_PERS, 0U);
    if (s)
        return s;

    s = TCS34_ClearInterrupt(i2c);
    if (s)
        return s;

    return M_I2C_WriteReg(i2c, TCS34_ADDR, TCS34_CMD | TCS34_REG_ENABLE,
                          TCS34_ENABLE_PON | TCS34_ENABLE_AEN | TCS34_ENABLE_AIEN);
}

I2C_Status TCS34_ClearInterrupt(volatile I2C_TypeDef *i2c)
{
    const uint8_t command = TCS34_CMD_CLEAR_INT;

    return M_I2C_Write(i2c, TCS34_ADDR, &command, 1U);
}

#ifndef __M_I2C_H__
#define __M_I2C_H__

#include "h_i2c.h"

/*
 * Bao h_i2c cho FreeRTOS. h_i2c doc nhieu byte bang polling va phai xu ly
 * byte cuoi kip thoi, nen scheduler bi khoa trong luc truyen.
 * Chi goi trong task.
 */
void M_I2C_Init(volatile I2C_TypeDef *i2c, uint32_t pclk1_MHz, uint32_t speed_kHz);
void M_I2C_Recover(volatile I2C_TypeDef *i2c, uint32_t pclk1_MHz, uint32_t speed_kHz);

I2C_Status M_I2C_Write(volatile I2C_TypeDef *i2c, uint8_t dev_addr, const uint8_t *data, uint8_t len);
I2C_Status M_I2C_WriteReg(volatile I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg, uint8_t value);
I2C_Status M_I2C_ReadReg(volatile I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg, uint8_t *value);
I2C_Status M_I2C_ReadRegs(volatile I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len);

#endif /*<! __M_I2C_H__ */

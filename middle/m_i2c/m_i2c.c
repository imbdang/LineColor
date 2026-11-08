#include "m_i2c.h"

#include "FreeRTOS.h"
#include "task.h"

void M_I2C_Init(volatile I2C_TypeDef *i2c, uint32_t pclk1_MHz, uint32_t speed_kHz)
{
    vTaskSuspendAll();
    I2C_Init(i2c, pclk1_MHz, speed_kHz);
    (void)xTaskResumeAll();
}

void M_I2C_Recover(volatile I2C_TypeDef *i2c, uint32_t pclk1_MHz, uint32_t speed_kHz)
{
    vTaskSuspendAll();
    I2C_Recover(i2c, pclk1_MHz, speed_kHz);
    (void)xTaskResumeAll();
}

I2C_Status M_I2C_Write(volatile I2C_TypeDef *i2c, uint8_t dev_addr, const uint8_t *data, uint8_t len)
{
    I2C_Status status;

    vTaskSuspendAll();
    status = I2C_Write(i2c, dev_addr, data, len);
    (void)xTaskResumeAll();

    return status;
}

I2C_Status M_I2C_WriteReg(volatile I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg, uint8_t value)
{
    I2C_Status status;

    vTaskSuspendAll();
    status = I2C_WriteReg(i2c, dev_addr, reg, value);
    (void)xTaskResumeAll();

    return status;
}

I2C_Status M_I2C_ReadReg(volatile I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg, uint8_t *value)
{
    I2C_Status status;

    vTaskSuspendAll();
    status = I2C_ReadReg(i2c, dev_addr, reg, value);
    (void)xTaskResumeAll();

    return status;
}

I2C_Status M_I2C_ReadRegs(volatile I2C_TypeDef *i2c, uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
    I2C_Status status;

    vTaskSuspendAll();
    status = I2C_ReadRegs(i2c, dev_addr, reg, buf, len);
    (void)xTaskResumeAll();

    return status;
}

/* h_i2c.h — STM32F103 bare-metal I2C driver (RM0008)
 * Supports I2C1 (PB6/PB7) and I2C2 (PB10/PB11)
 */
#ifndef __H_I2C_H__
#define __H_I2C_H__

#include <stdint.h>

/* ========================= I2C Register Map ================================ */
typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t DR;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
    volatile uint32_t CCR;
    volatile uint32_t TRISE;
} I2C_TypeDef;

#define I2C1_BASE 0x40005400UL
#define I2C2_BASE 0x40005800UL

#define I2C1 ((volatile I2C_TypeDef *)I2C1_BASE)
#define I2C2 ((volatile I2C_TypeDef *)I2C2_BASE)

/* --------------------------------- CR1 ------------------------------------ */
#define I2C_CR1_PE (1U << 0)
#define I2C_CR1_START (1U << 8)
#define I2C_CR1_STOP (1U << 9)
#define I2C_CR1_ACK (1U << 10)
#define I2C_CR1_SWRST (1U << 15)

/* --------------------------------- SR1 ------------------------------------ */
#define I2C_SR1_SB (1U << 0)
#define I2C_SR1_ADDR (1U << 1)
#define I2C_SR1_BTF (1U << 2)
#define I2C_SR1_RXNE (1U << 6)
#define I2C_SR1_TXE (1U << 7)
#define I2C_SR1_BERR (1U << 8)
#define I2C_SR1_ARLO (1U << 9)
#define I2C_SR1_AF (1U << 10)
#define I2C_SR1_OVR (1U << 11)

/* --------------------------------- SR2 ------------------------------------ */
#define I2C_SR2_BUSY (1U << 1)

/* --------------------------------- CCR ------------------------------------ */
#define I2C_CCR_FS (1U << 15)

/* ========================= Return Codes =================================== */
typedef enum
{
    I2C_OK = 0,
    I2C_ERR_TIMEOUT,
    I2C_ERR_NACK,
    I2C_ERR_BUS
} I2C_Status;

/* ========================= Public API ===================================== */

/**
 * I2C_Init - Khởi tạo I2C peripheral + GPIO
 *   i2c        : I2C1 hoặc I2C2
 *   pclk1_MHz  : tần số APB1 (MHz), vd: 36
 *   speed_kHz  : tốc độ bus (kHz), vd: 100 hoặc 400
 */
void I2C_Init(volatile I2C_TypeDef *i2c,
              uint32_t pclk1_MHz,
              uint32_t speed_kHz);

/**
 * I2C_Write - Ghi N byte tới device (không kèm địa chỉ register)
 */
I2C_Status I2C_Write(volatile I2C_TypeDef *i2c,
                     uint8_t dev_addr,
                     const uint8_t *data,
                     uint8_t len);

/**
 * I2C_Read - Đọc N byte từ device (không kèm địa chỉ register)
 */
I2C_Status I2C_Read(volatile I2C_TypeDef *i2c,
                    uint8_t dev_addr,
                    uint8_t *data,
                    uint8_t len);

/**
 * I2C_WriteReg - Ghi 1 byte vào register
 */
I2C_Status I2C_WriteReg(volatile I2C_TypeDef *i2c,
                        uint8_t dev_addr,
                        uint8_t reg,
                        uint8_t value);

/**
 * I2C_ReadReg - Đọc 1 byte từ register (dùng repeated start)
 */
I2C_Status I2C_ReadReg(volatile I2C_TypeDef *i2c,
                       uint8_t dev_addr,
                       uint8_t reg,
                       uint8_t *value);

/**
 * I2C_ReadRegs - Đọc nhiều byte liên tiếp từ register (dùng repeated start)
 */
I2C_Status I2C_ReadRegs(volatile I2C_TypeDef *i2c,
                        uint8_t dev_addr,
                        uint8_t reg,
                        uint8_t *buf,
                        uint8_t len);

/**
 * I2C_Recover - Giải phóng bus bị kẹt (BUSY stuck / slave giữ SDA thấp)
 *   Toggle SCL tối đa 9 lần bằng GPIO, generate STOP thủ công, rồi re-init.
 *   Gọi khi I2C_Read/Write trả về I2C_ERR_TIMEOUT liên tiếp.
 */
void I2C_Recover(volatile I2C_TypeDef *i2c,
                 uint32_t pclk1_MHz,
                 uint32_t speed_kHz);

#endif /* __H_I2C_H__ */
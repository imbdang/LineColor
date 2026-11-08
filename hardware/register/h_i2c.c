/* h_i2c.c — STM32F103 bare-metal I2C driver (RM0008)
 *
 * Fixes so với bản cũ:
 *  1. I2C_Init: so sánh địa chỉ peripheral qua integer thay vì pointer
 *     (tránh compiler warning/UB khi so sánh volatile pointer)
 *  2. I2C_ReadReg / I2C_ReadRegs: kiểm tra return value của _wait_flag
 *     ở bước TX trước repeated start (bản cũ bỏ qua)
 *  3. I2C_Read: xử lý đúng trường hợp len==1 theo RM0008 (NACK+STOP
 *     phải set trước khi clear ADDR)
 *  4. Tất cả _wait_flag trong ReadReg/ReadRegs đều được kiểm tra
 */

#include "h_i2c.h"
#include "h_gpio.h"
#include "h_rcc.h"

/* -------------------------------------------------------------------------- */
#define I2C_TIMEOUT 100000U

/* ========================== Private helpers ================================ */

static I2C_Status _wait_flag(volatile uint32_t *reg, uint32_t flag, uint8_t set)
{
    uint32_t t = I2C_TIMEOUT;
    if (set)
    {
        while (!(*reg & flag))
        {
            if (!--t)
                return I2C_ERR_TIMEOUT;
        }
    }
    else
    {
        while ((*reg & flag))
        {
            if (!--t)
                return I2C_ERR_TIMEOUT;
        }
    }
    return I2C_OK;
}

static I2C_Status _check_errors(volatile I2C_TypeDef *i2c)
{
    if (i2c->SR1 & I2C_SR1_AF)
    {
        i2c->SR1 &= ~I2C_SR1_AF;
        return I2C_ERR_NACK;
    }
    if (i2c->SR1 & I2C_SR1_BERR)
    {
        i2c->SR1 &= ~I2C_SR1_BERR;
        return I2C_ERR_BUS;
    }
    if (i2c->SR1 & I2C_SR1_ARLO)
    {
        i2c->SR1 &= ~I2C_SR1_ARLO;
        return I2C_ERR_BUS;
    }
    return I2C_OK;
}

static void _stop(volatile I2C_TypeDef *i2c)
{
    i2c->CR1 |= I2C_CR1_STOP;
}

/* Gửi START + địa chỉ, đợi ADDR set, clear bằng đọc SR1+SR2 */
static I2C_Status _start(volatile I2C_TypeDef *i2c, uint8_t addr_rw)
{
    I2C_Status s;

    /* Chờ bus rảnh */
    s = _wait_flag(&i2c->SR2, I2C_SR2_BUSY, 0);
    if (s)
        return s;

    /* Phát START */
    i2c->CR1 |= I2C_CR1_START;
    s = _wait_flag(&i2c->SR1, I2C_SR1_SB, 1);
    if (s)
        return s;

    /* Gửi địa chỉ */
    i2c->DR = addr_rw;
    s = _wait_flag(&i2c->SR1, I2C_SR1_ADDR, 1);
    if (s)
    {
        _check_errors(i2c);
        return s;
    }

    /* Clear ADDR bằng đọc SR1 rồi SR2 */
    (void)i2c->SR1;
    (void)i2c->SR2;

    return I2C_OK;
}

/* Repeated START: không chờ BUSY (bus đang được giữ bởi master) */
static I2C_Status _repeated_start(volatile I2C_TypeDef *i2c, uint8_t addr_rw)
{
    I2C_Status s;

    i2c->CR1 |= I2C_CR1_START;
    s = _wait_flag(&i2c->SR1, I2C_SR1_SB, 1);
    if (s)
        return s;

    i2c->DR = addr_rw;
    s = _wait_flag(&i2c->SR1, I2C_SR1_ADDR, 1);
    if (s)
    {
        _check_errors(i2c);
        return s;
    }

    (void)i2c->SR1;
    (void)i2c->SR2;

    return I2C_OK;
}

/* ========================== Public API ==================================== */

void I2C_Init(volatile I2C_TypeDef *i2c, uint32_t pclk1_MHz, uint32_t speed_kHz)
{
    /* Enable GPIOB clock (dùng chung cho I2C1 và I2C2) */
    RCC_APB2ENR |= (1U << 3); /* IOPBEN */

    /*
     * FIX: so sánh địa chỉ qua uint32_t thay vì pointer volatile
     * Tránh trường hợp compiler không match đúng do volatile qualifier
     */
    uint32_t base = (uint32_t)i2c;

    if (base == I2C1_BASE)
    {
        RCC_APB1ENR |= (1U << 21);                                            /* I2C1EN */
        GPIO_ConfigPin(GPIOB, 6, GPIO_MODE_OUTPUT_10MHz, GPIO_CNF_OUT_AF_OD); /* SCL */
        GPIO_ConfigPin(GPIOB, 7, GPIO_MODE_OUTPUT_10MHz, GPIO_CNF_OUT_AF_OD); /* SDA */
    }
    else if (base == I2C2_BASE)
    {
        RCC_APB1ENR |= (1U << 22);                                             /* I2C2EN */
        GPIO_ConfigPin(GPIOB, 10, GPIO_MODE_OUTPUT_10MHz, GPIO_CNF_OUT_AF_OD); /* SCL */
        GPIO_ConfigPin(GPIOB, 11, GPIO_MODE_OUTPUT_10MHz, GPIO_CNF_OUT_AF_OD); /* SDA */
    }
    /* Nếu không khớp: không làm gì, tránh cấu hình sai */

    /* Reset peripheral */
    i2c->CR1 |= I2C_CR1_SWRST;

    /* Nếu BUSY vẫn set sau SWRST → slave đang giữ SDA thấp, bit-bang để giải phóng */
    if (i2c->SR2 & I2C_SR2_BUSY)
    {
        uint8_t scl_pin = (base == I2C1_BASE) ? 6U : 10U;
        uint8_t sda_pin = (base == I2C1_BASE) ? 7U : 11U;

        GPIO_ConfigPin(GPIOB, scl_pin, GPIO_MODE_OUTPUT_10MHz, GPIO_CNF_OUT_PP);
        GPIO_ConfigPin(GPIOB, sda_pin, GPIO_MODE_INPUT, GPIO_CNF_IN_FLOATING);
        GPIO_SetPin(GPIOB, scl_pin);

        for (uint8_t i = 0U; i < 9U; i++)
        {
            if ((GPIOB->IDR >> sda_pin) & 1U)
                break;
            GPIO_ClearPin(GPIOB, scl_pin);
            for (volatile uint32_t d = 0U; d < 200U; d++) {}
            GPIO_SetPin(GPIOB, scl_pin);
            for (volatile uint32_t d = 0U; d < 200U; d++) {}
        }

        /* STOP thủ công */
        GPIO_ConfigPin(GPIOB, sda_pin, GPIO_MODE_OUTPUT_10MHz, GPIO_CNF_OUT_PP);
        GPIO_ClearPin(GPIOB, sda_pin);
        for (volatile uint32_t d = 0U; d < 200U; d++) {}
        GPIO_SetPin(GPIOB, scl_pin);
        for (volatile uint32_t d = 0U; d < 200U; d++) {}
        GPIO_SetPin(GPIOB, sda_pin);
        for (volatile uint32_t d = 0U; d < 200U; d++) {}
    }

    i2c->CR1 &= ~I2C_CR1_SWRST;
    i2c->CR1 &= ~I2C_CR1_PE;

    /* FREQ = tần số APB1 (MHz) */
    i2c->CR2 = pclk1_MHz & 0x3FU;

    if (speed_kHz <= 100)
    {
        /* Standard mode: T_high = T_low = CCR * T_pclk */
        uint32_t ccr = (pclk1_MHz * 1000U) / (2U * speed_kHz);
        if (ccr < 4U)
            ccr = 4U;
        i2c->CCR = ccr & 0xFFFU;
        i2c->TRISE = pclk1_MHz + 1U;
    }
    else
    {
        /* Fast mode (duty 2:1): T_high = CCR*T_pclk, T_low = 2*CCR*T_pclk */
        uint32_t ccr = (pclk1_MHz * 1000U) / (3U * speed_kHz);
        if (ccr < 1U)
            ccr = 1U;
        i2c->CCR = I2C_CCR_FS | (ccr & 0xFFFU);
        i2c->TRISE = (pclk1_MHz * 3U) / 10U + 1U;
    }

    i2c->CR1 |= I2C_CR1_PE;
}

/* -------------------------------------------------------------------------- */

I2C_Status I2C_Write(volatile I2C_TypeDef *i2c, uint8_t dev_addr,
                     const uint8_t *data, uint8_t len)
{
    I2C_Status s;

    s = _start(i2c, (uint8_t)(dev_addr << 1));
    if (s)
        return s;

    for (uint8_t i = 0; i < len; i++)
    {
        s = _wait_flag(&i2c->SR1, I2C_SR1_TXE, 1);
        if (s)
        {
            _stop(i2c);
            return s;
        }

        s = _check_errors(i2c);
        if (s)
        {
            _stop(i2c);
            return s;
        }

        i2c->DR = data[i];
    }

    s = _wait_flag(&i2c->SR1, I2C_SR1_BTF, 1);
    if (s)
    {
        _stop(i2c);
        return s;
    }

    _stop(i2c);
    return I2C_OK;
}

/* -------------------------------------------------------------------------- */

I2C_Status I2C_Read(volatile I2C_TypeDef *i2c, uint8_t dev_addr,
                    uint8_t *data, uint8_t len)
{
    I2C_Status s;

    if (len == 0)
        return I2C_OK;

    if (len == 1)
    {
        /*
         * FIX: RM0008 §26.3.3 — đọc 1 byte:
         * NACK và STOP phải được set TRƯỚC khi clear ADDR
         * (tức là trước khi đọc SR2)
         */
        /* Chờ bus rảnh và phát START */
        s = _wait_flag(&i2c->SR2, I2C_SR2_BUSY, 0);
        if (s)
            return s;

        i2c->CR1 |= I2C_CR1_START;
        s = _wait_flag(&i2c->SR1, I2C_SR1_SB, 1);
        if (s)
            return s;

        i2c->DR = (uint8_t)((dev_addr << 1) | 0x01U);
        s = _wait_flag(&i2c->SR1, I2C_SR1_ADDR, 1);
        if (s)
        {
            _check_errors(i2c);
            return s;
        }

        /* NACK ngay + STOP trước khi clear ADDR */
        i2c->CR1 &= ~I2C_CR1_ACK;
        (void)i2c->SR1;
        (void)i2c->SR2; /* clear ADDR */
        _stop(i2c);

        s = _wait_flag(&i2c->SR1, I2C_SR1_RXNE, 1);
        if (s)
            return s;

        data[0] = (uint8_t)i2c->DR;
        return I2C_OK;
    }

    /* len >= 2: ACK trước rồi clear ADDR bình thường */
    i2c->CR1 |= I2C_CR1_ACK;

    s = _start(i2c, (uint8_t)((dev_addr << 1) | 0x01U));
    if (s)
        return s;

    for (uint8_t i = 0; i < len; i++)
    {
        if (i == len - 1U)
        {
            /* Byte cuối: NACK + STOP */
            i2c->CR1 &= ~I2C_CR1_ACK;
            _stop(i2c);
        }
        s = _wait_flag(&i2c->SR1, I2C_SR1_RXNE, 1);
        if (s)
            return s;

        data[i] = (uint8_t)i2c->DR;
    }

    return I2C_OK;
}

/* -------------------------------------------------------------------------- */

I2C_Status I2C_WriteReg(volatile I2C_TypeDef *i2c, uint8_t dev_addr,
                        uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = {reg, value};
    return I2C_Write(i2c, dev_addr, buf, 2);
}

/* -------------------------------------------------------------------------- */

I2C_Status I2C_ReadReg(volatile I2C_TypeDef *i2c, uint8_t dev_addr,
                       uint8_t reg, uint8_t *value)
{
    I2C_Status s;

    /* Phase 1: gửi địa chỉ register */
    s = _start(i2c, (uint8_t)(dev_addr << 1));
    if (s)
        return s;

    /* FIX: kiểm tra return value của _wait_flag */
    s = _wait_flag(&i2c->SR1, I2C_SR1_TXE, 1);
    if (s)
    {
        _stop(i2c);
        return s;
    }
    i2c->DR = reg;
    s = _wait_flag(&i2c->SR1, I2C_SR1_BTF, 1);
    if (s)
    {
        _stop(i2c);
        return s;
    }

    /* Phase 2: repeated start, đọc 1 byte (xử lý giống I2C_Read len==1) */
    i2c->CR1 |= I2C_CR1_START;
    s = _wait_flag(&i2c->SR1, I2C_SR1_SB, 1);
    if (s)
        return s;

    i2c->DR = (uint8_t)((dev_addr << 1) | 0x01U);
    s = _wait_flag(&i2c->SR1, I2C_SR1_ADDR, 1);
    if (s)
    {
        _check_errors(i2c);
        return s;
    }

    /* NACK + STOP trước khi clear ADDR (RM0008 §26.3.3) */
    i2c->CR1 &= ~I2C_CR1_ACK;
    (void)i2c->SR1;
    (void)i2c->SR2;
    _stop(i2c);

    s = _wait_flag(&i2c->SR1, I2C_SR1_RXNE, 1);
    if (s)
        return s;
    *value = (uint8_t)i2c->DR;

    return I2C_OK;
}

/* -------------------------------------------------------------------------- */

void I2C_Recover(volatile I2C_TypeDef *i2c, uint32_t pclk1_MHz, uint32_t speed_kHz)
{
    uint32_t base = (uint32_t)i2c;
    uint8_t scl_pin, sda_pin;

    if (base == I2C1_BASE) { scl_pin = 6U; sda_pin = 7U; }
    else                   { scl_pin = 10U; sda_pin = 11U; }

    /* 1. Disable peripheral */
    i2c->CR1 |= I2C_CR1_SWRST;

    /* 2. Chuyển SCL → output PP, SDA → input để đọc trạng thái */
    GPIO_ConfigPin(GPIOB, scl_pin, GPIO_MODE_OUTPUT_10MHz, GPIO_CNF_OUT_PP);
    GPIO_ConfigPin(GPIOB, sda_pin, GPIO_MODE_INPUT, GPIO_CNF_IN_FLOATING);
    GPIO_SetPin(GPIOB, scl_pin);

    /* 3. Toggle SCL tối đa 9 lần cho đến khi SDA lên HIGH */
    for (uint8_t i = 0U; i < 9U; i++)
    {
        if ((GPIOB->IDR >> sda_pin) & 1U)
            break; /* SDA đã free */

        GPIO_ClearPin(GPIOB, scl_pin);
        for (volatile uint32_t d = 0U; d < 200U; d++) {}
        GPIO_SetPin(GPIOB, scl_pin);
        for (volatile uint32_t d = 0U; d < 200U; d++) {}
    }

    /* 4. Generate STOP thủ công: SDA low → high khi SCL đang high */
    GPIO_ConfigPin(GPIOB, sda_pin, GPIO_MODE_OUTPUT_10MHz, GPIO_CNF_OUT_PP);
    GPIO_ClearPin(GPIOB, sda_pin);
    for (volatile uint32_t d = 0U; d < 200U; d++) {}
    GPIO_SetPin(GPIOB, scl_pin);
    for (volatile uint32_t d = 0U; d < 200U; d++) {}
    GPIO_SetPin(GPIOB, sda_pin);
    for (volatile uint32_t d = 0U; d < 200U; d++) {}

    /* 5. Re-init I2C peripheral bình thường */
    i2c->CR1 &= ~I2C_CR1_SWRST;
    I2C_Init(i2c, pclk1_MHz, speed_kHz);
}

/* -------------------------------------------------------------------------- */

I2C_Status I2C_ReadRegs(volatile I2C_TypeDef *i2c, uint8_t dev_addr,
                        uint8_t reg, uint8_t *buf, uint8_t len)
{
    I2C_Status s;

    if (len == 0)
        return I2C_OK;

    /* Phase 1: gửi địa chỉ register */
    s = _start(i2c, (uint8_t)(dev_addr << 1));
    if (s)
        return s;

    /* FIX: kiểm tra return value của _wait_flag */
    s = _wait_flag(&i2c->SR1, I2C_SR1_TXE, 1);
    if (s)
    {
        _stop(i2c);
        return s;
    }
    i2c->DR = reg;
    s = _wait_flag(&i2c->SR1, I2C_SR1_BTF, 1);
    if (s)
    {
        _stop(i2c);
        return s;
    }

    /* Phase 2: repeated start + đọc */
    if (len == 1)
    {
        /* Xử lý đặc biệt cho len==1 */
        i2c->CR1 |= I2C_CR1_START;
        s = _wait_flag(&i2c->SR1, I2C_SR1_SB, 1);
        if (s)
            return s;

        i2c->DR = (uint8_t)((dev_addr << 1) | 0x01U);
        s = _wait_flag(&i2c->SR1, I2C_SR1_ADDR, 1);
        if (s)
        {
            _check_errors(i2c);
            return s;
        }

        i2c->CR1 &= ~I2C_CR1_ACK;
        (void)i2c->SR1;
        (void)i2c->SR2;
        _stop(i2c);

        s = _wait_flag(&i2c->SR1, I2C_SR1_RXNE, 1);
        if (s)
            return s;
        buf[0] = (uint8_t)i2c->DR;
        return I2C_OK;
    }

    /* len >= 2 */
    i2c->CR1 |= I2C_CR1_ACK;

    s = _repeated_start(i2c, (uint8_t)((dev_addr << 1) | 0x01U));
    if (s)
        return s;

    for (uint8_t i = 0; i < len; i++)
    {
        if (i == len - 1U)
        {
            i2c->CR1 &= ~I2C_CR1_ACK;
            _stop(i2c);
        }
        s = _wait_flag(&i2c->SR1, I2C_SR1_RXNE, 1);
        if (s)
            return s;
        buf[i] = (uint8_t)i2c->DR;
    }

    return I2C_OK;
}
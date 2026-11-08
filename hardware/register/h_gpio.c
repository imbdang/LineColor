#include "h_gpio.h"

void GPIO_SetPin(volatile GPIO_TypeDef *gpio, uint8_t pin)
{
    gpio->BSRR = (1UL << pin);
}

void GPIO_ClearPin(volatile GPIO_TypeDef *gpio, uint8_t pin)
{
    gpio->BRR = (1UL << pin);
}

void GPIO_WritePin(volatile GPIO_TypeDef *gpio, uint8_t pin, uint8_t value)
{
    if (value)
        gpio->BSRR = (1UL << pin);
    else
        gpio->BRR = (1UL << pin);
}

void GPIO_TogglePin(volatile GPIO_TypeDef *gpio, uint8_t pin)
{
    gpio->ODR ^= (1UL << pin);
}

void GPIO_SetMask(volatile GPIO_TypeDef *gpio, uint16_t mask)
{
    gpio->BSRR = (uint32_t)mask;
}

void GPIO_ClearMask(volatile GPIO_TypeDef *gpio, uint16_t mask)
{
    gpio->BRR = (uint32_t)mask;
}

void GPIO_WriteMask(volatile GPIO_TypeDef *gpio, uint16_t mask, uint16_t value)
{
    uint32_t set_bits = (uint32_t)(value & mask);
    uint32_t reset_bits = (uint32_t)(mask & (uint16_t)~value);

    gpio->BSRR = (reset_bits << 16) | set_bits;
}

uint8_t GPIO_ReadPin(volatile GPIO_TypeDef *gpio, uint8_t pin)
{
    return (uint8_t)((gpio->IDR >> pin) & 1U);
}

uint16_t GPIO_ReadPort(volatile GPIO_TypeDef *gpio)
{
    return (uint16_t)gpio->IDR;
}

void GPIO_ConfigPin(volatile GPIO_TypeDef *gpio, uint8_t pin, GPIO_Mode mode, GPIO_Cnf cnf)
{
    volatile uint32_t *cr;
    uint8_t shift;

    if (pin < 8)
    {
        cr = &gpio->CRL;
        shift = pin * 4;
    }
    else
    {
        cr = &gpio->CRH;
        shift = (pin - 8) * 4;
    }

    uint32_t nibble = (((uint32_t)cnf & 0x3U) << 2) | ((uint32_t)mode & 0x3U);

    *cr &= ~(0xFU << shift);
    *cr |= (nibble << shift);
}

void GPIO_ConfigOutputPP(volatile GPIO_TypeDef *gpio, uint8_t pin)
{
    GPIO_ConfigPin(gpio, pin, GPIO_MODE_OUTPUT_2MHz, GPIO_CNF_OUT_PP);
}

void GPIO_ConfigInputPullUp(volatile GPIO_TypeDef *gpio, uint8_t pin)
{
    GPIO_ConfigPin(gpio, pin, GPIO_MODE_INPUT, GPIO_CNF_IN_PUPD);
    gpio->ODR |= (1UL << pin);
}

void GPIO_ConfigInputPullDown(volatile GPIO_TypeDef *gpio, uint8_t pin)
{
    GPIO_ConfigPin(gpio, pin, GPIO_MODE_INPUT, GPIO_CNF_IN_PUPD);
    gpio->ODR &= ~(1UL << pin);
}

void GPIO_ConfigInputFloating(volatile GPIO_TypeDef *gpio, uint8_t pin)
{
    GPIO_ConfigPin(gpio, pin, GPIO_MODE_INPUT, GPIO_CNF_IN_FLOATING);
}

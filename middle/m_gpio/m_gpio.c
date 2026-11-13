#include "m_gpio.h"

#include "h_rcc.h"

static void m_gpio_enable_clock(volatile GPIO_TypeDef *gpio)
{
    uint32_t index = ((uint32_t)gpio - (uint32_t)GPIOA) / 0x400U;

    RCC_APB2_Enable(RCC_APB2ENR_IOPAEN << index);
}

void M_GPIO_InitInputPullUp(volatile GPIO_TypeDef *gpio, uint8_t pin)
{
    m_gpio_enable_clock(gpio);
    GPIO_ConfigInputPullUp(gpio, pin);
}

void M_GPIO_InitInputPullDown(volatile GPIO_TypeDef *gpio, uint8_t pin)
{
    m_gpio_enable_clock(gpio);
    GPIO_ConfigInputPullDown(gpio, pin);
}

void M_GPIO_InitInputFloating(volatile GPIO_TypeDef *gpio, uint8_t pin)
{
    m_gpio_enable_clock(gpio);
    GPIO_ConfigInputFloating(gpio, pin);
}

void M_GPIO_InitOutputPP(volatile GPIO_TypeDef *gpio, uint8_t pin, uint8_t initial_level)
{
    m_gpio_enable_clock(gpio);
    GPIO_WritePin(gpio, pin, initial_level);
    GPIO_ConfigOutputPP(gpio, pin);
}

void M_GPIO_InitAlternatePP(volatile GPIO_TypeDef *gpio, uint8_t pin)
{
    m_gpio_enable_clock(gpio);
    GPIO_ConfigPin(gpio, pin, GPIO_MODE_OUTPUT_2MHz, GPIO_CNF_OUT_AF_PP);
}

uint8_t M_GPIO_Read(volatile GPIO_TypeDef *gpio, uint8_t pin)
{
    return GPIO_ReadPin(gpio, pin);
}

uint16_t M_GPIO_ReadPort(volatile GPIO_TypeDef *gpio)
{
    return GPIO_ReadPort(gpio);
}

void M_GPIO_Write(volatile GPIO_TypeDef *gpio, uint8_t pin, uint8_t level)
{
    GPIO_WritePin(gpio, pin, level);
}

void M_GPIO_WriteMask(volatile GPIO_TypeDef *gpio, uint16_t mask, uint16_t value)
{
    GPIO_WriteMask(gpio, mask, value);
}

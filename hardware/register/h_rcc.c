#include "h_rcc.h"

void RCC_HSE_On(void)
{
    RCC->CR |= (1 << 16);
}

void RCC_HSE_Off(void)
{
    RCC->CR &= ~(1 << 16);
}

void RCC_HSI_On(void)
{
    RCC->CR |= (1 << 0);
}

void RCC_HSI_Off(void)
{
    RCC->CR &= ~(1 << 0);
}

void RCC_PLL_On(void)
{
    RCC->CR |= (1 << 24);
}

void RCC_PLL_Off(void)
{
    RCC->CR &= ~(1 << 24);
}

static uint32_t RCC_PLL_MulBits(uint8_t mul)
{
    if (mul < 2)
    {
        mul = 2;
    }
    if (mul > 16)
    {
        mul = 16;
    }
    return ((uint32_t)(mul - 2) << 18);
}

void RCC_PLL_Config_HSI_Div2(uint8_t mul)
{
    RCC->CFGR &= ~((1U << 16) | (1U << 17) | (0xFU << 18));
    RCC->CFGR |= RCC_PLL_MulBits(mul);
}

void RCC_PLL_Config_HSE(uint8_t mul, uint8_t hse_div2)
{
    RCC->CFGR &= ~((1U << 16) | (1U << 17) | (0xFU << 18));
    RCC->CFGR |= (1U << 16);
    if (hse_div2)
    {
        RCC->CFGR |= (1U << 17);
    }
    RCC->CFGR |= RCC_PLL_MulBits(mul);
}

uint8_t RCC_HSE_Ready(void)
{
    return (RCC->CR & (1U << 17)) ? 1U : 0U;
}

uint8_t RCC_HSI_Ready(void)
{
    return (RCC->CR & (1U << 1)) ? 1U : 0U;
}

uint8_t RCC_PLL_Ready(void)
{
    return (RCC->CR & (1U << 25)) ? 1U : 0U;
}

void RCC_Select_SYSCLK_HSE(void)
{
    RCC->CFGR &= ~(0x3);
    RCC->CFGR |= (1 << 0);
}

void RCC_Select_SYSCLK_HSI(void)
{
    RCC->CFGR &= ~(0x3);
    RCC->CFGR |= (0 << 0);
}

void RCC_Select_SYSCLK_PLL(void)
{
    RCC->CFGR &= ~(0x3);
    RCC->CFGR |= (2 << 0);
}

uint8_t RCC_Get_SYSCLK_Source(void)
{
    return (RCC->CFGR >> 2) & 0x3;
}

void RCC_APB1_Enable(uint32_t mask)
{
    RCC->APB1ENR |= mask;
}

void RCC_APB1_Disable(uint32_t mask)
{
    RCC->APB1ENR &= ~mask;
}

void RCC_APB1_Reset(uint32_t mask)
{
    RCC->APB1RSTR |= mask;
    RCC->APB1RSTR &= ~mask;
}

void RCC_APB2_Enable(uint32_t mask)
{
    RCC->APB2ENR |= mask;
}

void RCC_APB2_Disable(uint32_t mask)
{
    RCC->APB2ENR &= ~mask;
}

void RCC_APB2_Reset(uint32_t mask)
{
    RCC->APB2RSTR |= mask;
    RCC->APB2RSTR &= ~mask;
}

#include "m_clock.h"
#include "h_flash.h"

void M_CLOCK_Set72MHz_HSE_PLL9(void)
{
    RCC_HSI_On();
    while (!RCC_HSI_Ready())
    {
    }

    RCC_Select_SYSCLK_HSI();
    while (RCC_Get_SYSCLK_Source() != 0)
    {
    }

    RCC_HSE_On();
    while (!RCC_HSE_Ready())
    {
    }

    RCC_PLL_Off();
    while (RCC_PLL_Ready())
    {
    }

    /* 48-72 MHz can 2 wait state, phai dat truoc khi tang SYSCLK */
    FLASH_EnablePrefetch();
    FLASH_SetLatency(2U);

    RCC->CFGR &= ~((0xFU << 4) | (0x7U << 8) | (0x7U << 11) | (0x3U << 14));
    RCC->CFGR |= (4U << 8) | (2U << 14);

    RCC->CFGR &= ~((1U << 16) | (1U << 17) | (0xFU << 18));
    RCC->CFGR |= (1U << 16) | (7U << 18);

    RCC_PLL_On();
    while (!RCC_PLL_Ready())
    {
    }

    RCC_Select_SYSCLK_PLL();
    while (RCC_Get_SYSCLK_Source() != 2)
    {
    }
}

#include "h_exti.h"

void EXTI_SelectPort(uint8_t line, uint8_t port_index)
{
    uint8_t reg = (uint8_t)(line / 4U);
    uint8_t shift = (uint8_t)((line % 4U) * 4U);

    AFIO->EXTICR[reg] &= ~(0xFU << shift);
    AFIO->EXTICR[reg] |= ((uint32_t)port_index << shift);
}

void EXTI_SetRising(uint8_t line, uint8_t enable)
{
    if (enable)
        EXTI->RTSR |= (1UL << line);
    else
        EXTI->RTSR &= ~(1UL << line);
}

void EXTI_SetFalling(uint8_t line, uint8_t enable)
{
    if (enable)
        EXTI->FTSR |= (1UL << line);
    else
        EXTI->FTSR &= ~(1UL << line);
}

void EXTI_EnableLine(uint8_t line)
{
    EXTI->IMR |= (1UL << line);
}

uint8_t EXTI_IsPending(uint8_t line)
{
    return (uint8_t)((EXTI->PR >> line) & 1U);
}

void EXTI_ClearPending(uint8_t line)
{
    EXTI->PR = (1UL << line);
}

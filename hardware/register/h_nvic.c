#include "h_nvic.h"

void NVIC_SetPriority(uint8_t irq, uint8_t priority)
{
    /* Priority nam o cac bit cao cua moi byte IPR */
    NVIC_IPR[irq] = (uint8_t)(priority << 5);
}

void NVIC_ClearPending(uint8_t irq)
{
    NVIC_ICPR[irq / 32U] = (1UL << (irq % 32U));
}

void NVIC_EnableIRQ(uint8_t irq)
{
    NVIC_ISER[irq / 32U] = (1UL << (irq % 32U));
}

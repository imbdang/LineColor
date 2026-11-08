#include "m_exti.h"

#include "h_exti.h"
#include "h_nvic.h"
#include "h_rcc.h"

uint8_t M_EXTI_Init(volatile GPIO_TypeDef *gpio,
                    uint8_t pin,
                    M_EXTI_Edge edge,
                    uint8_t irq_priority,
                    Interrupt_Callback callback)
{
    if ((pin < 5U) || (pin > 9U))
    {
        return 1U;
    }

    RCC_APB2_Enable(RCC_APB2ENR_AFIOEN);

    EXTI_SelectPort(pin, (uint8_t)(((uint32_t)gpio - (uint32_t)GPIOA) / 0x400U));
    EXTI_SetRising(pin, (edge != M_EXTI_FALLING) ? 1U : 0U);
    EXTI_SetFalling(pin, (edge != M_EXTI_RISING) ? 1U : 0U);
    EXTI_ClearPending(pin);

    Interrupt_SetExti9_5Callback(pin, callback);

    NVIC_SetPriority(IRQ_EXTI9_5, irq_priority);
    NVIC_ClearPending(IRQ_EXTI9_5);
    EXTI_EnableLine(pin);
    NVIC_EnableIRQ(IRQ_EXTI9_5);

    return 0U;
}

#include "interrupt.h"
#include "h_exti.h"

#define EXTI9_5_FIRST_LINE 5U
#define EXTI9_5_LAST_LINE 9U

static Interrupt_Callback s_exti9_5[EXTI9_5_LAST_LINE - EXTI9_5_FIRST_LINE + 1U];

void Interrupt_SetExti9_5Callback(uint8_t line, Interrupt_Callback callback)
{
    if ((line >= EXTI9_5_FIRST_LINE) && (line <= EXTI9_5_LAST_LINE))
    {
        s_exti9_5[line - EXTI9_5_FIRST_LINE] = callback;
    }
}

void EXTI9_5_IRQHandler(void)
{
    for (uint8_t line = EXTI9_5_FIRST_LINE; line <= EXTI9_5_LAST_LINE; line++)
    {
        if (EXTI_IsPending(line))
        {
            Interrupt_Callback callback = s_exti9_5[line - EXTI9_5_FIRST_LINE];

            EXTI_ClearPending(line);
            if (callback != 0)
            {
                callback();
            }
        }
    }
}

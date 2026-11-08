#ifndef __M_EXTI_H__
#define __M_EXTI_H__

#include "h_gpio.h"
#include "interrupt.h"

typedef enum
{
    M_EXTI_RISING = 0,
    M_EXTI_FALLING,
    M_EXTI_BOTH
} M_EXTI_Edge;

/*
 * M_EXTI_Init - cau hinh line ngat theo chan GPIO va dang ky callback
 *   pin          : 5..9 (vector table hien chi noi EXTI9_5)
 *   irq_priority : 0 (cao nhat) .. 7 (thap nhat)
 *   callback     : chay trong ISR
 * Tra ve 0 neu thanh cong, 1 neu pin khong ho tro.
 */
uint8_t M_EXTI_Init(volatile GPIO_TypeDef *gpio,
                    uint8_t pin,
                    M_EXTI_Edge edge,
                    uint8_t irq_priority,
                    Interrupt_Callback callback);

#endif /*<! __M_EXTI_H__ */

/*PM0056 - Cortex-M3 programming manual*/
#ifndef __NVIC_H__
#define __NVIC_H__

#include <stdint.h>

#define NVIC_ISER ((volatile uint32_t *)0xE000E100)
#define NVIC_ICPR ((volatile uint32_t *)0xE000E280)
#define NVIC_IPR ((volatile uint8_t *)0xE000E400)

#define IRQ_EXTI9_5 23U

/* priority: 0 (cao nhat) .. 7 (thap nhat). Chi dung 3 bit vi chip clone chi co 3 bit. */
void NVIC_SetPriority(uint8_t irq, uint8_t priority);
void NVIC_ClearPending(uint8_t irq);
void NVIC_EnableIRQ(uint8_t irq);

#endif /*<! __NVIC_H__ */

/*RM - rm0008*/
#ifndef __EXTI_H__
#define __EXTI_H__

#include <stdint.h>

/* ====================================== EXTI / AFIO REGISTER ======================================= */
typedef struct
{
    volatile uint32_t IMR;
    volatile uint32_t EMR;
    volatile uint32_t RTSR;
    volatile uint32_t FTSR;
    volatile uint32_t SWIER;
    volatile uint32_t PR;
} EXTI_TypeDef;

typedef struct
{
    volatile uint32_t EVCR;
    volatile uint32_t MAPR;
    volatile uint32_t EXTICR[4];
} AFIO_TypeDef;

#define EXTI ((volatile EXTI_TypeDef *)0x40010400)
#define AFIO ((volatile AFIO_TypeDef *)0x40010000)

/* port_index: 0 = GPIOA, 1 = GPIOB, ... */
void EXTI_SelectPort(uint8_t line, uint8_t port_index);

void EXTI_SetRising(uint8_t line, uint8_t enable);
void EXTI_SetFalling(uint8_t line, uint8_t enable);

void EXTI_EnableLine(uint8_t line);

uint8_t EXTI_IsPending(uint8_t line);
void EXTI_ClearPending(uint8_t line);

#endif /*<! __EXTI_H__ */

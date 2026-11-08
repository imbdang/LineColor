/*RM - rm0008, also ImBdang*/
#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

/* ====================================== TIMER REGISTER ======================================= */
typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RCR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t BDTR;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
} TIM_TypeDef;

#define TIM2 ((volatile TIM_TypeDef *)0x40000000)
#define TIM3 ((volatile TIM_TypeDef *)0x40000400)
#define TIM4 ((volatile TIM_TypeDef *)0x40000800)

typedef enum
{
    TIM_CH1 = 0,
    TIM_CH2,
    TIM_CH3,
    TIM_CH4
} TIM_Channel;

void TIM_Start(volatile TIM_TypeDef *tim);
void TIM_Stop(volatile TIM_TypeDef *tim);

void TIM_SetPSC(volatile TIM_TypeDef *tim, uint16_t psc);
void TIM_SetARR(volatile TIM_TypeDef *tim, uint16_t arr);

void TIM_EnableARRPreload(volatile TIM_TypeDef *tim);
void TIM_GenerateUpdate(volatile TIM_TypeDef *tim);
void TIM_ClearFlags(volatile TIM_TypeDef *tim);

void TIM_CH_PWM1_Config(volatile TIM_TypeDef *tim, TIM_Channel ch, uint8_t preload_en);
void TIM_CH_Enable(volatile TIM_TypeDef *tim, TIM_Channel ch);
void TIM_CH_SetCCR(volatile TIM_TypeDef *tim, TIM_Channel ch, uint16_t ccr);

#endif /*<! __TIMER_H__ */

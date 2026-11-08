#include "h_timer.h"

void TIM_Start(volatile TIM_TypeDef *tim)
{
    tim->CR1 |= (1U << 0);
}

void TIM_Stop(volatile TIM_TypeDef *tim)
{
    tim->CR1 &= ~(1U << 0);
}

void TIM_SetPSC(volatile TIM_TypeDef *tim, uint16_t psc)
{
    tim->PSC = psc;
}

void TIM_SetARR(volatile TIM_TypeDef *tim, uint16_t arr)
{
    tim->ARR = arr;
}

void TIM_EnableARRPreload(volatile TIM_TypeDef *tim)
{
    tim->CR1 |= (1U << 7);
}

void TIM_GenerateUpdate(volatile TIM_TypeDef *tim)
{
    tim->EGR = 1U;
}

void TIM_ClearFlags(volatile TIM_TypeDef *tim)
{
    tim->SR = 0U;
}

void TIM_CH_PWM1_Config(volatile TIM_TypeDef *tim, TIM_Channel ch, uint8_t preload_en)
{
    volatile uint32_t *ccmr = (ch <= TIM_CH2) ? &tim->CCMR1 : &tim->CCMR2;
    uint8_t shift = ((ch == TIM_CH1) || (ch == TIM_CH3)) ? 0U : 8U;

    /* CCxS = 00 (output), OCxM = 110 (PWM mode 1) */
    *ccmr &= ~(0xFFU << shift);
    *ccmr |= ((6U << 4) << shift);
    if (preload_en)
        *ccmr |= ((1U << 3) << shift);
}

void TIM_CH_Enable(volatile TIM_TypeDef *tim, TIM_Channel ch)
{
    tim->CCER |= (1U << ((uint8_t)ch * 4U));
}

void TIM_CH_SetCCR(volatile TIM_TypeDef *tim, TIM_Channel ch, uint16_t ccr)
{
    (&tim->CCR1)[ch] = ccr;
}

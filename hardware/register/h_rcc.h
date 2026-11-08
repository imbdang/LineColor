/*RM - rm0008, also ImBdang*/
#ifndef __RCC_H__
#define __RCC_H__

#include <stdint.h>

/* ====================================== RCC REGISTER ======================================= */
typedef struct
{
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB1RSTR;
    volatile uint32_t AHBENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB1ENR;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
} RCC_TypeDef;

#define RCC ((volatile RCC_TypeDef *)0x40021000)

#define RCC_APB2ENR (RCC->APB2ENR)
#define RCC_APB1ENR (RCC->APB1ENR)

/*<! GPIOx */
#define RCC_APB2ENR_AFIOEN (1U << 0)
#define RCC_APB2ENR_IOPAEN (1U << 2)
#define RCC_APB2ENR_IOPBEN (1U << 3)
#define RCC_APB2ENR_IOPCEN (1U << 4)
#define RCC_APB2ENR_IOPDEN (1U << 5)
#define RCC_APB2ENR_IOPEEN (1U << 6)
#define RCC_APB2ENR_IOPFEN (1U << 7)
#define RCC_APB2ENR_IOPGEN (1U << 8)
#define RCC_APB2ENR_TIM1EN (1U << 11)
#define RCC_APB2ENR_USART1EN (1U << 14)

void RCC_HSE_On(void);
void RCC_HSE_Off(void);
void RCC_HSI_On(void);
void RCC_HSI_Off(void);
void RCC_PLL_On(void);
void RCC_PLL_Off(void);

uint8_t RCC_HSE_Ready(void);
uint8_t RCC_HSI_Ready(void);
uint8_t RCC_PLL_Ready(void);

void RCC_Select_SYSCLK_HSE(void);
void RCC_Select_SYSCLK_HSI(void);
void RCC_Select_SYSCLK_PLL(void);
uint8_t RCC_Get_SYSCLK_Source(void);

void RCC_APB2_Enable(uint32_t mask);
void RCC_APB2_Disable(uint32_t mask);

void RCC_APB1_Enable(uint32_t mask);
void RCC_APB1_Disable(uint32_t mask);

void RCC_APB2_Reset(uint32_t mask);
void RCC_APB1_Reset(uint32_t mask);

#endif /*<! __RCC_H__ */
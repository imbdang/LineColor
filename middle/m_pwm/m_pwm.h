#ifndef __M_PWM_H__
#define __M_PWM_H__

#include "h_gpio.h"
#include "h_timer.h"

typedef struct
{
    volatile TIM_TypeDef *tim; /* TIM2, TIM3 hoac TIM4 */
    TIM_Channel channel;
    volatile GPIO_TypeDef *gpio;
    uint8_t pin;
    uint32_t tim_clk_hz; /* phai chia het cho 1 MHz */
    uint16_t period_us;
    uint16_t initial_pulse_us;
} M_PWM_Config;

/* Counter chay 1 MHz: 1 don vi CCR = 1 us. Xung moi ap dung tu chu ky ke tiep. */
void M_PWM_Init(const M_PWM_Config *config);
void M_PWM_SetPulseUs(volatile TIM_TypeDef *tim, TIM_Channel channel, uint16_t pulse_us);

#endif /*<! __M_PWM_H__ */

#include "m_pwm.h"

#include "h_rcc.h"
#include "m_gpio.h"

#define M_PWM_COUNTER_HZ 1000000U

void M_PWM_Init(const M_PWM_Config *config)
{
    volatile TIM_TypeDef *tim = config->tim;
    /* TIM2/3/4 nam lien tiep tren APB1, bit enable va bit reset la 0/1/2 */
    uint32_t rcc_mask = 1UL << (((uint32_t)tim - (uint32_t)TIM2) / 0x400U);

    M_GPIO_InitAlternatePP(config->gpio, config->pin);

    RCC_APB1_Enable(rcc_mask);
    RCC_APB1_Reset(rcc_mask);

    TIM_SetPSC(tim, (uint16_t)((config->tim_clk_hz / M_PWM_COUNTER_HZ) - 1U));
    TIM_SetARR(tim, (uint16_t)(config->period_us - 1U));
    TIM_CH_SetCCR(tim, config->channel, config->initial_pulse_us);

    TIM_CH_PWM1_Config(tim, config->channel, 1U);
    TIM_CH_Enable(tim, config->channel);

    TIM_EnableARRPreload(tim);
    TIM_GenerateUpdate(tim);
    TIM_ClearFlags(tim);
    TIM_Start(tim);
}

void M_PWM_SetPulseUs(volatile TIM_TypeDef *tim, TIM_Channel channel, uint16_t pulse_us)
{
    TIM_CH_SetCCR(tim, channel, pulse_us);
}

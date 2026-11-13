#include "app.h"
#include "config.h"
#include "hardware.h"
#include "m_gpio.h"
#include "m_pwm.h"
#include "m_time.h"
#include "sm_motor.h"

#include "FreeRTOS.h"
#include "task.h"

static StaticTask_t s_tcb;
static StackType_t s_stack[CFG_TASK_STACK_MOTOR];

static void a_motor_set_duty(uint8_t percent)
{
    M_PWM_SetPulseUs(MOTOR_ENA_TIMER, MOTOR_ENA_CHANNEL,
                     (uint16_t)(((uint32_t)CFG_MOTOR_PWM_PERIOD_US * percent) / 100U));
}

static uint8_t a_motor_read_down_mask(void)
{
    uint8_t mask = 0U;

    if (M_GPIO_Read(BTN_CONV_UP_PORT, BTN_CONV_UP_PIN) == BTN_CONV_ACTIVE_LEVEL)
    {
        mask |= (uint8_t)(1U << SM_MOTOR_BTN_UP);
    }
    if (M_GPIO_Read(BTN_CONV_DOWN_PORT, BTN_CONV_DOWN_PIN) == BTN_CONV_ACTIVE_LEVEL)
    {
        mask |= (uint8_t)(1U << SM_MOTOR_BTN_DOWN);
    }

    return mask;
}

static void a_motor_task(void *arg)
{
    SmMotor sm;
    TickType_t wake = xTaskGetTickCount();

    (void)arg;

    sm_motor_init(&sm);
    a_motor_set_duty(sm.duty_pct);

    for (;;)
    {
        if (sm_motor_update(&sm, a_motor_read_down_mask(), M_TIME_GetMs()))
        {
            a_motor_set_duty(sm.duty_pct);
        }

        (void)xTaskDelayUntil(&wake, pdMS_TO_TICKS(CFG_BUTTON_PERIOD_MS));
    }
}

void a_motor_init(void)
{
    M_PWM_Config pwm = {
        .tim = MOTOR_ENA_TIMER,
        .channel = MOTOR_ENA_CHANNEL,
        .gpio = MOTOR_ENA_PORT,
        .pin = MOTOR_ENA_PIN,
        .tim_clk_hz = CFG_TIM2_CLK_HZ,
        .period_us = CFG_MOTOR_PWM_PERIOD_US,
        .initial_pulse_us = 0U, /* ENA thap den khi task chay */
    };

    /* Giu ENA thap truoc roi moi dat chieu, motor khong giat luc khoi dong */
    M_PWM_Init(&pwm);

    /* Chieu chay co dinh. Bang chuyen chay nguoc thi dao 2 day motor o OUT1, OUT2. */
    M_GPIO_InitOutputPP(MOTOR_IN1_PORT, MOTOR_IN1_PIN, 1U);
    M_GPIO_InitOutputPP(MOTOR_IN2_PORT, MOTOR_IN2_PIN, 0U);

    M_GPIO_InitInputPullDown(BTN_CONV_UP_PORT, BTN_CONV_UP_PIN);
    M_GPIO_InitInputPullDown(BTN_CONV_DOWN_PORT, BTN_CONV_DOWN_PIN);

    (void)xTaskCreateStatic(a_motor_task, "motor", CFG_TASK_STACK_MOTOR, NULL,
                            CFG_TASK_PRIO_MOTOR, s_stack, &s_tcb);
}

#include "app.h"
#include "config.h"
#include "hardware.h"
#include "m_pwm.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

static StaticTask_t s_tcb;
static StackType_t s_stack[CFG_TASK_STACK_SERVO];

static StaticQueue_t s_queue_mem;
static uint8_t s_queue_storage[CFG_SERVO_QUEUE_LEN * sizeof(SmServo_Event)];
static QueueHandle_t s_queue;

static SmServo s_sm;

static void a_servo_task(void *arg)
{
    SmServo_Event event;

    (void)arg;

    for (;;)
    {
        if (xQueueReceive(s_queue, &event, portMAX_DELAY) == pdTRUE)
        {
            sm_servo_step(&s_sm, event);
            M_PWM_SetPulseUs(SERVO_PWM_TIMER, SERVO_PWM_CHANNEL, sm_servo_pulse_us(&s_sm));
        }
    }
}

void a_servo_init(void)
{
    M_PWM_Config pwm = {
        .tim = SERVO_PWM_TIMER,
        .channel = SERVO_PWM_CHANNEL,
        .gpio = SERVO_PWM_PORT,
        .pin = SERVO_PWM_PIN,
        .tim_clk_hz = CFG_TIM4_CLK_HZ,
        .period_us = CFG_SERVO_PERIOD_US,
    };

    sm_servo_init(&s_sm);
    pwm.initial_pulse_us = sm_servo_pulse_us(&s_sm);
    M_PWM_Init(&pwm);

    s_queue = xQueueCreateStatic(CFG_SERVO_QUEUE_LEN, sizeof(SmServo_Event), s_queue_storage, &s_queue_mem);

    (void)xTaskCreateStatic(a_servo_task, "servo", CFG_TASK_STACK_SERVO, NULL,
                            CFG_TASK_PRIO_SERVO, s_stack, &s_tcb);
}

void a_servo_post(SmServo_Event event)
{
    (void)xQueueSend(s_queue, &event, 0U);
}

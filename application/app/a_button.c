#include "app.h"
#include "config.h"
#include "hardware.h"
#include "m_gpio.h"
#include "m_time.h"
#include "sm_button.h"

#include "FreeRTOS.h"
#include "task.h"

typedef struct
{
    volatile GPIO_TypeDef *port;
    uint8_t pin;
} ButtonPin;

static const ButtonPin s_pins[SM_BUTTON_COUNT] = {
    [SM_BUTTON_ZERO] = {BTN_ZERO_PORT, BTN_ZERO_PIN},
    [SM_BUTTON_INC] = {BTN_INC_PORT, BTN_INC_PIN},
    [SM_BUTTON_MAX] = {BTN_MAX_PORT, BTN_MAX_PIN},
    [SM_BUTTON_DEC] = {BTN_DEC_PORT, BTN_DEC_PIN},
};

static StaticTask_t s_tcb;
static StackType_t s_stack[CFG_TASK_STACK_BUTTON];

static uint8_t a_button_read_down_mask(void)
{
    uint8_t mask = 0U;

    for (uint8_t i = 0U; i < (uint8_t)SM_BUTTON_COUNT; i++)
    {
        if (M_GPIO_Read(s_pins[i].port, s_pins[i].pin) == 0U) /* active low */
        {
            mask |= (uint8_t)(1U << i);
        }
    }

    return mask;
}

static void a_button_task(void *arg)
{
    SmButton sm;
    TickType_t wake = xTaskGetTickCount();

    (void)arg;

    sm_button_init(&sm);

    for (;;)
    {
        SmServo_Event event = sm_button_update(&sm, a_button_read_down_mask(), M_TIME_GetMs());

        if (event != SM_SERVO_EV_NONE)
        {
            a_servo_post(event);
        }

        (void)xTaskDelayUntil(&wake, pdMS_TO_TICKS(CFG_BUTTON_PERIOD_MS));
    }
}

void a_button_init(void)
{
    for (uint8_t i = 0U; i < (uint8_t)SM_BUTTON_COUNT; i++)
    {
        M_GPIO_InitInputPullUp(s_pins[i].port, s_pins[i].pin);
    }

    (void)xTaskCreateStatic(a_button_task, "button", CFG_TASK_STACK_BUTTON, NULL,
                            CFG_TASK_PRIO_BUTTON, s_stack, &s_tcb);
}

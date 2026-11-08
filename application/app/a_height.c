#include "app.h"
#include "config.h"
#include "hardware.h"
#include "m_gpio.h"
#include "m_time.h"

#include "FreeRTOS.h"
#include "task.h"

volatile Height_Debug height_debug;

static StaticTask_t s_tcb;
static StackType_t s_stack[CFG_TASK_STACK_HEIGHT];

static void a_height_task(void *arg)
{
    SmHeight sm;
    TickType_t wake = xTaskGetTickCount();

    (void)arg;

    sm_height_init(&sm);

    for (;;)
    {
        uint16_t port = M_GPIO_ReadPort(HEIGHT_PORT); /* doc 2 chan cung luc */
        uint8_t bottom = (((port >> HEIGHT_BOTTOM_PIN) & 1U) == HEIGHT_ACTIVE_LEVEL) ? 1U : 0U;
        uint8_t top = (((port >> HEIGHT_TOP_PIN) & 1U) == HEIGHT_ACTIVE_LEVEL) ? 1U : 0U;
        uint8_t raw = (uint8_t)(bottom | (top << 1));

        sm_height_step(&sm, raw, M_TIME_GetMs());

        height_debug.raw_bits = raw;
        height_debug.stable_bits = sm.stable_bits;
        height_debug.result = sm.result;

        (void)xTaskDelayUntil(&wake, pdMS_TO_TICKS(CFG_HEIGHT_PERIOD_MS));
    }
}

void a_height_init(void)
{
    /* Cam bien chi keo xuong khi co vat, khong co vat thi can pull-up de ve muc cao */
    M_GPIO_InitInputPullUp(HEIGHT_PORT, HEIGHT_BOTTOM_PIN);
    M_GPIO_InitInputPullUp(HEIGHT_PORT, HEIGHT_TOP_PIN);

    (void)xTaskCreateStatic(a_height_task, "height", CFG_TASK_STACK_HEIGHT, NULL,
                            CFG_TASK_PRIO_HEIGHT, s_stack, &s_tcb);
}

Height_Result a_height_get_result(void)
{
    return height_debug.result;
}

uint8_t a_height_object_present(void)
{
    return (uint8_t)(height_debug.stable_bits & 1U);
}

uint8_t a_height_object_tall(void)
{
    return (uint8_t)((height_debug.stable_bits >> 1) & 1U);
}

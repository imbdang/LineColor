#include "app.h"
#include "config.h"
#include "m_clock.h"

#include "FreeRTOS.h"
#include "task.h"

void vApplicationGetIdleTaskMemory(StaticTask_t **tcb, StackType_t **stack, configSTACK_DEPTH_TYPE *stack_size)
{
    static StaticTask_t s_idle_tcb;
    static StackType_t s_idle_stack[configMINIMAL_STACK_SIZE];

    *tcb = &s_idle_tcb;
    *stack = s_idle_stack;
    *stack_size = configMINIMAL_STACK_SIZE;
}

void vApplicationStackOverflowHook(TaskHandle_t task, char *name)
{
    (void)task;
    (void)name;

    taskDISABLE_INTERRUPTS();
    for (;;)
    {
    }
}

void app_run(void)
{
    M_CLOCK_Set72MHz_HSE_PLL9();

    a_motor_init(); /* som nhat co the de keo ENA xuong thap */
    a_servo_init();
    a_button_init();
    a_height_init();
    a_color_init();

    vTaskStartScheduler();

    for (;;)
    {
    }
}

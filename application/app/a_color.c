#include "app.h"
#include "config.h"
#include "hardware.h"
#include "m_exti.h"
#include "m_gpio.h"
#include "m_i2c.h"
#include "m_sensor.h"
#include "m_time.h"
#include "sm_color.h"
#include "sm_sort.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

volatile Color_Debug color_debug;

static StaticTask_t s_tcb;
static StackType_t s_stack[CFG_TASK_STACK_COLOR];

static StaticSemaphore_t s_sem_mem;
static SemaphoreHandle_t s_int_sem;

/* Chay trong ISR: chi bao hieu, khong dung I2C */
static void a_color_on_int(void)
{
    BaseType_t woken = pdFALSE;

    (void)xSemaphoreGiveFromISR(s_int_sem, &woken);
    portYIELD_FROM_ISR(woken);
}

static void a_color_write_output(Color_table code)
{
    uint16_t mask = (uint16_t)((1U << COLOR_OUT_MSB_PIN) | (1U << COLOR_OUT_LSB_PIN));
    uint16_t value = 0U;

    if (((uint32_t)code & 2U) != 0U)
    {
        value |= (uint16_t)(1U << COLOR_OUT_MSB_PIN);
    }
    if (((uint32_t)code & 1U) != 0U)
    {
        value |= (uint16_t)(1U << COLOR_OUT_LSB_PIN);
    }

    /* 2 bit doi trong cung 1 lan ghi, ben doc khong thay ma trung gian */
    M_GPIO_WriteMask(COLOR_OUT_PORT, mask, value);
}

static SmColor_Event a_color_sensor_start(void)
{
    uint8_t id = 0U;

    if (TCS34_ReadID(TCS_I2C, &id) != I2C_OK)
    {
        return SM_COLOR_EV_INIT_FAIL;
    }

    color_debug.sensor_id = id;
    if ((id != TCS34_ID_34725) && (id != TCS34_ID_34727))
    {
        return SM_COLOR_EV_INIT_FAIL;
    }

    if (TCS34_Init(TCS_I2C, CFG_TCS34_ATIME, CFG_TCS34_GAIN) != I2C_OK)
    {
        return SM_COLOR_EV_INIT_FAIL;
    }

    return (TCS34_EnableInterrupt(TCS_I2C) == I2C_OK) ? SM_COLOR_EV_INIT_OK : SM_COLOR_EV_INIT_FAIL;
}

static SmColor_Event a_color_read_sample(Color_Sample *sample)
{
    const uint8_t ready = TCS34_STATUS_AVALID | TCS34_STATUS_AINT;
    uint8_t status = 0U;
    TCS34_Data data;

    if (TCS34_ReadStatus(TCS_I2C, &status) != I2C_OK)
    {
        return SM_COLOR_EV_BUS_ERROR;
    }

    if ((status & ready) != ready)
    {
        return (TCS34_ClearInterrupt(TCS_I2C) == I2C_OK) ? SM_COLOR_EV_TICK : SM_COLOR_EV_BUS_ERROR;
    }

    if ((TCS34_Read(TCS_I2C, &data) != I2C_OK) || (TCS34_ClearInterrupt(TCS_I2C) != I2C_OK))
    {
        return SM_COLOR_EV_BUS_ERROR;
    }

    color_remove_ir(&data.c, &data.r, &data.g, &data.b);
    data.r = color_weight_red(data.r);
    data.g = color_weight_green(data.g);

    sample->c = data.c;
    sample->r = data.r;
    sample->g = data.g;
    sample->b = data.b;
    sample->is_tall = a_height_object_tall();

    color_debug.c = data.c;
    color_debug.r = data.r;
    color_debug.g = data.g;
    color_debug.b = data.b;
    color_debug.sample_count++;

    return SM_COLOR_EV_SAMPLE;
}

static void a_color_set_led(uint8_t on)
{
    uint8_t off_level = (TCS_LED_ON_LEVEL != 0U) ? 0U : 1U;

    M_GPIO_Write(TCS_LED_PORT, TCS_LED_PIN, (on != 0U) ? TCS_LED_ON_LEVEL : off_level);
}

static void a_color_task(void *arg)
{
    SmColor sm;
    SmSort sort;
    uint8_t led_on = 0U;

    (void)arg;

#if CFG_TEST_LED_BLINK
    for (;;)
    {
        M_GPIO_Write(TCS_LED_PORT, TCS_LED_PIN, TCS_LED_ON_LEVEL);
        M_TIME_DelayMs(CFG_TEST_LED_BLINK_MS);
        M_GPIO_Write(TCS_LED_PORT, TCS_LED_PIN, (TCS_LED_ON_LEVEL != 0U) ? 0U : 1U);
        M_TIME_DelayMs(CFG_TEST_LED_BLINK_MS);
    }
#endif

    M_I2C_Init(TCS_I2C, CFG_PCLK1_MHZ, CFG_I2C_SPEED_KHZ);
    sm_color_init(&sm, M_TIME_GetMs());
    sm_sort_init(&sort);

    for (;;)
    {
        SmColor_State previous = sm.state;
        SmColor_Event event;
        Color_Sample sample = {0};

        switch (sm.state)
        {
        case SM_COLOR_INIT:
            event = a_color_sensor_start();
            break;

        case SM_COLOR_FAULT:
            M_TIME_DelayMs(CFG_COLOR_WAIT_MS);
            event = SM_COLOR_EV_TICK;
            break;

        default:
            /* INT co the da o muc thap tu truoc, luc do khong co canh xuong */
            if ((xSemaphoreTake(s_int_sem, pdMS_TO_TICKS(CFG_COLOR_WAIT_MS)) == pdTRUE) ||
                (M_GPIO_Read(TCS_INT_PORT, TCS_INT_PIN) == 0U))
            {
                event = a_color_read_sample(&sample);
            }
            else
            {
                event = SM_COLOR_EV_TICK;
            }
            break;
        }

        sm_color_step(&sm, event, &sample, M_TIME_GetMs());

        if ((previous == SM_COLOR_FAULT) && (sm.state == SM_COLOR_INIT))
        {
            M_I2C_Recover(TCS_I2C, CFG_PCLK1_MHZ, CFG_I2C_SPEED_KHZ);
        }
        if ((sm.state == SM_COLOR_FAULT) && (previous != SM_COLOR_FAULT))
        {
            color_debug.error_count++;
        }

        a_color_write_output(sm.output);
        color_debug.result = sm.output;
        color_debug.state = (uint8_t)sm.state;

        SmServo_Event servo_event = sm_sort_update(&sort, a_height_object_present(),
                                                   a_height_object_tall(), sm.output, M_TIME_GetMs());

        if (servo_event != SM_SERVO_EV_NONE)
        {
            a_servo_post(servo_event);
        }

        /* Chi bat LED va do mau khi cam bien duoi bao co vat */
        uint8_t measuring = sm_sort_is_measuring(&sort);

        if (measuring != led_on)
        {
            led_on = measuring;
            a_color_set_led(led_on);
            sm_color_set_active(&sm, led_on, M_TIME_GetMs());
        }
    }
}

void a_color_init(void)
{
    uint8_t led_off_level = (TCS_LED_ON_LEVEL != 0U) ? 0U : 1U;

    /* Dat 00 truoc khi bat output */
    M_GPIO_InitOutputPP(COLOR_OUT_PORT, COLOR_OUT_MSB_PIN, 0U);
    M_GPIO_InitOutputPP(COLOR_OUT_PORT, COLOR_OUT_LSB_PIN, 0U);
    M_GPIO_InitOutputPP(TCS_LED_PORT, TCS_LED_PIN, led_off_level);

    s_int_sem = xSemaphoreCreateBinaryStatic(&s_sem_mem);

    M_GPIO_InitInputPullUp(TCS_INT_PORT, TCS_INT_PIN);
    (void)M_EXTI_Init(TCS_INT_PORT, TCS_INT_PIN, M_EXTI_FALLING, CFG_IRQ_PRIO_COLOR_INT, a_color_on_int);

    (void)xTaskCreateStatic(a_color_task, "color", CFG_TASK_STACK_COLOR, NULL,
                            CFG_TASK_PRIO_COLOR, s_stack, &s_tcb);
}

#include "sm_sort.h"
#include "config.h"

static SmServo_Event sm_sort_target(Color_table color, uint8_t is_tall)
{
    switch (color)
    {
    case C_RED:
        return is_tall ? CFG_SORT_SERVO_RED_HIGH : CFG_SORT_SERVO_RED_LOW;
    case C_GREEN:
        return is_tall ? CFG_SORT_SERVO_GREEN_HIGH : CFG_SORT_SERVO_GREEN_LOW;
    case C_BLUE:
        return is_tall ? CFG_SORT_SERVO_BLUE_HIGH : CFG_SORT_SERVO_BLUE_LOW;
    default:
        return SM_SERVO_EV_NONE;
    }
}

void sm_sort_init(SmSort *sm)
{
    sm->state = SM_SORT_IDLE;
    sm->pending = SM_SERVO_EV_NONE;
    sm->last_sent = SM_SERVO_EV_NONE;
    sm->since_ms = 0U;
}

SmServo_Event sm_sort_update(SmSort *sm, uint8_t present, uint8_t tall, Color_table color, uint32_t now_ms)
{
    if (present == 0U)
    {
        sm_sort_init(sm);
        return SM_SERVO_EV_NONE;
    }

    sm->state = SM_SORT_MEASURING;

    /* C_NONE chi la luc dang doi xac nhan mau, giu nguyen pending */
    if (color != C_NONE)
    {
        SmServo_Event target = sm_sort_target(color, tall);

        if (target != sm->pending)
        {
            sm->pending = target;
            sm->since_ms = now_ms;
        }
    }

    if ((sm->pending == SM_SERVO_EV_NONE) || (sm->pending == sm->last_sent) ||
        ((uint32_t)(now_ms - sm->since_ms) < CFG_SORT_DECISION_MS))
    {
        return SM_SERVO_EV_NONE;
    }

    sm->last_sent = sm->pending;
    return sm->pending;
}

uint8_t sm_sort_is_measuring(const SmSort *sm)
{
    return (sm->state == SM_SORT_MEASURING) ? 1U : 0U;
}

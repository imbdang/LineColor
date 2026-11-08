#include "sm_button.h"
#include "config.h"

/* Tra ve 1 khi vua xac nhan mot lan nhan moi */
static uint8_t sm_button_item_step(SmButton_Item *item, uint8_t is_down, uint32_t now_ms)
{
    uint8_t pressed = 0U;

    switch (item->state)
    {
    case SM_BUTTON_RELEASED:
        if (is_down)
        {
            item->state = SM_BUTTON_PRESS_DEBOUNCE;
            item->since_ms = now_ms;
        }
        break;

    case SM_BUTTON_PRESS_DEBOUNCE:
        if (!is_down)
        {
            item->state = SM_BUTTON_RELEASED;
        }
        else if ((uint32_t)(now_ms - item->since_ms) >= CFG_BUTTON_DEBOUNCE_MS)
        {
            item->state = SM_BUTTON_PRESSED;
            pressed = 1U;
        }
        break;

    case SM_BUTTON_PRESSED:
        if (!is_down)
        {
            item->state = SM_BUTTON_RELEASE_DEBOUNCE;
            item->since_ms = now_ms;
        }
        break;

    case SM_BUTTON_RELEASE_DEBOUNCE:
        if (is_down)
        {
            item->state = SM_BUTTON_PRESSED;
        }
        else if ((uint32_t)(now_ms - item->since_ms) >= CFG_BUTTON_DEBOUNCE_MS)
        {
            item->state = SM_BUTTON_RELEASED;
        }
        break;

    default:
        break;
    }

    return pressed;
}

void sm_button_init(SmButton *sm)
{
    for (uint8_t i = 0U; i < (uint8_t)SM_BUTTON_COUNT; i++)
    {
        sm->items[i].state = SM_BUTTON_RELEASED;
        sm->items[i].since_ms = 0U;
    }
}

SmServo_Event sm_button_update(SmButton *sm, uint8_t down_mask, uint32_t now_ms)
{
    uint8_t press_events = 0U;
    uint8_t held = 0U;

    for (uint8_t i = 0U; i < (uint8_t)SM_BUTTON_COUNT; i++)
    {
        SmButton_Item *item = &sm->items[i];

        if (sm_button_item_step(item, (uint8_t)((down_mask >> i) & 1U), now_ms))
        {
            press_events |= (uint8_t)(1U << i);
        }
        if ((item->state == SM_BUTTON_PRESSED) || (item->state == SM_BUTTON_RELEASE_DEBOUNCE))
        {
            held |= (uint8_t)(1U << i);
        }
    }

    /* Bo qua to hop phim. Tha mot nut khac khong duoc tinh la nhan moi. */
    if ((press_events == 0U) || (press_events != held))
    {
        return SM_SERVO_EV_NONE;
    }

    switch (press_events)
    {
    case (1U << SM_BUTTON_ZERO):
        return SM_SERVO_EV_ZERO;
    case (1U << SM_BUTTON_INC):
        return SM_SERVO_EV_INC;
    case (1U << SM_BUTTON_MAX):
        return SM_SERVO_EV_MAX;
    case (1U << SM_BUTTON_DEC):
        return SM_SERVO_EV_DEC;
    default:
        return SM_SERVO_EV_NONE;
    }
}

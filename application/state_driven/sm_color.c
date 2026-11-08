#include "sm_color.h"
#include "config.h"

static void sm_color_enter(SmColor *sm, SmColor_State state, uint32_t now_ms)
{
    sm->state = state;
    sm->state_since_ms = now_ms;
}

static void sm_color_clear_result(SmColor *sm)
{
    sm->output = C_NONE;
    sm->candidate = C_NONE;
    sm->candidate_count = 0U;
}

static void sm_color_fault(SmColor *sm, uint32_t now_ms)
{
    sm_color_clear_result(sm);
    sm_color_enter(sm, SM_COLOR_FAULT, now_ms);
}

static void sm_color_on_sample(SmColor *sm, const Color_Sample *sample, uint32_t now_ms)
{
    Color_table code = color_classify(sample);

    if (code == C_NONE)
    {
        sm_color_clear_result(sm);
        sm_color_enter(sm, SM_COLOR_SAMPLING, now_ms);
        return;
    }

    if (code != sm->candidate)
    {
        sm->candidate = code;
        sm->candidate_count = 1U;
        sm->output = C_NONE;
        sm_color_enter(sm, SM_COLOR_SAMPLING, now_ms);
    }
    else if (sm->candidate_count < CFG_COLOR_CONFIRM_SAMPLES)
    {
        sm->candidate_count++;
    }

    if (sm->candidate_count >= CFG_COLOR_CONFIRM_SAMPLES)
    {
        sm->output = code;
        if (sm->state != SM_COLOR_CONFIRMED)
        {
            sm_color_enter(sm, SM_COLOR_CONFIRMED, now_ms);
        }
    }
}

void sm_color_init(SmColor *sm, uint32_t now_ms)
{
    sm_color_clear_result(sm);
    sm->settle_count = 0U;
    sm->active = 0U;
    sm->last_sample_ms = now_ms;
    sm_color_enter(sm, SM_COLOR_INIT, now_ms);
}

void sm_color_set_active(SmColor *sm, uint8_t active, uint32_t now_ms)
{
    sm->active = active;

    if (active != 0U)
    {
        if (sm->state == SM_COLOR_PAUSED)
        {
            sm->settle_count = 0U;
            sm_color_enter(sm, SM_COLOR_LED_SETTLE, now_ms);
        }
    }
    else if ((sm->state == SM_COLOR_LED_SETTLE) || (sm->state == SM_COLOR_SAMPLING) ||
             (sm->state == SM_COLOR_CONFIRMED))
    {
        sm_color_clear_result(sm);
        sm_color_enter(sm, SM_COLOR_PAUSED, now_ms);
    }
}

void sm_color_step(SmColor *sm, SmColor_Event event, const Color_Sample *sample, uint32_t now_ms)
{
    switch (sm->state)
    {
    case SM_COLOR_INIT:
        if (event == SM_COLOR_EV_INIT_OK)
        {
            sm->last_sample_ms = now_ms;
            sm->settle_count = 0U;
            sm_color_enter(sm, (sm->active != 0U) ? SM_COLOR_LED_SETTLE : SM_COLOR_PAUSED, now_ms);
        }
        else if (event == SM_COLOR_EV_INIT_FAIL)
        {
            sm_color_fault(sm, now_ms);
        }
        break;

    case SM_COLOR_FAULT:
        if ((event == SM_COLOR_EV_TICK) &&
            ((uint32_t)(now_ms - sm->state_since_ms) >= CFG_COLOR_RETRY_MS))
        {
            sm_color_enter(sm, SM_COLOR_INIT, now_ms);
        }
        break;

    case SM_COLOR_PAUSED:
        /* Van doc mau de giu ngat chay va phat hien mat cam bien */
        if (event == SM_COLOR_EV_BUS_ERROR)
        {
            sm_color_fault(sm, now_ms);
        }
        else if (event == SM_COLOR_EV_SAMPLE)
        {
            sm->last_sample_ms = now_ms;
        }
        else if ((uint32_t)(now_ms - sm->last_sample_ms) >= CFG_COLOR_STALE_MS)
        {
            sm_color_fault(sm, now_ms);
        }
        break;

    case SM_COLOR_LED_SETTLE:
    case SM_COLOR_SAMPLING:
    case SM_COLOR_CONFIRMED:
        if (event == SM_COLOR_EV_BUS_ERROR)
        {
            sm_color_fault(sm, now_ms);
        }
        else if (event == SM_COLOR_EV_SAMPLE)
        {
            sm->last_sample_ms = now_ms;
            if ((sm->state == SM_COLOR_LED_SETTLE) &&
                (sm->settle_count < CFG_COLOR_LED_SETTLE_SAMPLES))
            {
                sm->settle_count++;
            }
            else
            {
                sm_color_on_sample(sm, sample, now_ms);
            }
        }
        else if ((uint32_t)(now_ms - sm->last_sample_ms) >= CFG_COLOR_STALE_MS)
        {
            sm_color_fault(sm, now_ms); /* mat ngat hoac mat du lieu, khong giu ma cu */
        }
        break;

    default:
        break;
    }
}

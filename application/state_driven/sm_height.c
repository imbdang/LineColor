#include "sm_height.h"
#include "config.h"

static Height_Result sm_height_decode(uint8_t bits)
{
    switch (bits)
    {
    case 1U:
        return HEIGHT_LOW;
    case 3U:
        return HEIGHT_HIGH;
    case 2U:
        return HEIGHT_FAULT;
    default:
        return HEIGHT_IDLE;
    }
}

void sm_height_init(SmHeight *sm)
{
    sm->state = SM_HEIGHT_STABLE;
    sm->stable_bits = 0U;
    sm->last_bits = 0U;
    sm->result = HEIGHT_IDLE;
    sm->since_ms = 0U;
}

void sm_height_step(SmHeight *sm, uint8_t raw_bits, uint32_t now_ms)
{
    switch (sm->state)
    {
    case SM_HEIGHT_STABLE:
        if (raw_bits != sm->stable_bits)
        {
            sm->state = SM_HEIGHT_FILTERING;
            sm->last_bits = raw_bits;
            sm->since_ms = now_ms;
        }
        break;

    case SM_HEIGHT_FILTERING:
        if (raw_bits == sm->stable_bits)
        {
            sm->state = SM_HEIGHT_STABLE; /* chi la nhieu, ket qua giu nguyen */
        }
        else if (raw_bits != sm->last_bits)
        {
            sm->last_bits = raw_bits;
            sm->since_ms = now_ms;
        }
        else if ((uint32_t)(now_ms - sm->since_ms) >= CFG_HEIGHT_FILTER_MS)
        {
            sm->state = SM_HEIGHT_STABLE;
            sm->stable_bits = raw_bits;
            sm->result = sm_height_decode(raw_bits);
        }
        break;

    default:
        break;
    }
}

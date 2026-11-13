#include "sm_motor.h"
#include "config.h"

void sm_motor_init(SmMotor *sm)
{
    sm_button_items_init(sm->buttons, (uint8_t)SM_MOTOR_BTN_COUNT);
    sm->duty_pct = CFG_MOTOR_DUTY_INIT_PCT;
}

uint8_t sm_motor_update(SmMotor *sm, uint8_t down_mask, uint32_t now_ms)
{
    uint8_t previous = sm->duty_pct;

    switch (sm_button_scan(sm->buttons, (uint8_t)SM_MOTOR_BTN_COUNT, down_mask, now_ms))
    {
    case (1U << SM_MOTOR_BTN_UP):
        sm->duty_pct = ((sm->duty_pct + CFG_MOTOR_DUTY_STEP_PCT) <= CFG_MOTOR_DUTY_MAX_PCT)
                           ? (uint8_t)(sm->duty_pct + CFG_MOTOR_DUTY_STEP_PCT)
                           : CFG_MOTOR_DUTY_MAX_PCT;
        break;

    case (1U << SM_MOTOR_BTN_DOWN):
        sm->duty_pct = (sm->duty_pct >= (CFG_MOTOR_DUTY_MIN_PCT + CFG_MOTOR_DUTY_STEP_PCT))
                           ? (uint8_t)(sm->duty_pct - CFG_MOTOR_DUTY_STEP_PCT)
                           : CFG_MOTOR_DUTY_MIN_PCT;
        break;

    default:
        break;
    }

    return (sm->duty_pct != previous) ? 1U : 0U;
}

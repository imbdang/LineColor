#include "sm_servo.h"
#include "config.h"

static const uint16_t s_pulse_us[SM_SERVO_STEP_COUNT] = CFG_SERVO_PULSE_US;

void sm_servo_init(SmServo *sm)
{
    sm->state = SM_SERVO_STEP_0;
}

void sm_servo_step(SmServo *sm, SmServo_Event event)
{
    switch (event)
    {
    case SM_SERVO_EV_ZERO:
        sm->state = SM_SERVO_STEP_0;
        break;

    case SM_SERVO_EV_INC:
        sm->state = (sm->state < SM_SERVO_STEP_5) ? (SmServo_State)(sm->state + 1) : SM_SERVO_STEP_0;
        break;

    case SM_SERVO_EV_MAX:
        sm->state = SM_SERVO_STEP_5;
        break;

    case SM_SERVO_EV_DEC:
        sm->state = (sm->state > SM_SERVO_STEP_0) ? (SmServo_State)(sm->state - 1) : SM_SERVO_STEP_0;
        break;

    case SM_SERVO_EV_GOTO_0:
    case SM_SERVO_EV_GOTO_1:
    case SM_SERVO_EV_GOTO_2:
    case SM_SERVO_EV_GOTO_3:
    case SM_SERVO_EV_GOTO_4:
    case SM_SERVO_EV_GOTO_5:
        sm->state = (SmServo_State)(event - SM_SERVO_EV_GOTO_0);
        break;

    case SM_SERVO_EV_NONE:
    default:
        break;
    }
}

uint16_t sm_servo_pulse_us(const SmServo *sm)
{
    return s_pulse_us[sm->state];
}

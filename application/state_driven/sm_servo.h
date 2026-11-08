#ifndef __SM_SERVO_H__
#define __SM_SERVO_H__

#include <stdint.h>

typedef enum
{
    SM_SERVO_STEP_0 = 0, /* 0 do   */
    SM_SERVO_STEP_1,     /* 36 do  */
    SM_SERVO_STEP_2,     /* 72 do  */
    SM_SERVO_STEP_3,     /* 108 do */
    SM_SERVO_STEP_4,     /* 144 do */
    SM_SERVO_STEP_5,     /* 180 do */
    SM_SERVO_STEP_COUNT
} SmServo_State;

typedef enum
{
    SM_SERVO_EV_NONE = 0,
    SM_SERVO_EV_ZERO,
    SM_SERVO_EV_INC, /* +1 buoc, tu cuoi quay ve 0 */
    SM_SERVO_EV_MAX,
    SM_SERVO_EV_DEC, /* -1 buoc, dung o 0 */
    SM_SERVO_EV_GOTO_0, /* di thang toi buoc 0..5 */
    SM_SERVO_EV_GOTO_1,
    SM_SERVO_EV_GOTO_2,
    SM_SERVO_EV_GOTO_3,
    SM_SERVO_EV_GOTO_4,
    SM_SERVO_EV_GOTO_5
} SmServo_Event;

typedef struct
{
    SmServo_State state;
} SmServo;

void sm_servo_init(SmServo *sm);
void sm_servo_step(SmServo *sm, SmServo_Event event);
uint16_t sm_servo_pulse_us(const SmServo *sm);

#endif /* __SM_SERVO_H__ */

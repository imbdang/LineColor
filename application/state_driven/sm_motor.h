#ifndef __SM_MOTOR_H__
#define __SM_MOTOR_H__

#include "sm_button.h"
#include <stdint.h>

/* Thu tu nay la vi tri bit trong down_mask */
typedef enum
{
    SM_MOTOR_BTN_UP = 0,
    SM_MOTOR_BTN_DOWN,
    SM_MOTOR_BTN_COUNT
} SmMotor_ButtonId;

typedef struct
{
    SmButton_Item buttons[SM_MOTOR_BTN_COUNT];
    uint8_t duty_pct;
} SmMotor;

/* duty_pct bat dau tu CFG_MOTOR_DUTY_INIT_PCT */
void sm_motor_init(SmMotor *sm);

/*
 * Moi lan nhan tang hoac giam CFG_MOTOR_DUTY_STEP_PCT, dung o MIN va MAX.
 * Tra ve 1 khi duty_pct vua doi.
 */
uint8_t sm_motor_update(SmMotor *sm, uint8_t down_mask, uint32_t now_ms);

#endif /* __SM_MOTOR_H__ */

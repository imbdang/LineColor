#ifndef __SM_BUTTON_H__
#define __SM_BUTTON_H__

#include "sm_servo.h"
#include <stdint.h>

/* Thu tu nay la vi tri bit trong down_mask */
typedef enum
{
    SM_BUTTON_ZERO = 0,
    SM_BUTTON_INC,
    SM_BUTTON_MAX,
    SM_BUTTON_DEC,
    SM_BUTTON_COUNT
} SmButton_Id;

typedef enum
{
    SM_BUTTON_RELEASED = 0,
    SM_BUTTON_PRESS_DEBOUNCE,
    SM_BUTTON_PRESSED,
    SM_BUTTON_RELEASE_DEBOUNCE
} SmButton_State;

typedef struct
{
    SmButton_State state;
    uint32_t since_ms;
} SmButton_Item;

typedef struct
{
    SmButton_Item items[SM_BUTTON_COUNT];
} SmButton;

void sm_button_init(SmButton *sm);

/*
 * Moi lan nhan chi tao 1 su kien, giu nut khong lap lai.
 * Bam nhieu nut cung luc thi bo qua.
 */
SmServo_Event sm_button_update(SmButton *sm, uint8_t down_mask, uint32_t now_ms);

#endif /* __SM_BUTTON_H__ */

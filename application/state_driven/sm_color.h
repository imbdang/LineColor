#ifndef __SM_COLOR_H__
#define __SM_COLOR_H__

#include "color_classify.h"
#include <stdint.h>

typedef enum
{
    SM_COLOR_INIT = 0,   /* cho khoi tao cam bien */
    SM_COLOR_LED_SETTLE, /* bo mau khi LED chua on dinh */
    SM_COLOR_SAMPLING,   /* dang doi du mau xac nhan */
    SM_COLOR_CONFIRMED,  /* dang xuat ma mau */
    SM_COLOR_FAULT,      /* loi, xuat 00, cho thu lai */
    SM_COLOR_PAUSED      /* khong co vat, LED tat, khong phan loai */
} SmColor_State;

typedef enum
{
    SM_COLOR_EV_INIT_OK = 0,
    SM_COLOR_EV_INIT_FAIL,
    SM_COLOR_EV_SAMPLE,    /* co mau moi, sample khac NULL */
    SM_COLOR_EV_BUS_ERROR, /* loi I2C khi doc */
    SM_COLOR_EV_TICK       /* khong co gi moi, chi kiem tra timeout */
} SmColor_Event;

typedef struct
{
    SmColor_State state;
    Color_table output;
    Color_table candidate;
    uint8_t candidate_count;
    uint8_t settle_count;
    uint8_t active;
    uint32_t state_since_ms;
    uint32_t last_sample_ms;
} SmColor;

void sm_color_init(SmColor *sm, uint32_t now_ms);

/* active = 1 khi LED bat va can do mau, 0 de tam dung */
void sm_color_set_active(SmColor *sm, uint8_t active, uint32_t now_ms);
void sm_color_step(SmColor *sm, SmColor_Event event, const Color_Sample *sample, uint32_t now_ms);

#endif /* __SM_COLOR_H__ */

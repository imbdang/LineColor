#ifndef __SM_HEIGHT_H__
#define __SM_HEIGHT_H__

#include <stdint.h>

typedef enum
{
    HEIGHT_IDLE = 0, /* 00: khong co vat */
    HEIGHT_LOW,      /* 01: chi cam bien duoi */
    HEIGHT_HIGH,     /* 11: ca hai cam bien */
    HEIGHT_FAULT     /* 10: chi cam bien tren, bat thuong */
} Height_Result;

typedef enum
{
    SM_HEIGHT_STABLE = 0,
    SM_HEIGHT_FILTERING
} SmHeight_State;

typedef struct
{
    SmHeight_State state;
    uint8_t stable_bits;
    uint8_t last_bits;
    Height_Result result;
    uint32_t since_ms;
} SmHeight;

void sm_height_init(SmHeight *sm);

/* raw_bits: bit 0 = cam bien duoi, bit 1 = cam bien tren */
void sm_height_step(SmHeight *sm, uint8_t raw_bits, uint32_t now_ms);

#endif /* __SM_HEIGHT_H__ */

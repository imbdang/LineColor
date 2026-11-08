#ifndef __COLOR_CLASSIFY_H__
#define __COLOR_CLASSIFY_H__

#include <stdint.h>

/* Gia tri enum chinh la ma 2 bit xuat ra PA6 PA7 */
typedef enum
{
    C_NONE = 0, /* 00: khong ro hoac loi */
    C_RED,      /* 01 */
    C_GREEN,    /* 10 */
    C_BLUE,     /* 11 */
} Color_table;

typedef struct
{
    uint16_t c;
    uint16_t r;
    uint16_t g;
    uint16_t b;
    uint8_t is_tall; /* cam bien tren PA5 dang bao, chon he so phat */
} Color_Sample;

/* Tru thanh phan hong ngoai IR = (R + G + B - C) / 2 khoi ca 4 kenh. Mau bao hoa giu nguyen. */
void color_remove_ir(uint16_t *c, uint16_t *r, uint16_t *g, uint16_t *b);

/* Ap he so RAW_WEIGHT len gia tri R, G doc tu cam bien */
uint16_t color_weight_red(uint16_t raw_r);
uint16_t color_weight_green(uint16_t raw_g);

/* sample.r va sample.g phai da qua color_weight_red, color_weight_green */
Color_table color_classify(const Color_Sample *sample);

#endif /* __COLOR_CLASSIFY_H__ */

#include "color_classify.h"
#include "config.h"

typedef struct
{
    Color_table color;
    int16_t r;
    int16_t g;
    int16_t b;
} Color_Ref;

static const Color_Ref s_refs_low[] = CFG_COLOR_REFS_LOW;
static const Color_Ref s_refs_tall[] = CFG_COLOR_REFS_TALL;

static uint16_t color_subtract(uint16_t value, uint16_t amount)
{
    return (value > amount) ? (uint16_t)(value - amount) : 0U;
}

void color_remove_ir(uint16_t *c, uint16_t *r, uint16_t *g, uint16_t *b)
{
    int32_t ir = ((int32_t)*r + (int32_t)*g + (int32_t)*b - (int32_t)*c) / 2;

    if ((ir <= 0) || (*c >= CFG_COLOR_SATURATION) || (*r >= CFG_COLOR_SATURATION) ||
        (*g >= CFG_COLOR_SATURATION) || (*b >= CFG_COLOR_SATURATION))
    {
        return;
    }

    *c = color_subtract(*c, (uint16_t)ir);
    *r = color_subtract(*r, (uint16_t)ir);
    *g = color_subtract(*g, (uint16_t)ir);
    *b = color_subtract(*b, (uint16_t)ir);
}

uint16_t color_weight_red(uint16_t raw_r)
{
    return (uint16_t)(((uint32_t)raw_r * CFG_COLOR_R_RAW_WEIGHT) / 100U);
}

uint16_t color_weight_green(uint16_t raw_g)
{
    return (uint16_t)(((uint32_t)raw_g * CFG_COLOR_G_RAW_WEIGHT) / 100U);
}

Color_table color_classify(const Color_Sample *sample)
{
    int32_t r;
    int32_t g;
    int32_t b;
    uint32_t best_distance = UINT32_MAX;
    Color_table best_color = C_NONE;
    uint32_t green_penalty = (sample->is_tall != 0U) ? CFG_COLOR_GREEN_PENALTY_TALL : CFG_COLOR_GREEN_PENALTY;
    const Color_Ref *refs = (sample->is_tall != 0U) ? s_refs_tall : s_refs_low;
    uint8_t count = (uint8_t)((sample->is_tall != 0U) ? (sizeof(s_refs_tall) / sizeof(s_refs_tall[0]))
                                                      : (sizeof(s_refs_low) / sizeof(s_refs_low[0])));

    if (sample->c < CFG_COLOR_MIN_CLEAR)
        return C_NONE; /* too dark, unreliable */

    if ((sample->c >= CFG_COLOR_SATURATION) || (sample->r >= CFG_COLOR_SATURATION) ||
        (sample->g >= CFG_COLOR_SATURATION) || (sample->b >= CFG_COLOR_SATURATION))
        return C_NONE; /* ADC bao hoa, ti le khong con dung */

    r = ((int32_t)sample->r * 1000) / sample->c;
    g = ((int32_t)sample->g * 1000) / sample->c;
    b = ((int32_t)sample->b * 1000) / sample->c;

    for (uint8_t i = 0U; i < count; i++)
    {
        int32_t dr = r - refs[i].r;
        int32_t dg = g - refs[i].g;
        int32_t db = b - refs[i].b;
        uint32_t distance = (uint32_t)((dr * dr) + (dg * dg) + (db * db));

        if (refs[i].color == C_GREEN)
        {
            /* distance dang la binh phuong nen he so cung phai binh phuong */
            distance = (distance * ((green_penalty * green_penalty) / 100U)) / 100U;
        }

        if (distance < best_distance)
        {
            best_distance = distance;
            best_color = refs[i].color;
        }
    }

    if (best_distance > (CFG_COLOR_MAX_DISTANCE * CFG_COLOR_MAX_DISTANCE))
        return C_NONE;

    return best_color;
}

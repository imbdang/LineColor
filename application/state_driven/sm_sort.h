#ifndef __SM_SORT_H__
#define __SM_SORT_H__

#include "color_classify.h"
#include "sm_servo.h"

typedef enum
{
    SM_SORT_IDLE = 0, /* khong co vat: LED tat, servo khong dong */
    SM_SORT_MEASURING /* co vat: LED bat, do mau */
} SmSort_State;

typedef struct
{
    SmSort_State state;
    SmServo_Event pending;   /* o dich dang doi on dinh */
    SmServo_Event last_sent; /* o dich da gui lenh gan nhat */
    uint32_t since_ms;       /* luc pending bat dau */
} SmSort;

void sm_sort_init(SmSort *sm);

/*
 * present: cam bien duoi bao co vat. tall: cam bien tren bao vat cao.
 * color: mau da xac nhan, C_NONE neu chua co.
 * Tra ve lenh servo khi mau va chieu cao giu nguyen CFG_SORT_DECISION_MS va khac o da gui,
 * nen cac vat di lien nhau van duoc phan loai ma khong can cam bien duoi nha ra.
 * Khi het vat servo giu nguyen vi tri.
 */
SmServo_Event sm_sort_update(SmSort *sm, uint8_t present, uint8_t tall, Color_table color, uint32_t now_ms);

/* Tra ve 1 khi can bat LED va do mau */
uint8_t sm_sort_is_measuring(const SmSort *sm);

#endif /* __SM_SORT_H__ */

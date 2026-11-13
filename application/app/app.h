#ifndef __APP_H__
#define __APP_H__

#include "color_classify.h"
#include "sm_height.h"
#include "sm_servo.h"
#include <stdint.h>

void app_run(void);

/* ============ a_servo.c ============ */
void a_servo_init(void);
void a_servo_post(SmServo_Event event);

/* ============ a_motor.c ============ */
/* Motor bang chuyen chay mot chieu, toc do chinh bang 2 nut BTN_CONV_UP, BTN_CONV_DOWN */
void a_motor_init(void);

/* ============ a_button.c ============ */
void a_button_init(void);

/* ============ a_height.c ============ */
typedef struct
{
    uint8_t raw_bits; /* bit 0 = PA4 duoi, bit 1 = PA5 tren */
    uint8_t stable_bits;
    Height_Result result;
} Height_Debug;

extern volatile Height_Debug height_debug;

void a_height_init(void);
Height_Result a_height_get_result(void);
uint8_t a_height_object_present(void); /* cam bien duoi PA4 */
uint8_t a_height_object_tall(void);    /* cam bien tren PA5 */

/* ============ a_color.c ============ */
/* Xem color_debug trong debugger de lay mau RGBC that khi chinh nguong. */
typedef struct
{
    uint16_t c;
    uint16_t r;
    uint16_t g;
    uint16_t b;
    Color_table result;
    uint8_t state;
    uint8_t sensor_id;
    uint32_t sample_count;
    uint32_t error_count;
} Color_Debug;

extern volatile Color_Debug color_debug;

void a_color_init(void);

#endif /* __APP_H__ */

#ifndef __HARDWARE_H__
#define __HARDWARE_H__

// ===================== NUT NHAN (thuong ho, noi GND, pull-up trong) =====================
#define BTN_ZERO_PORT       GPIOA
#define BTN_ZERO_PIN        GPIO_PIN_1   // ve 0 do

#define BTN_INC_PORT        GPIOA
#define BTN_INC_PIN         GPIO_PIN_2   // +36 do, tu 180 quay ve 0

#define BTN_MAX_PORT        GPIOA
#define BTN_MAX_PIN         GPIO_PIN_3   // len 180 do

#define BTN_DEC_PORT        GPIOB
#define BTN_DEC_PIN         GPIO_PIN_1   // -36 do, dung o 0

// ===================== SERVO MG996R (nguon 5-6V rieng, chung GND) =====================
#define SERVO_PWM_PORT      GPIOB
#define SERVO_PWM_PIN       GPIO_PIN_9
#define SERVO_PWM_TIMER     TIM4
#define SERVO_PWM_CHANNEL   TIM_CH4

// ===================== CHIEU CAO (active low, pull-up trong, tin hieu toi da 3.3V) =====================
#define HEIGHT_ACTIVE_LEVEL 0U           // muc tin hieu khi co vat
#define HEIGHT_PORT         GPIOA
#define HEIGHT_BOTTOM_PIN   GPIO_PIN_4   // cam bien duoi: co vat
#define HEIGHT_TOP_PIN      GPIO_PIN_5   // cam bien tren: vat cao

// ===================== TCS34725 (logic 3.3V, I2C can keo len 3.3V) =====================
#define TCS_I2C             I2C1

#define I2C1_SCL_PORT       GPIOB
#define I2C1_SCL_PIN        GPIO_PIN_6

#define I2C1_SDA_PORT       GPIOB
#define I2C1_SDA_PIN        GPIO_PIN_7

#define TCS_INT_PORT        GPIOB
#define TCS_INT_PIN         GPIO_PIN_5   // active low, open drain, keo len 3.3V

#define TCS_LED_PORT        GPIOB
#define TCS_LED_PIN         GPIO_PIN_0   // muc cao = bat LED
#define TCS_LED_ON_LEVEL    1U

// ===================== MA MAU RA (khong noi thang vao ngo vao PLC 24V) =====================
// 00 = khong ro hoac loi, 01 = do, 10 = xanh la, 11 = xanh duong
#define COLOR_OUT_PORT      GPIOA
#define COLOR_OUT_MSB_PIN   GPIO_PIN_6
#define COLOR_OUT_LSB_PIN   GPIO_PIN_7

#endif /* __HARDWARE_H__ */

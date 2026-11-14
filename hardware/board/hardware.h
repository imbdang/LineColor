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

// ===================== NUT TOC DO BANG CHUYEN (thuong ho, noi GND, pull-up trong) =====================
#define BTN_CONV_ACTIVE_LEVEL 0U         // muc tin hieu khi nhan

#define BTN_CONV_UP_PORT    GPIOB
#define BTN_CONV_UP_PIN     GPIO_PIN_14  // tang toc do 1 buoc

#define BTN_CONV_DOWN_PORT  GPIOB
#define BTN_CONV_DOWN_PIN   GPIO_PIN_15  // giam toc do 1 buoc

// ===================== SERVO MG996R (nguon 5-6V rieng, chung GND) =====================
#define SERVO_PWM_PORT      GPIOB
#define SERVO_PWM_PIN       GPIO_PIN_9
#define SERVO_PWM_TIMER     TIM4
#define SERVO_PWM_CHANNEL   TIM_CH4

// ===================== DONG CO BANG CHUYEN, L298 kenh A (nguon motor rieng, chung GND) =====================
// Ngo vao L298 nhan muc cao tu 2.3V nen noi thang 3.3V duoc.
// Nen gan tro 10k keo ENA xuong GND de motor khong chay luc reset hoac nap, khi chan MCU con la input.
#define MOTOR_ENA_PORT      GPIOA
#define MOTOR_ENA_PIN       GPIO_PIN_0   // PWM toc do, phai thao jumper ENA tren module
#define MOTOR_ENA_TIMER     TIM2         // TIM4 dang chay 50 Hz cho servo nen dung timer rieng
#define MOTOR_ENA_CHANNEL   TIM_CH1

// ENA cao: IN1 IN2 = 10 chay toi, 01 chay lui, 00 hoac 11 phanh. ENA thap: motor tu troi.
#define MOTOR_IN1_PORT      GPIOB
#define MOTOR_IN1_PIN       GPIO_PIN_12

#define MOTOR_IN2_PORT      GPIOB
#define MOTOR_IN2_PIN       GPIO_PIN_13

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

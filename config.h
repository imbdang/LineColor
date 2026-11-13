#ifndef __CONFIG_H__
#define __CONFIG_H__

/* ============================== Clock ====================================== */
#define CFG_CORE_CLK_HZ (72000000U)
#define CFG_PCLK1_MHZ (36U)
#define CFG_TIM4_CLK_HZ (72000000U) /* APB1 chia 2 nen timer clock = PCLK1 x 2 */
#define CFG_TIM2_CLK_HZ (72000000U) /* cung APB1 voi TIM4 */

/* ============================== I2C ======================================== */
#define CFG_I2C_SPEED_KHZ (100U)

/* ============================== TCS34 ====================================== */
#define CFG_TCS34_ATIME 0xFC /* 9.6 ms, moi chu ky do khoang 12 ms */

#define CFG_TCS34_GAIN TCS34_GAIN_60X

// ATIME   Thoi gian tich phan
// 0xFF    2.4 ms
// 0xFE    4.8 ms
// 0xFD    7.2 ms
// 0xFC    9.6 ms
// 0xFB    12.0 ms
// 0xFA    14.4 ms
// 0xF9    16.8 ms
// 0xF8    19.2 ms
// 0xF7    21.6 ms
// 0xF6    24.0 ms
// 0xF5    26.4 ms
// 0xF4    28.8 ms
// 0xF0    38.4 ms
// 0xEB    50.4 ms
// 0xE0    76.8 ms
// 0xD5    103.2 ms
// 0xC0    153.6 ms
// 0xB0    192.0 ms
// 0x80    307.2 ms
// 0x40    460.8 ms
// 0x00    614.4 ms

/* ============================== Nguong mau ================================= */
#define CFG_COLOR_MIN_CLEAR (15U) /* toi hon muc nay la khong tin cay */

/* Giam kenh R, G tho truoc khi xu ly va log, 80 = con 80%. Doi so nay thi phai doi R, G trong cum mau. */
#define CFG_COLOR_R_RAW_WEIGHT (80U)
#define CFG_COLOR_G_RAW_WEIGHT (80U)

/* Cum mau do that (gain 60x, ATIME 0xFC), don vi phan nghin so voi c: mau, r, g, b.
 * Cung thang voi r%, g%, b% trong log: da tru hong ngoai, R va G da nhan he so RAW_WEIGHT.
 * Mau duoc xep vao cum gan nhat trong bo tuong ung: vat thap (PA5 nha) hoac vat cao (PA5 bao).
 * Khong co cum C_NONE vi co vat hay khong da lay tu cam bien vat can. */
#define CFG_COLOR_REFS_LOW                             \
    {                                                  \
        {C_RED, 472, 142, 231},                        \
        {C_GREEN, 339, 221, 300},                      \
        {C_BLUE, 294, 218, 358},                       \
    }

#define CFG_COLOR_REFS_TALL                            \
    {                                                  \
        {C_RED, 549, 137, 143},                        \
        {C_GREEN, 290, 319, 239},                      \
        {C_BLUE, 184, 275, 426},                       \
    }

/* Xa hon muc nay so voi moi cum thi coi la khong ro mau (350 = 35 diem phan tram).
 * Dat rong de vat o gan khong bi loai: mau dam hon nen cach cum 2 cm kha xa. */
#define CFG_COLOR_MAX_DISTANCE (350U)

/* Phat cum xanh la: khoang cach toi cum nay duoc nhan them he so, 150 = 1.5 lan, 100 = khong phat.
 * Tang len neu xanh duong hay bi nhan thanh xanh la. */
#define CFG_COLOR_GREEN_PENALTY (150U)
#define CFG_COLOR_GREEN_PENALTY_TALL (100U) /* vat cao, PA5 bao: khong phat */

/* 97% cua ADC max = (256 - ATIME) * 1024, tu doi theo ATIME (dung khi ATIME >= 0xC1) */
#define CFG_COLOR_SATURATION (((256U - CFG_TCS34_ATIME) * 1024U * 97U) / 100U)

/* ============================== FSM mau ==================================== */
#define CFG_COLOR_CONFIRM_SAMPLES (2U)
#define CFG_COLOR_LED_SETTLE_SAMPLES (2U) /* bo cac mau dau sau khoi tao, luc LED chua on dinh */
#define CFG_COLOR_WAIT_MS (100U)      /* thoi gian cho ngat toi da moi vong */
#define CFG_COLOR_STALE_MS (500U)     /* khong co mau moi lau hon muc nay la loi */
#define CFG_COLOR_RETRY_MS (1000U)    /* thoi gian cho truoc khi khoi tao lai sau loi */

/* ============================== Nut nhan =================================== */
#define CFG_BUTTON_PERIOD_MS (5U)
#define CFG_BUTTON_DEBOUNCE_MS (20U)

/* ============================== Chieu cao ================================== */
#define CFG_HEIGHT_PERIOD_MS (5U)
#define CFG_HEIGHT_FILTER_MS (20U)

/* ============================== Servo MG996R =============================== */
#define CFG_SERVO_PERIOD_US (20000U)
#define CFG_SERVO_QUEUE_LEN (4U)

/* Goc muc tieu:              0     36     72     108    144    180 do
 * 0 va 180 da xac nhan tren servo that, 4 vi tri giua la uoc tinh can do lai. */
#define CFG_SERVO_PULSE_US {500U, 869U, 1256U, 1701U, 2077U, 2446U}

/* ============================== Dong co bang chuyen (L298) ================= */
/* PWM chan ENA. Counter chay 1 MHz nen 1000 us = 1 kHz, chia duoc 1000 muc. */
#define CFG_MOTOR_PWM_PERIOD_US (1000U)

/* Duty luc khoi dong. Moi lan bam nut tang hoac giam 1 buoc, dung o MIN va MAX. */
#define CFG_MOTOR_DUTY_INIT_PCT (50U)
#define CFG_MOTOR_DUTY_MIN_PCT (20U)
#define CFG_MOTOR_DUTY_MAX_PCT (80U)
#define CFG_MOTOR_DUTY_STEP_PCT (10U)

/* ============================== Phan loai ================================== */
/* Mau va chieu cao phai giu nguyen bay nhieu ms moi gui lenh servo, ke ca khi cac vat di lien nhau.
 * Tranh quay theo mau thoang qua o mep vat. Giam so nay neu can phan ung nhanh hon. */
#define CFG_SORT_DECISION_MS (120U)

/* O chua theo mau va chieu cao. Cao = cam bien tren PA5 bao trong luc vat di qua. */
#define CFG_SORT_SERVO_RED_LOW SM_SERVO_EV_GOTO_0    /* 0 do   */
#define CFG_SORT_SERVO_RED_HIGH SM_SERVO_EV_GOTO_1   /* 36 do  */
#define CFG_SORT_SERVO_GREEN_LOW SM_SERVO_EV_GOTO_2  /* 72 do  */
#define CFG_SORT_SERVO_GREEN_HIGH SM_SERVO_EV_GOTO_3 /* 108 do */
#define CFG_SORT_SERVO_BLUE_LOW SM_SERVO_EV_GOTO_4   /* 144 do */
#define CFG_SORT_SERVO_BLUE_HIGH SM_SERVO_EV_GOTO_5  /* 180 do */

/* ============================== Test LED =================================== */
/* Dat 1 de nhay LED cam bien, luc do task mau khong do. Dat 0 de do mau binh thuong. */
#define CFG_TEST_LED_BLINK (0)
#define CFG_TEST_LED_BLINK_MS (500U)

/* ============================== FreeRTOS =================================== */
/* Stack tinh theo word (4 byte). */
#define CFG_TASK_STACK_SERVO (128U)
#define CFG_TASK_STACK_BUTTON (128U)
#define CFG_TASK_STACK_HEIGHT (128U)
#define CFG_TASK_STACK_COLOR (256U)
#define CFG_TASK_STACK_MOTOR (128U)

#define CFG_TASK_PRIO_SERVO (3U)
#define CFG_TASK_PRIO_BUTTON (2U)
#define CFG_TASK_PRIO_HEIGHT (2U)
#define CFG_TASK_PRIO_COLOR (1U)
#define CFG_TASK_PRIO_MOTOR (2U)

/* Thang 0..7, phai >= 3 (configMAX_SYSCALL_INTERRUPT_PRIORITY) de ISR goi duoc FreeRTOS API */
#define CFG_IRQ_PRIO_COLOR_INT (4U)

#endif /* __CONFIG_H__ */

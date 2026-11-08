#ifndef __M_CLOCK_H__
#define __M_CLOCK_H__

#include "h_rcc.h"

/* SYSCLK = HCLK = PCLK2 = 72 MHz, PCLK1 = 36 MHz, can thach anh 8 MHz */
void M_CLOCK_Set72MHz_HSE_PLL9(void);

#endif /*<! __M_CLOCK_H__ */

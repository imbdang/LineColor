#ifndef __M_TIME_H__
#define __M_TIME_H__

#include <stdint.h>

uint32_t M_TIME_GetMs(void);

/* Chi goi trong task. Thoi gian cho toi thieu la ms. */
void M_TIME_DelayMs(uint32_t ms);

#endif /*<! __M_TIME_H__ */

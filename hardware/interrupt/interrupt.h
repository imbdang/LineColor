#ifndef __H__INTERRUPT_H__
#define __H__INTERRUPT_H__

#include "stdint.h"

typedef void (*Interrupt_Callback)(void);

/* line: 5..9, callback chay trong ISR */
void Interrupt_SetExti9_5Callback(uint8_t line, Interrupt_Callback callback);

#endif /*<! __H__INTERRUPT_H__ */

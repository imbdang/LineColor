#ifndef __M_GPIO_H__
#define __M_GPIO_H__

#include "h_gpio.h"

/* Cac ham Init tu bat clock cua port. */
void M_GPIO_InitInputPullUp(volatile GPIO_TypeDef *gpio, uint8_t pin);
void M_GPIO_InitInputFloating(volatile GPIO_TypeDef *gpio, uint8_t pin);

/* Dat muc output truoc khi chuyen mode, chan khong bi nhay muc luc khoi tao. */
void M_GPIO_InitOutputPP(volatile GPIO_TypeDef *gpio, uint8_t pin, uint8_t initial_level);
void M_GPIO_InitAlternatePP(volatile GPIO_TypeDef *gpio, uint8_t pin);

uint8_t M_GPIO_Read(volatile GPIO_TypeDef *gpio, uint8_t pin);
uint16_t M_GPIO_ReadPort(volatile GPIO_TypeDef *gpio);

void M_GPIO_Write(volatile GPIO_TypeDef *gpio, uint8_t pin, uint8_t level);
void M_GPIO_WriteMask(volatile GPIO_TypeDef *gpio, uint16_t mask, uint16_t value);

#endif /*<! __M_GPIO_H__ */

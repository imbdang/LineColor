/*RM - rm0008, PM0075*/
#ifndef __FLASH_H__
#define __FLASH_H__

#include <stdint.h>

/* ====================================== FLASH REGISTER ======================================= */
typedef struct
{
    volatile uint32_t ACR;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t AR;
    volatile uint32_t RESERVED;
    volatile uint32_t OBR;
    volatile uint32_t WRPR;
} FLASH_TypeDef;

#define FLASH ((volatile FLASH_TypeDef *)0x40022000)

/* SYSCLK: 0-24 MHz = 0 WS, 24-48 MHz = 1 WS, 48-72 MHz = 2 WS */
void FLASH_EnablePrefetch(void);
void FLASH_SetLatency(uint8_t wait_states);

#endif /*<! __FLASH_H__ */

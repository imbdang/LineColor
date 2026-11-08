#include "h_flash.h"

#define FLASH_ACR_LATENCY_MASK (0x7U)
#define FLASH_ACR_PRFTBE (1U << 4)

void FLASH_EnablePrefetch(void)
{
    FLASH->ACR |= FLASH_ACR_PRFTBE;
}

void FLASH_SetLatency(uint8_t wait_states)
{
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY_MASK) | (wait_states & FLASH_ACR_LATENCY_MASK);

    while ((FLASH->ACR & FLASH_ACR_LATENCY_MASK) != (wait_states & FLASH_ACR_LATENCY_MASK))
    {
    }
}

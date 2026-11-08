#include "m_time.h"

#include "FreeRTOS.h"
#include "task.h"

uint32_t M_TIME_GetMs(void)
{
    return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

void M_TIME_DelayMs(uint32_t ms)
{
    /* vTaskDelay(n) tinh ca tick dang chay do, them 1 tick de du toi thieu ms */
    vTaskDelay(pdMS_TO_TICKS(ms) + 1U);
}

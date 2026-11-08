#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* ============================== Scheduler ================================== */
#define configUSE_PREEMPTION 1
#define configUSE_TIME_SLICING 1
#define configCPU_CLOCK_HZ (72000000UL)
#define configTICK_RATE_HZ (1000U)
#define configMAX_PRIORITIES (5)
#define configMINIMAL_STACK_SIZE (128U)
#define configMAX_TASK_NAME_LEN (8)
#define configTICK_TYPE_WIDTH_IN_BITS TICK_TYPE_WIDTH_32_BITS

/* ============================== Memory ===================================== */
#define configSUPPORT_STATIC_ALLOCATION 1
#define configSUPPORT_DYNAMIC_ALLOCATION 0

/* ============================== Features =================================== */
#define configUSE_IDLE_HOOK 0
#define configUSE_TICK_HOOK 0
#define configUSE_MUTEXES 0
#define configUSE_RECURSIVE_MUTEXES 0
#define configUSE_COUNTING_SEMAPHORES 0
#define configUSE_TASK_NOTIFICATIONS 0
#define configUSE_TIMERS 0
#define configUSE_CO_ROUTINES 0
#define configUSE_TRACE_FACILITY 0
#define configQUEUE_REGISTRY_SIZE 0

/* ============================== Debug ====================================== */
#define configCHECK_FOR_STACK_OVERFLOW 2
#define configASSERT(x)           \
    if ((x) == 0)                 \
    {                             \
        taskDISABLE_INTERRUPTS(); \
        for (;;)                  \
        {                         \
        }                         \
    }

/* ============================== Interrupt priority ========================= */
/* Chip clone tren board chi co 3 bit priority (STM32 that co 4), nen chi dung 3 bit cao.
 * Khong dinh nghia configPRIO_BITS de port.c tu do so bit tren phan cung. */
#define configKERNEL_INTERRUPT_PRIORITY 255U
#define configMAX_SYSCALL_INTERRUPT_PRIORITY (3U << 5U)

/* ============================== API duoc dung ============================== */
#define INCLUDE_vTaskDelay 1
#define INCLUDE_xTaskDelayUntil 1
#define INCLUDE_vTaskSuspend 1
#define INCLUDE_vTaskPrioritySet 0
#define INCLUDE_uxTaskPriorityGet 0
#define INCLUDE_vTaskDelete 0

/* ============================== Vector table =============================== */
/* startup.c tro thang vao cac ten nay. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */

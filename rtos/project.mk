RTOS_SRC_DIRS := rtos/FreeRTOS-Kernel \
                 rtos/FreeRTOS-Kernel/portable/GCC/ARM_CM3

RTOS_INC_DIRS := rtos \
                 rtos/FreeRTOS-Kernel/include \
                 rtos/FreeRTOS-Kernel/portable/GCC/ARM_CM3

COMPONENTS_SRCDIRS      += $(RTOS_SRC_DIRS)
COMPONENTS_ADD_INCLUDES += $(RTOS_INC_DIRS)

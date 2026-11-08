HARDWARE_DIRS := hardware/board \
                 hardware/register \
                 hardware/interrupt

COMPONENTS_SRCDIRS      += $(HARDWARE_DIRS)
COMPONENTS_ADD_INCLUDES += $(HARDWARE_DIRS)

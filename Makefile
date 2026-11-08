###################################
#           PROJECT NAME
###################################
PROJECT := main

MCU        := cortex-m3
LD_SCRIPT  := stm32_flash.ld

ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

###################################
#           TOOLCHAINS
###################################
TOOLCHAINS_PATH   ?= /home/imbdang/Toolchain/arm-gcc-14.3
PREFIX_TOOLCHAINS := arm-none-eabi

CC   := $(TOOLCHAINS_PATH)/bin/$(PREFIX_TOOLCHAINS)-gcc
CP   := $(TOOLCHAINS_PATH)/bin/$(PREFIX_TOOLCHAINS)-objcopy
SZ   := $(TOOLCHAINS_PATH)/bin/$(PREFIX_TOOLCHAINS)-size

####################################
#           DIRECTORIES
####################################
BUILD_DIR   := build
OBJ_DIR     := $(BUILD_DIR)/obj

LINKER_DIR  := hardware/linker
STARTUP_C   := hardware/startup/startup.c

COMPONENTS_SRCDIRS      :=
COMPONENTS_ADD_INCLUDES :=

include $(ROOT)application/project.mk
include $(ROOT)middle/project.mk
include $(ROOT)hardware/project.mk
include $(ROOT)rtos/project.mk

####################################
#      SOURCES & OBJECTS LIST
####################################
SRC_C := main.c \
         $(STARTUP_C) \
         $(foreach d,$(COMPONENTS_SRCDIRS),$(wildcard $(d)/*.c))

OBJ_C := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC_C))

OBJS := $(OBJ_C)

DEPS := $(OBJS:.o=.d)

####################################
#           OPT FLAGS
####################################
CFLAGS := -mcpu=$(MCU) \
          -mthumb -Wall -Og -g \
          -ffreestanding \
          -fdata-sections -ffunction-sections \
          -DSTM32F10X_MD \
          -I. \
          $(addprefix -I,$(COMPONENTS_ADD_INCLUDES)) \
          -MMD -MP

LDFLAGS := -T$(LINKER_DIR)/$(LD_SCRIPT) \
           -Wl,--gc-sections \
           -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map \
           -specs=nano.specs \
           -specs=nosys.specs

####################################
#           MAKE RULES
####################################
.PHONY: all clean flash erase bin hex size

all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/$(PROJECT).hex size

bin: $(BUILD_DIR)/$(PROJECT).bin
hex: $(BUILD_DIR)/$(PROJECT).hex
size: $(BUILD_DIR)/$(PROJECT).elf
	$(SZ) $<

$(BUILD_DIR)/$(PROJECT).elf: $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	@echo "Build complete: $@"

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	$(CP) -O binary $< $@
	@echo "Created: $@"

$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	$(CP) -O ihex $< $@
	@echo "Created: $@"

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete"

flash: $(BUILD_DIR)/$(PROJECT).bin
	st-flash write $< 0x8000000
	@echo "Flash complete"

erase:
	st-flash erase
	@echo "Erase complete"

debug:
	gdb-multiarch $(BUILD_DIR)/$(PROJECT).elf \
    -ex "target extended-remote localhost:3333" \
    -ex "monitor reset halt" \
    -ex "load" \
    -ex "break main" \
    -ex "continue"

debug_attach:
	gdb-multiarch $(BUILD_DIR)/$(PROJECT).elf \
    -ex "set remote interrupt-on-connect off" \
    -ex "target extended-remote localhost:3333" \
    -ex "monitor halt"

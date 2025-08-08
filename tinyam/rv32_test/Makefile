PROJECT_NAME ?= tinyam

BUILD_DIR = build

SRCS += $(shell find $(BOARD_DIR)/device -name '*.c')
ASMS += $(shell find $(BOARD_DIR)/boot -name '*.S')
SRCS += $(shell find $(BOARD_DIR)/libgcc -name '*.c')
ASMS += $(shell find $(BOARD_DIR)/libgcc -name '*.S')

TOOLPREFIX ?= riscv64-unknown-elf-

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

CFLAGS += -Wall -Werror -O2 -fno-omit-frame-pointer -ggdb
CFLAGS += -march=rv32e -mabi=ilp32e
CFLAGS += -mcmodel=medany
CFLAGS += -I$(BOARD_DIR)
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax

LDFLAGS ?= -T $(BOARD_DIR)/link.ld
LDFLAGS += -z max-page-size=4096 -Map=$(BUILD_DIR)/$(PROJECT_NAME).map
LDFLAGS += -m elf32lriscv

OBJS += $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))
OBJS += $(patsubst %.S, $(BUILD_DIR)/%.o, $(ASMS))

$(BUILD_DIR)/$(PROJECT_NAME): $(OBJS)
	@echo "[LINK] Linking final $(PROJECT_NAME): $@"
	@$(LD) $(LDFLAGS) -o $@ $(OBJS)
	@$(OBJDUMP) -S $@ > $(BUILD_DIR)/$(PROJECT_NAME).asm
	@$(OBJCOPY) -O binary $@ $(BUILD_DIR)/$(PROJECT_NAME).bin

$(BUILD_DIR)/%.o: %.c $(HEADS)
	@echo "[CC] Compiling $< -> $@"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: %.S $(HEADS)
	@echo "[AS] Compiling $< -> $@"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "[CLEAN] Cleaning up build artifacts"
	@rm -rf $(BUILD_DIR)

.PHONY: clean run crun


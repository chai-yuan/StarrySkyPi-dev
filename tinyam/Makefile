PROJECT_NAME ?= tinyam

BUILD_DIR = build

SRCS += $(shell find $(AM_HOME) -name '*.c')
ASMS += $(shell find $(AM_HOME) -name '*.S')

TOOLPREFIX ?= riscv64-linux-gnu-

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

CFLAGS += -Wall -Werror -O -fno-omit-frame-pointer -ggdb
# CFLAGS += -march=rv32imazicsr -mabi=ilp32
CFLAGS += -march=rv64im_zicsr -mabi=lp64
CFLAGS += -mcmodel=medany
CFLAGS += -I$(AM_HOME)
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

LDFLAGS ?= -T $(AM_HOME)/link.ld
LDFLAGS += -z max-page-size=4096 -Map=$(BUILD_DIR)/$(PROJECT_NAME).map
# LDFLAGS += -m elf32lriscv
LDFLAGS += -m elf64lriscv

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
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S $(HEADS)
	@echo "[AS] Compiling $< -> $@"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "[CLEAN] Cleaning up build artifacts"
	@rm -rf $(BUILD_DIR)

.PHONY: clean run crun


PROJECT_NAME = bootloader
SRCS = main.c
LDFLAGS = -T bootloader.ld

PY_LOAD ?= file_ready_to_load.bin

OBJS += $(BUILD_DIR)/py_load.o

include $(AM_HOME)/Makefile

$(BUILD_DIR)/py_load.o : $(PY_LOAD)
	@mkdir -p $(dir $@)
	cp $(PY_LOAD) app.bin
	$(OBJCOPY) -I binary -O elf64-littleriscv -B riscv app.bin $@

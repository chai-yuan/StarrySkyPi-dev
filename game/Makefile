PROJECT_NAME = game
SRCS = main.c platform.c spi_utils.c
SRCS += $(shell find MicroCoreEngine/engine -name '*.c')
SRCS += $(shell find games -name '*.c')

CFLAGS += -I MicroCoreEngine/engine

include $(AM_HOME)/Makefile

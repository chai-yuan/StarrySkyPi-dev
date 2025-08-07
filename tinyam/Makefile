BOARD_NAME ?= StarrySkyPi2
BOARD_DIR = $(AM_HOME)/$(BOARD_NAME)

SRCS += $(shell find $(AM_HOME)/tinylibc -name '*.c')
ASMS += $(shell find $(AM_HOME)/tinylibc -name '*.S')

include $(AM_HOME)/$(BOARD_NAME)/Makefile

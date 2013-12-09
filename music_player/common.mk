PLATFORM = RASPBERRY_PI

SHELL := /bin/bash
CWD = $(shell pwd)

ARMGNU ?= arm-none-eabi

AS = $(ARMGNU)-as
CC = $(ARMGNU)-gcc
LD = $(ARMGNU)-ld
AR = $(ARMGNU)-ar
OBJDUMP = $(ARMGNU)-objdump
OBJCOPY = $(ARMGNU)-objcopy

HW_DIR=$(ROOT_DIR)/hw
MUSIC_DIR=$(ROOT_DIR)/music
SCHED_DIR=$(ROOT_DIR)/scheduler
START_DIR=$(ROOT_DIR)/start

CFLAGS += -Wall -O0 -std=c99 -nostdlib -nostartfiles -ffreestanding -mcpu=arm1176jzf-s -mtune=arm1176jzf-s -mhard-float -mfloat-abi=hard -mfpu=vfp -ffast-math -g -I. -I$(ROOT_DIR) -I$(HW_DIR) -I$(MUSIC_DIR) -I$(SCHED_DIR) -I$(START_DIR)
#-D$(PLATFORM) -fomit-frame-pointer -mno-apcs-frame -march=armv6z -marm

ASFLAGS += --warn --fatal-warnings -mcpu=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -march=armv6 -g

LD_FLAGS += -L$(GNUARM_ROOT)/lib -L$(GNUARM_ROOT)/arm-none-eabi/lib -L$(GNUARM_ROOT)/lib/gcc/arm-none-eabi/4.7.1 -lc -lm -lgcc 

OBJS = $(patsubst %.s,%.o, $(AS_FILES))
OBJS += $(patsubst %.c,%.o, $(C_FILES))

TARGET: all

.PHONY: common_clean

common_clean :
	rm -f *~
	rm -f *.o
	rm -f *.a
	rm -f *.bin
	rm -f *.hex
	rm -f *.elf
	rm -f *.list
	rm -f *.img
	rm -f *.bc
	rm -f *.clang.opt.s

%.o: %.c
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@

%.o: %.s
	$(ARMGNU)-as $(ASFLAGS) $< -o $@

%.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

%.so : %.c
	$(CC) $(CFLAGS) $(PKGFLAGS)-c $^ -o $@

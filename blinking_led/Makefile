ARMGNU ?= arm-none-eabi

CFLAGS = -Wall -Wextra -Wfloat-equal -Wwrite-strings \
		 -Wpointer-arith -Wconversion -Wredundant-decls -Winit-self \
		 -Wswitch-default -Wswitch-enum -Wundef -Winline -W \
		 -Wunused-parameter -ansi -Wshadow -std=gnu99 -nostdlib \
		 -fomit-frame-pointer -mno-apcs-frame -nostartfiles \
		 -ffreestanding -g -march=armv6z -marm -mthumb-interwork
ASFLAGS = -g -march=armv6z

C_FILES = kernel.c \
		  ../os/dispatcher.c \
		  ../os/hw.c \
		  ../os/malloc.c \
		  ../os/sched.c \
		  ../os/sem.c \
		  ../os/mutex.c
AS_FILES = ../os/vectors.s
MEMMAP = ../os/memmap

OBJS = $(patsubst %.s,%.o,$(AS_FILES))
OBJS += $(patsubst %.c,%.o,$(C_FILES))

.PHONY: gcc clean

gcc: kernel

clean:
	@rm -f *.o
	@rm -f *.bin
	@rm -f *.hex
	@rm -f *.elf
	@rm -f *.list
	@rm -f *.img
	@rm -f *.bc
	@rm -f *.clang.opt.s

%.o: %.c
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@

%.o: %.s
	$(ARMGNU)-as $(ASFLAGS) $< -o $@

kernel: $(MEMMAP) $(OBJS)
	$(ARMGNU)-ld $(OBJS) -T $(MEMMAP) -o kernel.elf
	$(ARMGNU)-objdump -D kernel.elf > kernel.list
	$(ARMGNU)-objcopy kernel.elf -O binary kernel.img
	$(ARMGNU)-objcopy kernel.elf -O ihex kernel.hex

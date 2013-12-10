ARMGNU ?= arm-none-eabi

CFLAGS = -Wall -Wextra -Wfloat-equal -Wwrite-strings \
		 -Wpointer-arith -Wconversion -Wredundant-decls -Winit-self \
		 -Wswitch-default -Wswitch-enum -Wundef -Winline -W \
		 -Wunused-parameter -ansi -Wshadow -std=gnu99 -nostdlib \
		 -fomit-frame-pointer -mno-apcs-frame -nostartfiles \
		 -ffreestanding -g -march=armv6z -marm -mthumb-interwork
ASFLAGS = -g -march=armv6z

C_FILES = kernel.c \
		  ../os/malloc.c \
		  ../os/hw.c \
		  ../os/sched.c \
		  ../os/sem.c \
		  ../os/mutex.c \
		  ../os/pipe.c \
		  ../os/morse.c \
		  ../os/audio.c \
		  ../os/reboot.c \
		  ../os/uart.c
AS_FILES = ../os/vectors.s
MEMMAP = ../os/memmap

OBJS = $(patsubst %.s,%.o,$(AS_FILES))
OBJS += $(patsubst %.c,%.o,$(C_FILES))

.PHONY: gcc clean

gcc: kernel

clean:
	rm -f $(OBJS)
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
music : 
	cd music && make

kernel: $(MEMMAP) $(OBJS) music
	$(ARMGNU)-ld $(OBJS) -T $(MEMMAP) -o kernel.elf
	$(ARMGNU)-objdump -D kernel.elf > kernel.list
	$(ARMGNU)-objcopy kernel.elf -O binary kernel.img
	$(ARMGNU)-objcopy kernel.elf -O ihex kernel.hex

ARMGNU ?= arm-none-eabi

CFLAGS = -Wall -Wextra -Wfloat-equal -Wwrite-strings \
		 -Wpointer-arith -Wcast-align -Wconversion -Wredundant-decls \
		 -Winit-self -Wswitch-default -Wswitch-enum -Wundef -Winline -W \
		 -Wunused-parameter -ansi -Wshadow -std=gnu99 -nostdlib \
		 -fomit-frame-pointer -mno-apcs-frame -nostartfiles \
		 -ffreestanding -g -march=armv6z -marm -mthumb-interwork
ASFLAGS = -g -march=armv6z

C_FILES = kernel.c malloc.c hw.c dispatcher.c sched.c sem.c mutex.c pipe.c
AS_FILES = vectors.s

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

kernel: memmap $(OBJS)
	$(ARMGNU)-ld $(OBJS) -T memmap -o kernel.elf
	$(ARMGNU)-objdump -D kernel.elf > kernel.list
	$(ARMGNU)-objcopy kernel.elf -O binary kernel.img
	$(ARMGNU)-objcopy kernel.elf -O ihex kernel.hex

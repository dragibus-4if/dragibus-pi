ARMGNU ?= arm-none-eabi

CFLAGS = -Wall -Wextra -Wfloat-equal -Wwrite-strings \
		 -Wpointer-arith -Wcast-align -Wconversion -Wredundant-decls \
		 -Winit-self -Wswitch-default -Wswitch-enum -Wundef -Winline -W \
		 -Wunused-parameter -ansi -Wshadow -std=gnu99 -nostdlib \
		 -fomit-frame-pointer -mno-apcs-frame -nostartfiles \
		 -ffreestanding -g -march=armv6z -marm -mthumb-interwork
ASFLAGS = -g -march=armv6z

C_FILES = kernel.c ../../src/dispatcher.c ../../src/hw.c ../../src/malloc.c ../../src/sched.c ../../src/sem.c ../../src/mutex.c ../../src/pipe.c
AS_FILES = ../../src/vectors.s

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

kernel: ../../src/memmap $(OBJS)
	$(ARMGNU)-ld $(OBJS) -T ../../src/memmap -o kernel.elf
	$(ARMGNU)-objdump -D kernel.elf > kernel.list
	$(ARMGNU)-objcopy kernel.elf -O binary kernel.img
	$(ARMGNU)-objcopy kernel.elf -O ihex kernel.hex

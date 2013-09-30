ARMGNU ?= arm-none-eabi

OBJDUMP = $(ARMGNU)-objdump
LD = $(ARMGNU)-ld
OBJCOPY = $(ARMGNU)-objcopy
CC = $(ARMGNU)-gcc
AS = $(ARMGNU)-as

CFLAGS = -Wall -nostdlib -fomit-frame-pointer -mno-apcs-frame -nostartfiles -ffreestanding -g -march=armv6z -marm
ASFLAGS = -g -march=armv6z

C_FILES=notmain.c
AS_FILES=vectors.s

OBJS = $(patsubst %.s,%.o, $(AS_FILES))
OBJS += $(patsubst %.c,%.o, $(C_FILES))

.PHONY: gcc clean

gcc : kernel.hex kernel.bin kernel.img

clean :
	rm -f *.o
	rm -f *.bin
	rm -f *.hex
	rm -f *.elf
	rm -f *.list
	rm -f *.img
	rm -f *.bc
	rm -f *.clang.opt.s

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

kernel.elf : memmap $(OBJS)
	$(LD) $(OBJS) -T memmap -o kernel.elf
	$(OBJDUMP) -D kernel.elf > kernel.list

kernel.bin : kernel.elf
	$(OBJCOPY) kernel.elf -O binary kernel.bin

kernel.hex : kernel.elf
	$(OBJCOPY) kernel.elf -O ihex kernel.hex

kernel.img : kernel.elf
	$(OBJCOPY) kernel.elf -O binary kernel.img
	cp kernel.img SD_Card/

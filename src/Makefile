ARMGNU ?= arm-none-eabi

LD = $(ARMGNU)-ld
CC = $(ARMGNU)-gcc
AS = $(ARMGNU)-as

OBJDUMP = $(ARMGNU)-objdump
OBJCOPY = $(ARMGNU)-objcopy

CFLAGS = -Wall -nostdlib -fomit-frame-pointer -mno-apcs-frame -nostartfiles -ffreestanding -g -march=armv6z -marm
ASFLAGS = -g -march=armv6z

C_FILES = notmain.c hw.c allocateMemory.c process.c
AS_FILES = vectors.s

OBJS = $(patsubst %.s,%.o, $(AS_FILES))
OBJS += $(patsubst %.c,%.o, $(C_FILES))

RM = @rm -f
CP = @cp
MKDIR = @mkdir -p

.PHONY: all clean

all: kernel.hex kernel.bin kernel.img

clean :
	$(RM) -r SD_Card
	$(RM) *.o *.bin *.hex *.elf *.list *.img *.bc *.clang.opt.s

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
	$(MKDIR) SD_Card
	$(CP) kernel.img SD_Card

# vim: ft=make noet sw=4 ts=4 nolist

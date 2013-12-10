ROOT_DIR=.
include common.mk

.PHONY: start hw scheduler music

all : kernel

clean : common_clean
	cd hw && make clean
	cd music && make clean
	cd scheduler && make clean
	cd start && make clean

hw:
	cd hw && make

start:
	cd start && make

scheduler:
	cd scheduler && make

music:
	cd music && make

kernel.elf : hw start music scheduler start/memmap notmain.o
	$(LD) start/vectors.o notmain.o music/music.a hw/hw.a scheduler/sched.a -T start/memmap -o kernel.elf $(LD_FLAGS)

kernel : kernel.elf
	$(OBJCOPY) kernel.elf -O binary kernel.img
	cp kernel.img SD_Card/
	$(OBJDUMP) -D kernel.elf > kernel.list

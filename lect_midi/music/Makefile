MUSIC_ROOT=.
include $(MUSIC_ROOT)/common.mk

PKGFLAGS += `pkg-config --cflags --libs lv2`

.PHONY: plugins posix platform

all : music.a

PLUGINS_DIRS = plugins/*.lv2
PLUGINS_OBJS = $(wildcard $(PLUGINS_DIRS)/*.o)

PLATFORM_SRC = $(wildcard includes/pi/*.c)
PLATFORM_OBJS = $(PLATFORM_SRC:.c=.o)

POSIX_SRC = $(wildcard includes/posix/*.c)
POSIX_OBJS = $(POSIX_SRC:.c=.o)

clean : common_clean
	rm -f includes/*.o includes/*/*.o
	make -C plugins clean

tune.o : tune.mid
	$(LD) -s -r -o $@ -b binary $^

posix: $(POSIX_OBJS)

platform: $(PLATFORM_OBJS)

music.a : music.o posix platform plugins includes/lv2.o includes/mf_read.o tune.o
	arm-none-eabi-ar rcs music.a music.o includes/mf_read.o includes/lv2.o tune.o $(PLATFORM_OBJS) $(POSIX_OBJS) $(PLUGINS_OBJS)

plugins :
	cd plugins && make

MUSIC_ROOT=$(PLUGINS_DIR)/..
include $(MUSIC_ROOT)/common.mk

PKGFLAGS += `pkg-config --cflags --libs lv2`

all :  $(LV2NAME).o

clean: common_clean

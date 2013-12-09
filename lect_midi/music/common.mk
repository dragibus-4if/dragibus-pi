ROOT_DIR=$(MUSIC_ROOT)/..

CFLAGS += -I$(MUSIC_ROOT)/includes -I$(MUSIC_ROOT)/includes/posix -D$(PLATFORM)

include $(ROOT_DIR)/common.mk


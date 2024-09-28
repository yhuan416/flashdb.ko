ifeq ($(target_os), stm32mp13x)

ARCH := arm
CROSS_COMPILE := arm-none-linux-gnueabihf-
KERNEL := /home/yhuan/stm32mp135/linux/linux-5.15.24
EXTRA_CFLAGS += -D__stm32mp13x__

else

ARCH := arm
CROSS_COMPILE ?=
KERNEL ?= /lib/modules/$(shell uname -r)/build

endif

# 模块名
MODULE_NAME = flashdb

# 源文件
MODULE_OBJECTS += main.o common.o kvdb.o

# 编译选项
EXTRA_CFLAGS += -DFDB_LKM -I$(PWD) -Wall -Werror
EXTRA_CFLAGS += -DDEBUG

FLASHDB_PATH := $(PWD)/FlashDB

#enable logging by default
CONFIG_DEBUG_LOGS := y

# porting
EXTRA_CFLAGS += -I$(PWD)/porting/inc
MODULE_OBJECTS += porting/fal_flash_virt_parts.o
MODULE_OBJECTS += porting/fal_flash_kvdb_mem.o
MODULE_OBJECTS += porting/fal_flash_kvdb_norflash.o

# Fal
EXTRA_CFLAGS += -I$(FLASHDB_PATH)/port/fal/inc
MODULE_OBJECTS += FlashDB/port/fal/src/fal.o
MODULE_OBJECTS += FlashDB/port/fal/src/fal_flash.o
MODULE_OBJECTS += FlashDB/port/fal/src/fal_partition.o 

# FlashDB
EXTRA_CFLAGS += -I$(FLASHDB_PATH)/inc
# MODULE_OBJECTS += FlashDB/src/fdb_file.o # not used
MODULE_OBJECTS += FlashDB/src/fdb_kvdb.o
MODULE_OBJECTS += FlashDB/src/fdb_tsdb.o
MODULE_OBJECTS += FlashDB/src/fdb_utils.o
MODULE_OBJECTS += FlashDB/src/fdb.o

obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-y := $(MODULE_OBJECTS)

.PHONY: clean info

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL) M=$(PWD) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL) M=$(PWD) clean

info:
	@echo "KERNEL=$(KERNEL)"
	@echo "MODULE_NAME=$(MODULE_NAME)"
	@echo "SRC=$(MODULE_OBJECTS)"
	@echo "PWD=$(PWD)"

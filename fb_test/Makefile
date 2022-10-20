#!/bin/sh
KERNEL_SOURCE_PATH=/project/workspace1/frazho01/juno/Linaro/android/kernel/linaro/armlt
CC=/project/workspace1/frazho01/tools/gcc-linaro-6.2.1-2016.11-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
CFLAGS += -static -O3 -Wl,-no-as-needed -Wall
INCLUDES=-I$(KERNEL_SOURCE_PATH)/include
LFLAGS=-L
LIBS=-l

all:
	$(CC) $(CFLAGS) $(LFLAGS) $(LIBS) -o fb_test fb_test.c 

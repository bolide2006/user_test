#!/bin/sh
#KERNEL_SOURCE_PATH=/project/workspace1/frazho01/kernel/repo-android12-5.10/out/android12-5.10/kernel_uapi_headers/usr
CC=/project/workspace1/frazho01/tools/gcc-linaro-6.2.1-2016.11-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
CFLAGS += -static -O3 -Wl,-no-as-needed -Wall
#INCLUDES= -I/project/workspace1/frazho01/juno/Linaro/android/kernel/linaro/armlt/arch/arm64/include
INCLUDES+= -I/project/workspace1/frazho01/kernel/repo-android12-5.10/out/android12-5.10/kernel_uapi_headers/usr/include
LFLAGS=-L
LIBS=-l

all:
	$(CC) $(CFLAGS) $(LFLAGS) $(LIBS) $(INCLUDES) -o dma_buf_test dma_buf_test.c

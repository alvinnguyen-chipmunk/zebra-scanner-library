TARGET=zebra-demo
TARGET_LIB=libssi.so
SRC=main.c
SRC_LIB=ssi_utils.c mlsBarcode.c
CFLAGS=-std=gnu99 -Wall
CFLAGS_LIB=-fPIC -shared
CC=$(CROSS_COMPILE)gcc
BIN_PATH=./image/usr/bin
LIB_PATH=$(BIN)/usr/lib

all:
	$(CC) $(CFLAGS) $(SRC) -L. -lssi -I. -o $(BIN_PATH)/$(TARGET)

lib:
	$(CC) $(CFLAGS) $(CFLAGS_LIB) $(SRC_LIB) -o $(LIB_PATH)/$(TARGET_LIB)

clean:
	rm $(BIN_PATH)/$(TARGET) $(LIB_PATH)/$(TARGET_LIB)

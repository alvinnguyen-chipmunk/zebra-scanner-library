TARGET=zebra
TARGET_LIB=libssi.so
SRC=main.c
SRC_LIB=ssi_utils.c mlsBarcode.c
CFLAGS=-std=gnu99 -Wall
CFLAGS_LIB=-fPIC -shared
CC=gcc

all:
	$(CC) $(CFLAGS) $(SRC) -L. -lssi -I. -o $(TARGET)

lib:
	$(CC) $(CFLAGS) $(CFLAGS_LIB) $(SRC_LIB) -o $(TARGET_LIB)

clean:
	rm $(TARGET) $(TARGET_LIB)

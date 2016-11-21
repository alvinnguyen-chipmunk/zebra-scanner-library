lib_LTLIBRARIES = libstylssi.la
ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -std=c99
libstylssi_la_SOURCES =  mlsBarcode.c mlsBarcode.h ssi.h ssi_utils.c ssi_utils.h
include_HEADERS = mlsBarcode.h

# Reference application
bin_PROGRAMS = barcode_demo
barcode_demo_SOURCES = example/barcode_demo.c
barcode_demo_LDADD = libstylssi.la

lib_LTLIBRARIES = libstylssi.la
ACLOCAL_AMFLAGS = -I m4
libstylssi_la_SOURCES = mlsBarcode.c ssi_utils.c
include_HEADERS = mlsBarcode.h

# Reference application
bin_PROGRAMS = barcode_demo
barcode_demo_SOURCES = example/main.c
barcode_demo_LDADD = libstylssi.la

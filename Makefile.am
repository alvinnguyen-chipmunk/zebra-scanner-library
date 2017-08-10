######################### STYL SCANNER COMMON VARIABLE ############################

ACLOCAL_AMFLAGS = -I m4

pkgconfig_DATA = libstylssi.pc
pkgconfigdir = $(libdir)/pkgconfig

if CONSOLE
CONSOLE_CFLAGS = -D__CONSOLE__
else
CONSOLE_CFLAGS = 
endif

if DEBUG
DEBUG_CFLAGS = -g -ggdb
else
DEBUG_CFLAGS = -D__RELEASE__
endif

STYLSCANNER_includedir = $(includedir)
STYLSCANNER_SOURCES = ssi.h mlsBarcode.h mlsBarcode.c
STYLSCANNER_include_HEADERS = mlsBarcode.h
STYLSCANNER_CFLAGS = $(DEBUG_CFLAGS) $(CONSOLE_CFLAGS) -std=c99 
STYLSCANNER_LDFLAGS = 

####################### STYL SCANNER SSI LIBTOOL LIBRARY ##########################
lib_LTLIBRARIES = libstylssi.la
libstylssi_la_SOURCES = ${STYLSCANNER_SOURCES}
libstylssi_la_include_HEADERS = $(STYLSCANNER_include_HEADERS)
libstylssi_la_includedir = $(STYLSCANNER_includedir)
libstylssi_la_CFLAGS = $(STYLSCANNER_CFLAGS)
libstylssi_la_LDFLAGS = $(STYLSCANNER_LDFLAGS)

######################## STYL SCANNER SSI STATIC LIBRARY ##########################
lib_LIBRARIES = libstylssi.a
libstylssi_a_SOURCES = ${STYLSCANNER_SOURCES}
libstylssi_a_include_HEADERS = $(STYLSCANNER_include_HEADERS)
libstylssi_a_includedir = $(STYLSCANNER_includedir)
libstylssi_a_CFLAGS = $(STYLSCANNER_CFLAGS)

######################### STYL SCANNER COMMON VARIABLE ############################
bin_PROGRAMS = barcode_demo
barcode_demo_SOURCES = example/barcode_demo.c
barcode_demo_CFLAGS = $(DEBUG_CFLAGS) -std=c99 -I$(top_srcdir)/src
barcode_demo_LDADD = libstylssi.la
#barcode_demo_LDFLAGS = -lstylssi -L./../src

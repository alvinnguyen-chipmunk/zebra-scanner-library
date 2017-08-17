######################### STYL SCANNER COMMON VARIABLE ############################
if SYSTEMD
SYSTEMD_CFLAGS = -D__SYSTEMD__
else
SYSTEMD_CFLAGS =
endif

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
STYLSCANNER_CFLAGS = $(DEBUG_CFLAGS) $(CONSOLE_CFLAGS) $(SYSTEMD_CFLAGS) -std=c99 
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

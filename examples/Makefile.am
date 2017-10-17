######################### STYL SCANNER COMMON VARIABLE ############################

if DEBUG
DEBUG_CFLAGS = -g -ggdb
else
DEBUG_CFLAGS = -D__RELEASE__
endif

STYLSCANNER_SOURCES = mlsScannerExample_Common.h mlsScannerExample_Common.c
STYLSCANNER_CFLAGS = $(DEBUG_CFLAGS) -std=c99 -D_BSD_SOURCE -D_DEFAULT_SOURCE -I$(top_srcdir)/src $(LIBUDEV_CFLAGS)
STYLSCANNER_LDADD  = -lstylssi -L./../src $(LIBUDEV_LIBS)

######################### STYL SCANNER Example with some feature ############################
bin_PROGRAMS = StylScannerExample
StylScannerExample_SOURCES = $(STYLSCANNER_SOURCES) mlsScannerExample.c
StylScannerExample_CFLAGS = $(STYLSCANNER_CFLAGS)
StylScannerExample_LDADD = $(STYLSCANNER_LDADD)

######################### STYL SCANNER Example with Auto mode  ##############################
bin_PROGRAMS += StylScannerExample_Auto
StylScannerExample_Auto_SOURCES = $(STYLSCANNER_SOURCES) mlsScannerExample_Auto.c
StylScannerExample_Auto_CFLAGS = $(STYLSCANNER_CFLAGS)
StylScannerExample_Auto_LDADD = $(STYLSCANNER_LDADD)

######################### STYL SCANNER Example with Manual mode  ############################
bin_PROGRAMS += StylScannerExample_Manual
StylScannerExample_Manual_SOURCES = $(STYLSCANNER_SOURCES) mlsScannerExample_Manual.c
StylScannerExample_Manual_CFLAGS = $(STYLSCANNER_CFLAGS)
StylScannerExample_Manual_LDADD = $(STYLSCANNER_LDADD)

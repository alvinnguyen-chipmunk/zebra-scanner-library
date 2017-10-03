######################### STYL SCANNER COMMON VARIABLE ############################

if DEBUG
DEBUG_CFLAGS = -g -ggdb
else
DEBUG_CFLAGS = -D__RELEASE__
endif

######################### STYL SCANNER AUTO DETECT MODE ############################
bin_PROGRAMS = StylScannerAuto_Demo
StylScannerAuto_Demo_SOURCES = mlsScannerAuto_Demo.c
StylScannerAuto_Demo_CFLAGS = $(DEBUG_CFLAGS) -std=c99 -D_BSD_SOURCE -D_DEFAULT_SOURCE -I$(top_srcdir)/src $(LIBUDEV_CFLAGS)
StylScannerAuto_Demo_LDADD = -lstylssi -L./../src $(LIBUDEV_LIBS)

########################## STYL SCANNER MANUAL MODE ################################
bin_PROGRAMS += StylScannerManual_Demo
StylScannerManual_Demo_SOURCES = mlsScannerManual_Demo.c
StylScannerManual_Demo_CFLAGS = $(DEBUG_CFLAGS) -std=c99 -D_BSD_SOURCE -D_DEFAULT_SOURCE -I$(top_srcdir)/src $(LIBUDEV_CFLAGS)
StylScannerManual_Demo_LDADD = -lstylssi -L./../src $(LIBUDEV_LIBS)

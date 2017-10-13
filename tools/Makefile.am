######################### STYL SCANNER COMMON VARIABLE ############################

if DEBUG
DEBUG_CFLAGS = -g -ggdb
else
DEBUG_CFLAGS = -D__RELEASE__
endif

######################### STYL SCANNER AUTO DETECT MODE ############################
bin_PROGRAMS = StylScannerSetup
StylScannerSetup_SOURCES = mlsScannerSetup.c
StylScannerSetup_CFLAGS = $(DEBUG_CFLAGS) -std=c99 -D_BSD_SOURCE -D_DEFAULT_SOURCE -I$(top_srcdir)/src $(LIBUDEV_CFLAGS) $(GLIB_CFLAGS)
StylScannerSetup_LDADD = -lstylssi -L./../src $(LIBUDEV_LIBS) $(GLIB_LIBS)

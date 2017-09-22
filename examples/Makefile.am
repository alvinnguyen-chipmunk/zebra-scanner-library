######################### STYL SCANNER COMMON VARIABLE ############################

if DEBUG
DEBUG_CFLAGS = -g -ggdb
else
DEBUG_CFLAGS = -D__RELEASE__
endif

######################### STYL SCANNER EXAMPLE ####################################
bin_PROGRAMS = barcode_demo
barcode_demo_SOURCES = barcode_demo.c
barcode_demo_CFLAGS = $(DEBUG_CFLAGS) -std=c99 -I$(top_srcdir)/src
barcode_demo_LDADD = -lstylssi -L./../src

########################## STYL SCANNER TEST ######################################
bin_PROGRAMS += barcode_test
barcode_test_SOURCES = barcode_test.c
barcode_test_CFLAGS = $(DEBUG_CFLAGS) -std=c99 -I$(top_srcdir)/src $(LIBUDEV_CFLAGS)
barcode_test_LDADD = -lstylssi -L./../src $(LIBUDEV_LIBS)

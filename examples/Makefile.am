######################### STYL SCANNER COMMON VARIABLE ############################

if DEBUG
DEBUG_CFLAGS = -g -ggdb
else
DEBUG_CFLAGS = -D__RELEASE__
endif

######################### STYL SCANNER EXAMPLE ####################################
bin_PROGRAMS = StylScanner_Demo_1
StylScanner_Demo_1_SOURCES = CStylScanner_Demo_1.c
StylScanner_Demo_1_CFLAGS = $(DEBUG_CFLAGS) -std=c99 -D_BSD_SOURCE -D_DEFAULT_SOURCE -I$(top_srcdir)/src $(LIBUDEV_CFLAGS)
StylScanner_Demo_1_LDADD = -lstylssi -L./../src $(LIBUDEV_LIBS)

########################## STYL SCANNER TEST ######################################
bin_PROGRAMS += StylScanner_Demo_2
StylScanner_Demo_2_SOURCES = CStylScanner_Demo_2.c
StylScanner_Demo_2_CFLAGS = $(DEBUG_CFLAGS) -std=c99 -D_BSD_SOURCE -D_DEFAULT_SOURCE -I$(top_srcdir)/src $(LIBUDEV_CFLAGS)
StylScanner_Demo_2_LDADD = -lstylssi -L./../src $(LIBUDEV_LIBS)

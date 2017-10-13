######################### STYL SCANNER COMMON VARIABLE ############################
if DEBUG
DEBUG_CFLAGS = -g -ggdb -D__DEBUG__
endif

if RELEASE
RELEASE_CFLAGS = -D__RELEASE__
endif

#STYLSCANNER_includedir = $(includedir)/styl
STYLSCANNER_includedir = $(includedir)

STYLSCANNER_SOURCES = mlsBarcode.h          \
                      mlsScanner.c          \
                      mlsScannerUtils.h     \
                      mlsScannerUtils.c     \
                      mlsScannerConfig.h    \
                      mlsScannerConfig.c    \
                      mlsScannerDevice.h    \
                      mlsScannerDevice.c    \
                      mlsScannerSSI.h       \
                      mlsScannerSSI.c       \
                      mlsScannerPackage.h   \
                      mlsScannerPackage.c


STYLSCANNER_include_HEADERS = mlsBarcode.h
STYLSCANNER_CFLAGS = $(DEBUG_CFLAGS) $(RELEASE_CFLAGS) $(LIBUDEV_CFLAGS) $(GLIB_CFLAGS) \
					-std=c99 -D_BSD_SOURCE -D_DEFAULT_SOURCE
STYLSCANNER_LDFLAGS = $(LIBUDEV_LIBS) $(GLIB_LIBS) -version-info @STYL_VERSION_INFO@

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

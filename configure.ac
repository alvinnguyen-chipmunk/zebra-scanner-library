AC_INIT([libstylscanner], [2.0.0], [alvin.nguyen@styl.solutions])
AC_CONFIG_SRCDIR([configure.ac])

# Version information
STYL_MAJOR_VERSION=0
STYL_MINOR_VERSION=0
STYL_MICRO_VERSION=0
STYL_MICRO_VERSION_SUFFIX=
STYL_VERSION=$STYL_MAJOR_VERSION.$STYL_MINOR_VERSION.$STYL_MICRO_VERSION$STYL_MICRO_VERSION_SUFFIX
STYL_VERSION_INFO=`expr $STYL_MAJOR_VERSION + $STYL_MINOR_VERSION`:$STYL_MICRO_VERSION:$STYL_MINOR_VERSION
STYL_VERSION_NUMBER=`expr $STYL_MAJOR_VERSION \* 10000 + $STYL_MINOR_VERSION \* 100 + $STYL_MICRO_VERSION`

AC_SUBST(STYL_MAJOR_VERSION)
AC_SUBST(STYL_MINOR_VERSION)
AC_SUBST(STYL_MICRO_VERSION)
AC_SUBST(STYL_VERSION)
AC_SUBST(STYL_VERSION_INFO)
AC_SUBST(STYL_VERSION_NUMBER)
AC_SUBST(STYL_VERSION_EXTRA)

VERSION=${STYL_VERSION}

dnl Must come before AM_INIT_AUTOMAKE.

# Auxiliary files go "build-aux" directory
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([-Wall subdir-objects])
AC_CONFIG_MACRO_DIR([m4])

AC_ENABLE_SHARED
AC_ENABLE_STATIC

AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AR
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S

dnl ================================================================

LT_INIT

# Configure for libtool
AM_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --silent"

dnl ================================================================

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_STRUCT_TM

# LIBUDEV checks
udev_module="libudev"
PKG_CHECK_MODULES(LIBUDEV, [$udev_module])
AC_SUBST(LIBUDEV_CFLAGS)
AC_SUBST(LIBUDEV_LIBS)
LIBUDEV_VERSION=`$PKG_CONFIG --modversion ${udev_module}`

# Glib-2.0 checks
glib_modules="glib-2.0"
PKG_CHECK_MODULES([GLIB], [$glib_modules])
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)
GLIB_VERSION=`$PKG_CONFIG --modversion ${glib_modules}`

dnl ================================================================

AC_ARG_ENABLE([debug],[  --enable-debug            Turn on print debug information to console.],
[case "${enableval}" in
	yes) debug=true ;;
	no)  debug=false ;;
	*) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],[debug=false])
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])


AC_ARG_ENABLE([blocking],[  --enable-blocking     Enable serial blocking for scanner device port.],
[case "${enableval}" in
 	yes) 	blocking=true ;;
	no)  	blocking=false ;;
  	*) 	AC_MSG_ERROR([bad value ${enableval} for --enable-blocking]) ;;
esac],[blocking=false])
AM_CONDITIONAL([BLOCKING], [test x$blocking = xtrue])

dnl ================================================================

AC_CONFIG_FILES([ Makefile src/Makefile examples/Makefile tools/Makefile])
AC_OUTPUT([ stylssi.pc])


dnl ================================================================

echo "----------------------------------------------------------------"
echo "----------------------------------------------------------------"
echo ""
echo "Install STYL SSI LIBRARY in     : ${prefix}"
echo "Using UDEV library version      : ${LIBUDEV_VERSION}"
echo "Using GLIB version              : ${GLIB_VERSION}"
echo ""
echo "Configuration is done OK."
echo ""

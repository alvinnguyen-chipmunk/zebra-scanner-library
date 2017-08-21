AC_INIT([libstylscanner], [1.0], [alvin.nguyen@styl.solutions])
AC_CONFIG_SRCDIR([configure.ac])

dnl Must come before AM_INIT_AUTOMAKE.

# Auxiliary files go "build-aux" directory
AC_CONFIG_AUX_DIR([build-aux])

# Init recursive automake
AM_INIT_AUTOMAKE([-Wall])

AC_CONFIG_MACRO_DIR([m4])

AC_ENABLE_SHARED

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

dnl ================================================================

AC_ARG_ENABLE([debug],
[  --enable-debug    Turn on print debug information to console.],
[case "${enableval}" in
  yes) debug=true ;;
  no)  debug=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],[debug=false])

AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

AC_ARG_ENABLE([console],
[  --enable-console    Let print information and warning to console.],
[case "${enableval}" in
  yes) console=true ;;
  no)  console=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-console]) ;;
esac],[console=false])

AM_CONDITIONAL([CONSOLE], [test x$console = xtrue])

AC_ARG_ENABLE([systemd],
[  --enable-systemd    Run application under systemd.],
[case "${enableval}" in
  yes) systemd=true ;;
  no)  systemd=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-systemd]) ;;
esac],[systemd=false])

AM_CONDITIONAL([SYSTEMD], [test x$systemd = xtrue])

dnl ================================================================

AC_CONFIG_FILES([ Makefile src/Makefile examples/Makefile])
AC_OUTPUT([ stylssi.pc])


dnl ================================================================

echo "----------------------------------------------------------------"
echo "----------------------------------------------------------------"
echo ""
echo "Install STYL SSI LIBRARY in           : ${prefix}"

echo ""
echo "Configuration is done OK."
echo ""

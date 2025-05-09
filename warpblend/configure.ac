#                                            -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.50])
AC_INIT([warpblend], [0.1], [Reto Stockli])
AC_CONFIG_SRCDIR([src/warpblend.c])

# produce makefiles (turn on warnings on automake!)
AM_INIT_AUTOMAKE([-Wall -Werror])

# AM_CONFIG_HEADER does more than AC_CONFIG_HEADERS
AC_CONFIG_HEADERS([config.h])

# Checks for programs
# AC_PROG_CC redefines CFLAGS, so save original CFLAGS and reapply them
OLD_CFLAGS=$CFLAGS
AC_PROG_CC
CFLAGS=$OLD_CFLAGS

# Checks for Operating System
AC_CANONICAL_HOST
echo $host_os
case $host_os in
linux*)
    os=lin
    AC_DEFINE([LIN],[1],[Define if Host Type is Linux])
    ;;
*)
    AC_MSG_ERROR([Unsupported operating system. Currently only Linux is supported for NVIDIA Warping/Blending.])
esac

AM_CONDITIONAL([OS_LINUX], [test "x$os" = xlin])
AM_CONDITIONAL([OS_DARWIN], [test "x$os" = xmac])
AM_CONDITIONAL([OS_WINDOWS], [test "x$os" = xwin])

# Debug compilation support
AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [enable debug data generation (default=no)])],
    [HAVE_DEBUG="$enableval"],
    [HAVE_DEBUG=no])
AC_MSG_RESULT([$HAVE_DEBUG])

if test "x$HAVE_DEBUG" = xyes; then
    AC_DEFINE([DEBUG],[],[Debug Mode])
    #CFLAGS="$CFLAGS -g -Wall -Wno-uninitialized -fsanitize=address -O0"
    CFLAGS="$CFLAGS -g -Wall -Wno-uninitialized -O0"
else
    AC_DEFINE([NDEBUG],[],[No-debug Mode])
    CFLAGS="$CFLAGS -O2"
fi

# do not allow multiple definitions
CFLAGS="$CFLAGS -fno-common"

# Enable Threading support
CFLAGS="$CFLAGS -pthread"

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h stdbool.h assert.h string.h ctype.h unistd.h stdio.h stdbool.h math.h signal.h sys/types.h sys/time.h errno.h])


HAVE_XLIB=yes
AC_CHECK_LIB([X11],[main],,unset HAVE_XLIB)
AC_CHECK_HEADERS([X11/Xlib.h X11/Xutil.h],,unset HAVE_XLIB)

if test -z "$HAVE_XLIB"; then
    AC_MSG_ERROR([Can't find X11 library or headers. Please try installing 'libx11-dev'.])
fi

HAVE_XNVCtrl=yes
AC_CHECK_LIB([XNVCtrl],[main],,unset HAVE_XNVCtrl)
AC_CHECK_HEADERS(NVCtrl/NVCtrl.h NVCtrl/NVCtrlLib.h,,unset HAVE_XNVCtrl,[#include <X11/Xlib.h>])

if test -z "$HAVE_XNVCtrl"; then
    AC_MSG_ERROR([Can't find XNVCtrl library or headers. Please try installing 'libxnvctrl-dev'.])
fi


# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday memmove memset pow socket strchr strtol malloc])
AC_CHECK_LIB([m],[roundf])

# set installation prefix to current directory if none was given
#if test "x$prefix" = xNONE; then
#    prefix=`pwd`;
#fi

AC_MSG_NOTICE([$PACKAGE_NAME will be installed in $prefix])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

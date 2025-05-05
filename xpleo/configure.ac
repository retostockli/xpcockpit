#                                            -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.50])
AC_INIT([xpleo], [0.10], [Reto Stockli])
AC_CONFIG_SRCDIR([src/xpleo.c])

# produce makefiles (turn on warnings on automake!)
AM_INIT_AUTOMAKE([-Wall])

# AM_CONFIG_HEADER does more than AC_CONFIG_HEADERS
AC_CONFIG_HEADERS([config.h])

# Checks for programs
# AC_PROG_CC redefines CFLAGS, so save original CFLAGS and reapply them
OLD_CFLAGS=$CFLAGS
AC_PROG_CC
CFLAGS=$OLD_CFLAGS

# Checks for Operating System
AC_CANONICAL_HOST
case $host_os in
linux*)
    os=lin
    AC_DEFINE([LIN],[1],[Define if Host Type is Linux])
    ;;
darwin*)
    os=mac
    AC_DEFINE([MAC],[1],[Define if Host Type is Apple])
    LIBS="$LIBS -framework IOKit -framework CoreFoundation"
    ;;
mingw*)
    os=win
    AC_DEFINE([WIN],[1],[Define if Host Type is Windows])
    LDFLAGS="$LDFLAGS -static"
    LIBS="$LIBS -lws2_32 -lsetupapi"
    ;;
**)
    AC_MSG_ERROR([Unsupported operating system $host_os. Currently only Linux, OSX and MinGW are supported.])
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
# DEBUGGGING WITH LEAK CHECK AND OUT OF BOUNDS CHECK
#    CFLAGS="$CFLAGS -g -Wall -Wno-uninitialized -fsanitize=address"
# REGULAR DEBUGGING FOR GDB
    CFLAGS="$CFLAGS -g -Wall -Wno-uninitialized"
# DEBUGING WITH GPROF SYMBOLS
#    CFLAGS="$CFLAGS -g -pg -Wall"
else
    AC_DEFINE([NDEBUG],[],[No-debug Mode])
    CFLAGS="$CFLAGS -O2"
fi

# Checks for libraries.
AC_MSG_CHECKING([whether a location for the libusb-1.0 library was specified])
AC_ARG_WITH([libusb],
[AS_HELP_STRING([--with-libusb=<directory>],
                [Specify location the libusb-1.0 library. Configure will expect
to find subdirs include and lib. If none is given, by default hidapi is used])],
                [LIBUSBDIR=$with_libusb])
if test -z "$LIBUSBDIR" ; then
# no libusb path was provided: use HIDAPI to access HID devices directly
    AC_MSG_RESULT([no])
    HAVE_LIBUSB=no
    HAVE_HIDAPI=yes
    AC_DEFINE([HIDAPI],[],[Use HIDAPI to access HID devices directly])
else
# libusb path was provided: use libusb-1.0 library to access HID devices as USB devices
    AC_MSG_RESULT([$LIBUSBDIR])
    LIBS="$LIBS -L$LIBUSBDIR/lib"
    CPPFLAGS="$CPPFLAGS -I$LIBUSBDIR/include"
    HAVE_LIBUSB=yes
    HAVE_HIDAPI=no
    AC_DEFINE([LIBUSB],[],[Use LIBUSB to access HID devices as regular USB devices])

    # check for library presence
    AC_CHECK_LIB(usb-1.0, main, ,HAVE_LIBUSB=no)
    if test "x$HAVE_LIBUSB" = xno; then
	AC_MSG_ERROR([Can't find libusb-1.0 library, or can't link a program to it.])
    fi
fi

# on linux, check for udev, rt library
if test "x$os" = xlin; then
    if test "x$HAVE_HIDAPI" = xyes; then
	AC_CHECK_LIB(udev,main,,AC_MSG_ERROR([Could not find the udev library]))
    fi
    AC_CHECK_LIB(rt,clock_gettime,,AC_MSG_ERROR([Could not find the rt library]))
fi

# do not allow multiple definitions
CFLAGS="$CFLAGS -fno-common"

# Enable Threading support
CFLAGS="$CFLAGS -pthread"

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h stdio.h stdbool.h math.h signal.h sys/types.h sys/time.h errno.h])

if test "x$os" = xwin; then
AC_CHECK_HEADERS([winsock2.h])
else
AC_CHECK_HEADERS([arpa/inet.h sys/ioctl.h sys/socket.h netinet/tcp.h netinet/in.h netinet/ip.h])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday memmove memset pow socket strchr strtol malloc])
AC_CHECK_LIB(m,roundf,,AC_MSG_ERROR([Could not find the math library]))
if test "x$os" = xwin; then
    AC_CHECK_LIB([ws2_32],[socket])
fi

HAVE_LIBXPCOCKPIT=yes
AC_MSG_CHECKING([whether a location for the xpcockpit library was specified])
AC_ARG_WITH([libxpcockpit],
[AS_HELP_STRING([--with-libxpcockpit=<directory>],
                [Specify location the xpcockpit library. Configure will expect
to find headers in <directory>/include and libraries in <directory>/lib.])]
,
                [LIBXPCOCKPITDIR=$with_libxpcockpit])
if test -z "$LIBXPCOCKPITDIR" ; then
    AC_MSG_RESULT([no])
    LIBS="$LIBS -L$ac_pwd/../libxpcockpit/lib"
    CPPFLAGS="$CPPFLAGS -I$ac_pwd/../libxpcockpit/include"
else
    AC_MSG_RESULT([$LIBXPCOCKPITDIR])
    LIBS="$LIBS -L$LIBXPCOCKPITDIR/lib"
    CPPFLAGS="$CPPFLAGS -I$LIBXPCOCKPITDIR/include"
fi

AC_CHECK_HEADER([xplanebeacon.h],,unset HAVE_LIBXPCOCKPIT)
AC_CHECK_LIB([xpcockpit],[initialize_beacon_client],,unset HAVE_LIBXPCOCKPIT)

if test -z "$HAVE_LIBXPCOCKPIT"; then
    AC_MSG_ERROR([Can't find xpcockpit library, or can't link a program to it.
    Please compile (make) and install (make install) the library in the xpcockpit 
    subdirectory libxpcockpit.
    In case you have installed it in a custom location, use:
    --with-libxpcockpit=<directory> to specify a custom installation directory.])
fi


# set installation prefix to current directory if none was given
#if test "x$prefix" = xNONE; then
#    prefix=`pwd`;
#fi
AC_MSG_NOTICE([xpleo will be installed in $prefix])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

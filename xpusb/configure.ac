#                                            -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.50])
AC_INIT([xpusb], [0.60], [Reto Stockli])
AC_CONFIG_SRCDIR([src/dictionary.c])

# produce makefiles (turn on warnings on automake!)
AM_INIT_AUTOMAKE([-Wall])

# AM_CONFIG_HEADER does more than AC_CONFIG_HEADERS
AM_CONFIG_HEADER([config.h])

# Checks for programs
# AC_PROG_CC redefines CFLAGS, so save original CFLAGS and reapply them
OLD_CFLAGS=$CFLAGS
AC_PROG_CC
CFLAGS=$OLD_CFLAGS

# Checks for Operating System
AC_CANONICAL_HOST
case $host_os in
linux*)
    AC_DEFINE([LINUX],[],[Compile for Linux])
    os=linux
    ;;
darwin*)
    AC_DEFINE([DARWIN],[],[Compile for Darwin])
    LIBS="$LIBS -framework IOKit -framework CoreFoundation"
    os=darwin
    ;;
*)
    AC_MSG_ERROR([Unsupported operating system. Currently only Linux and Darwin/OSX are supported.])
esac

AM_CONDITIONAL([OS_LINUX], [test "x$os" = xlinux])
AM_CONDITIONAL([OS_DARWIN], [test "x$os" = xdarwin])

# Checks for libraries.
AC_MSG_CHECKING([whether a location for the libusb-1.0 library was specified])
AC_ARG_WITH([libusb],
[AS_HELP_STRING([--with-libusb=<directory>],
                [Specify location the libusb-1.0 library. Configure will expect
to find subdirs include and lib. If none is given, by default hidraw and hidapi is used])],
                [LIBUSBDIR=$with_libusb])
if test -z "$LIBUSBDIR" ; then
# no libusb path was provided: use HIDAPI to access HID devices directly
    AC_MSG_RESULT([no])
    HAVE_LIBUSB=no
    HAVE_HIDRAW=yes
    AC_DEFINE([HIDAPI],[],[Use HIDAPI to access HID devices directly])
else
# libusb path was provided: use libusb-1.0 library to access HID devices as USB devices
    AC_MSG_RESULT([$LIBUSBDIR])
    LIBS="$LIBS -L$LIBUSBDIR/lib"
    CPPFLAGS="$CPPFLAGS -I$LIBUSBDIR/include"
    HAVE_LIBUSB=yes
    HAVE_HIDRAW=no
    AC_DEFINE([LIBUSB],[],[Use LIBUSB to access HID devices as regular USB devices])

    # check for library presence
    AC_CHECK_LIB(usb-1.0, main, ,HAVE_LIBUSB=no)
    if test "x$HAVE_LIBUSB" = xno; then
	AC_MSG_ERROR([Can't find libusb-1.0 library, or can't link a program to it.])
    fi
fi

# on linux, check for udev, rt and math library - also, for SDL and SDL_ttf libraries
if test "x$os" = xlinux; then
    if test "x$HAVE_HIDRAW" = xyes; then
	AC_CHECK_LIB(udev,main,,AC_MSG_ERROR([Could not find the udev library]))
    fi
    AC_CHECK_LIB(rt,clock_gettime,,AC_MSG_ERROR([Could not find the rt library]))
    AC_CHECK_LIB(m,main,,AC_MSG_ERROR([Could not find the math library]))
#    AC_CHECK_LIB(SDL,main,,AC_MSG_ERROR([Could not find the SDL library]))
#    AC_CHECK_LIB(SDL_ttf,main,,AC_MSG_ERROR([Could not find the SDL_ttf library]))
fi

# Enable Threading support
CFLAGS="$CFLAGS -pthread"

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
    CFLAGS="$CFLAGS -g -Wall -Wno-uninitialized -O0"
else
    AC_DEFINE([NDEBUG],[],[No-debug Mode])
    CFLAGS="$CFLAGS -O2"
fi

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h stdlib.h string.h sys/ioctl.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday memmove memset pow socket strchr strtol])


# set installation prefix to current directory if none was given
if test "x$prefix" = xNONE; then
    prefix=`pwd`;
fi
AC_MSG_NOTICE([usbiocards will be installed in $prefix])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

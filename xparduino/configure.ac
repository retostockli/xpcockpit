#                                            -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.50])
AC_INIT([xparduino], [0.10], [Reto Stockli])
AC_CONFIG_SRCDIR([src/xparduino.c])

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
case $host_os in
linux*)
    os=linux
    ;;
darwin*)
    os=darwin
    ;;
mingw*)
    os=win
    AC_DEFINE([WIN],[1],[Define if Host Type is Windows])
    ;;
*)
    AC_MSG_ERROR([Unsupported operating system. Currently only Linux and Darwin/OSX are supported.])
esac

AM_CONDITIONAL([OS_LINUX], [test "x$os" = xlinux])
AM_CONDITIONAL([OS_DARWIN], [test "x$os" = xdarwin])
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
AC_CHECK_FUNCS([gettimeofday memmove memset pow socket strchr strtol])
AC_CHECK_LIB([m],[roundf])
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
AC_MSG_NOTICE([$PACKAGE_NAME will be installed in $prefix])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

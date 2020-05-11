#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.50])
AC_INIT([xpserver], [0.61], [Reto Stockli])
AC_CONFIG_SRCDIR([src/xpserver.c])

# produce makefiles (turn on warnings on automake!)
AM_INIT_AUTOMAKE([-Wall])

# AM_CONFIG_HEADER does more than AC_CONFIG_HEADERS
AM_CONFIG_HEADER([config.h])

# Checks for programs
# AC_PROG_CC redefines CFLAGS, so save original CFLAGS and reapply them
OLD_CFLAGS=$CFLAGS
AC_PROG_CC
AC_PROG_CXX
CFLAGS=$OLD_CFLAGS

# Checks for Operating System
AC_CANONICAL_HOST
case $host_os in
linux*)
	OS=lin
        ;;
darwin*)
	OS=mac
        ;;
*)
        AC_MSG_ERROR([Unsupported operating system. Currently only Linux and Darwin/OSX are supported.])
esac

AM_CONDITIONAL([OS_LINUX], [test "x$OS" = xlin])
AM_CONDITIONAL([OS_APPLE], [test "x$OS" = xmac])

# Debug compilation support
AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [enable debug data generation (def=no)])],
    [debugit=yes],
    [debugit=no])
AC_MSG_RESULT([$debugit])

if test x"$debugit" = x"yes"; then
    AC_DEFINE([DEBUG],[],[Debug Mode])
    CFLAGS="$CFLAGS -g -Wall -Wno-uninitialized -O0"
else
    AC_DEFINE([NDEBUG],[],[No-debug Mode])
    CFLAGS="$CFLAGS -O3"
fi

# Check for X-Plane root directory
#HAVE_XPLANEDIR="yes"
#AC_MSG_CHECKING([whether the X-Plane installation directory was specified])
#AC_ARG_WITH([xplanedir],
#[AS_HELP_STRING([--with-xplanedir=<directory>],
#		[Specify the installation directory of X-Plane. Configure will expect to find the 
#subdirectory 'Resources' in there.])],
#		[xplanedir=$with_xplanedir])
#		
#if test -z "$xplanedir"; then
#    AC_MSG_RESULT([no])
#    AC_MSG_ERROR([Please specify the installation directory of X-Plane by use of --with-xplanedir=<directory>.])
#else
#    AC_MSG_RESULT([$xplanedir])
#    
#    if test ! -d "$xplanedir/Resources"; then
#	AC_MSG_ERROR([X-Plane is not installed in the specified directory. Please provide a correct installation directory.])
#    fi
#fi

# set installation prefix to current directory if none was given
if test "x$prefix" = xNONE; then
    prefix=`pwd`;
fi
AC_MSG_NOTICE([$PACKAGE_NAME will be installed in $prefix])

# Checks for libraries.
# none needed

# Substitution
AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_LDFLAGS])
AC_SUBST([OS])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_HEADER_STDBOOL
#AC_TYPE_UINT32_T

# Checks for library functions.
AC_CHECK_FUNCS([inet_ntoa memset pow select socket strchr strtol malloc])
AC_CHECK_LIB([m],[roundf])

#AC_CONFIG_FILES([Makefile src/32/Makefile src/64/Makefile])
AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT

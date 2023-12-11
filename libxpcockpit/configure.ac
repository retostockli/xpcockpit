#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.50])
AC_INIT([libxpcockpit], [0.2], [Reto Stockli])
AC_CONFIG_SRCDIR([src/handleserver.c])

# produce makefiles (turn on warnings on automake!)
AM_INIT_AUTOMAKE([-Wall])

# AM_CONFIG_HEADER does more than AC_CONFIG_HEADERS
AC_CONFIG_HEADERS([config.h])

# Checks for programs
# AC_PROG_CC redefines CFLAGS, so save original CFLAGS and reapply them
OLD_CFLAGS=$CFLAGS
AC_PROG_CC
AC_PROG_CXX
CFLAGS=$OLD_CFLAGS

# Check for library generation
AC_PROG_RANLIB
AM_PROG_AR

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
*)
        AC_MSG_ERROR([Unsupported operating system. Currently only Linux, Darwin/OSX and MinGW are supported.])
esac

AM_CONDITIONAL([OS_LIN], [test "x$os" = xlin])
AM_CONDITIONAL([OS_MAC], [test "x$os" = xmac])
AM_CONDITIONAL([OS_WIN], [test "x$os" = xwin])

	
# Debug compilation support
AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [enable debug data generation (default=no)])],
    [debugit=yes],
    [debugit=no])
AC_MSG_RESULT([$debugit])

if test x"$debugit" = x"yes"; then
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

# do not allow multiple definitions
CFLAGS="$CFLAGS -fno-common"

# Enable Threading support
CFLAGS="$CFLAGS -pthread"

# set installation prefix to current directory if none was given
#if test "x$prefix" = xNONE; then
#    prefix=`pwd`;
#fi
AC_MSG_NOTICE([$PACKAGE_NAME will be installed in $prefix])

# Checks for libraries.
# none needed

# Substitution
AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_LDFLAGS])
AC_SUBST([os])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h stdio.h stdbool.h math.h signal.h sys/types.h sys/time.h errno.h])
if test "x$os" = xwin; then
AC_CHECK_HEADERS([winsock2.h])
else
AC_CHECK_HEADERS([arpa/inet.h sys/ioctl.h sys/socket.h netinet/tcp.h netinet/in.h netinet/ip.h])
fi

# Checks for library functions.
AC_CHECK_FUNCS([inet_ntoa memset pow select socket strchr strtol malloc])
AC_CHECK_LIB(m,roundf,,AC_MSG_ERROR([Could not find the math library]))
if test "x$os" = xwin; then
    AC_CHECK_LIB([ws2_32],[socket])
fi

AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT

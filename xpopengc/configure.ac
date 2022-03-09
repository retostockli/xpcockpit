#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.50])
AC_INIT([xpopengc], [0.81], [Reto Stockli])
AC_CONFIG_SRCDIR([src/main/main.cpp])
AM_INIT_AUTOMAKE([-Wall subdir-objects])
AM_CONFIG_HEADER([config.h])

# Determine operating system
AC_CANONICAL_HOST
AC_MSG_CHECKING([operating system])
case $host_os in
linux*)
    os=lin
    AC_DEFINE([LIN],[1],[Define if Host Type is Linux])
    ;;
darwin*)
    os=mac
    AC_DEFINE([MAC],[1],[Define if Host Type is Apple])
    ;;
mingw*)
    os=win
    AC_DEFINE([WIN],[1],[Define if Host Type is Windows])
    LDFLAGS="$LDFLAGS -static"
    LIBS="$LIBS -mconsole -mwindows -lws2_32 -lsetupapi -lole32 -luuid -lcomctl32"
    ;;
**)
        AC_MSG_ERROR([Unsupported operating system $host_os. Currently only Linux, OSX and MinGW are supported.])
esac
AC_MSG_RESULT([$os])

AM_CONDITIONAL([OS_LINUX], [test "x$os" = "xlin"])
AM_CONDITIONAL([OS_DARWIN], [test "x$os" = "xmac"])
AM_CONDITIONAL([OS_WINDOWS], [test "x$os" = "xwin"])

# Checks for programs.
OLD_CXXFLAGS=$CXXFLAGS
OLD_CFLAGS=$CFLAGS
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR
CXXFLAGS=$OLD_CXXFLAGS
CFLAGS=$OLD_CFLAGS

# Add support for debugging and strict compilation flags
AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [enable debug data generation (default=yes)])],
    [debugit="$enableval"],
    [debugit=no])
AC_MSG_RESULT([$debugit])

if test "x$debugit" = xyes; then
    AC_DEFINE([DEBUG],[],[Debug Mode])
# DEBUGGING USING LEAK CHECK
#    CXXFLAGS="$CXXFLAGS -g -Wall -Wpedantic -Wno-uninitialized -fsanitize=address"
# REGULAR DEBUGGING
#    CXXFLAGS="$CXXFLAGS -g -Wall -Wno-uninitialized"
# DEBUGGING USING GPROF PROFILING
    CXXFLAGS="$CXXFLAGS -pg -g -Wall -Wno-uninitialized"
else
    AC_DEFINE([NDEBUG],[],[No-debug Mode])
    CXXFLAGS="$CXXFLAGS -O2"
fi

CXXFLAGS="$CXXFLAGS -fno-common"
CFLAGS="$CFLAGS -fno-common"

# Enable Threading support
CXXFLAGS="$CXXFLAGS -pthread"

# Checks for libraries and headers.

# check for OpenGL libraries and headers
# Not tested on Linux
HAVE_GL=yes
AC_MSG_CHECKING([whether a location for the OpenGL library was specified])
AC_ARG_WITH([gl],
[AS_HELP_STRING([--with-gl=<directory>],
                [Specify location the OpenGL library. Configure will expect
to find headers in <directory>/include and libraries in <directory>/lib.])],
                [GLDIR=$with_gl])
if test -z "$GLDIR" ; then
    AC_MSG_RESULT([no])
else
    AC_MSG_RESULT([$GLDIR])
    LIBS="$LIBS -L$GLDIR/lib"
    CPPFLAGS="$CPPFLAGS -I$GLDIR/include"
fi

if test "x$os" = xmac; then
# OSX Darwin: no more checking, just link the frameworks
   LIBS="$LIBS -framework AGL -framework OpenGL"
else 
  if test "x$os" = xlin; then
# Linux: check presence of OpenGL library and headers
   AC_CHECK_LIB(GL, main, , unset HAVE_GL, -lGL)
   AC_CHECK_LIB(GLU, main, , unset HAVE_GL, -lGLU)
#   AC_CHECK_LIB(glut, main, , unset HAVE_GL,-lglut)

   AC_CHECK_HEADERS(GL/gl.h GL/glu.h GL/glext.h, , unset HAVE_GL,
       [
	#ifdef WIN32
        #include <windows.h>
        #endif
        #if HAVE_GL_GL_H
        #include <GL/gl.h>
        #endif
        #if HAVE_GL_GLU_H
        #include <GL/glu.h>
        #endif
        #if HAVE_GL_GLUT_H
        #include <GL/glut.h>
        #endif
       ])
  else
# Windows: check presence of OpenGL32 / glu32 library and headers
   AC_CHECK_LIB(opengl32, main, , unset HAVE_GL, -lopengl32)
   AC_CHECK_LIB(glu32, main, , unset HAVE_GL, -lglu32)
#   AC_CHECK_LIB(glut32, main, , unset HAVE_GL, -lglut32)

   AC_CHECK_HEADERS(GL/gl.h GL/glu.h GL/glext.h, , unset HAVE_GL,
       [
	#ifdef WIN32
        #include <windows.h>
        #endif
        #if HAVE_GL_GL_H
        #include <GL/gl.h>
        #endif
        #if HAVE_GL_GLU_H
        #include <GL/glu.h>
        #endif
        #if HAVE_GL_GLUT_H
        #include <GL/glut.h>
        #endif
       ])

fi
fi

if test -z "$HAVE_GL"; then
    AC_MSG_ERROR([Can't find OpenGL libraries, or can't link a program to it.
please use --with-gl=<directory> to specify a custom installation directory for OpenGL.])
fi


# check for the FLTK libraries and headers
HAVE_FLTK=yes
AC_MSG_CHECKING([whether a location for the fltk library was specified])
AC_ARG_WITH([fltk],
[AS_HELP_STRING([--with-fltk=<directory>],
                [Specify location the fltk library. Configure will expect
to find headers in <directory>/include/fltk and libraries in <directory>/lib.])],
                [FLTKDIR=$with_fltk])
if test -z "$FLTKDIR" ; then
    AC_MSG_RESULT([no])
else
    AC_MSG_RESULT([$FLTKDIR])
    LIBS="$LIBS -L$FLTKDIR/lib"
    CPPFLAGS="$CPPFLAGS -I$FLTKDIR/include"
fi

AC_CHECK_LIB(fltk, main, , unset HAVE_FLTK)
AC_CHECK_LIB(fltk_gl, main, , unset HAVE_FLTK)
AC_CHECK_LIB(fltk_forms, main, , unset HAVE_FLTK)
AC_CHECK_LIB(fltk_images, main, , unset HAVE_FLTK)
AC_CHECK_HEADER(FL/filename.H, ,unset HAVE_FLTK)

if test -z "$HAVE_FLTK"; then
    AC_MSG_ERROR([Can't find fltk library, or can't link a program to it.
please use --with-fltk=<directory> to specify a custom installation directory for fltk.])
fi


# check for FREETYPE2 library and headers
HAVE_FREETYPE=yes
AC_MSG_CHECKING([whether a location for the freetype library was specified])
AC_ARG_WITH([freetype],
[AS_HELP_STRING([--with-freetype=<directory>],
                [Specify location the freetype library. Configure will expect
to find headers in <directory>/include/freetype and libraries in <directory>/lib.])],
                [FREETYPEDIR=$with_freetype])
if test -z "$FREETYPEDIR" ; then
    AC_MSG_RESULT([no])
    CPPFLAGS="$CPPFLAGS -I/usr/include/freetype2" 
else
    AC_MSG_RESULT([$FREETYPEDIR])
    LIBS="$LIBS -L$FREETYPEDIR/lib"
    CPPFLAGS="$CPPFLAGS -I$FREETYPEDIR/include/freetype2" 
fi

AC_CHECK_LIB(z, main)
AC_CHECK_LIB(freetype, main, , unset HAVE_FREETYPE)
AC_CHECK_HEADER(ft2build.h, , unset HAVE_FREETYPE)
#AC_CHECK_HEADER(freetype.h, , unset HAVE_FREETYPE,
#				       [include <ft2build.h>
#				        include FT_FREETYPE_H
#				])

if test -z "$HAVE_FREETYPE"; then
    AC_MSG_ERROR([Can't find freetype library, or can't link a program to it.
please use --with-freetype=<directory> to specify a custom installation directory for freetype.])
fi


# check for the FTGL library and headers (depends on freetype)
HAVE_FTGL=yes
AC_MSG_CHECKING([whether a location for the ftgl library was specified])
AC_ARG_WITH([ftgl],
[AS_HELP_STRING([--with-ftgl=<directory>],
                [Specify location the ftgl library. Configure will expect
to find headers in <directory>/include/ftgl and libraries in <directory>/lib.])],
                [FTGLDIR=$with_ftgl])
if test -z "$FTGLDIR" ; then
    AC_MSG_RESULT([no])
else
    AC_MSG_RESULT([$FTGLDIR])
    LIBS="$LIBS -L$FTGLDIR/lib"
    CPPFLAGS="$CPPFLAGS -I$FTGLDIR/include"
fi

AC_CHECK_LIB(ftgl, main, , unset HAVE_FTGL)
AC_CHECK_HEADER(FTGL/ftgl.h, , unset HAVE_FTGL)

if test -z "$HAVE_FTGL"; then
    AC_MSG_ERROR([Can't find ftgl library, or can't link a program to it.
please use --with-ftgl=<directory> to specify a custom installation directory for ftgl.])
fi

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h stdio.h stdbool.h math.h signal.h sys/types.h sys/time.h errno.h])
if test "x$os" = xwin; then
AC_CHECK_HEADERS([winsock2.h])
else
AC_CHECK_HEADERS([arpa/inet.h sys/ioctl.h sys/socket.h netinet/tcp.h netinet/in.h netinet/ip.h])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_UINT32_T

# Checks for library functions.
AC_CHECK_FUNCS([bzero floor memset socket sqrt strcspn malloc])

# Add local OpenGC source code directories to include directives
CPPFLAGS="$CPPFLAGS -I$PWD/src/base -I$PWD/src/datasources -I$PWD/src/navigation -I$PWD/src/gauges -I$PWD/src/main"

# Substitute
AC_SUBST(AM_CXXFLAGS)
AC_SUBST(AM_CPPFLAGS)
AC_SUBST(AM_LDFLAGS)

if test "x$prefix" = xNONE; then
    prefix=`pwd`;
fi
AC_MSG_NOTICE([opengc will be installed in $prefix])

# Define Packagename for CPP directive in C-based datasource code
AC_DEFINE([XPOPENGC],[],[CPP directive in C-based datasource code])

AC_CONFIG_FILES([Makefile
src/base/Makefile
src/datasources/Makefile
src/gauges/Makefile
src/main/Makefile
src/navigation/Makefile])

AC_OUTPUT

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.50])
AC_INIT([xpopengc], [0.80], [Reto Stockli])
AC_CONFIG_SRCDIR([src/main/main.cpp])
AM_INIT_AUTOMAKE([-Wall subdir-objects])
AM_CONFIG_HEADER([config.h])

# Determine operating system
AC_CANONICAL_HOST
AC_MSG_CHECKING([operating system])
case $host_os in
linux*)
        os=linux;;
darwin*)
        os=darwin;;
*)
        AC_MSG_ERROR([Unsupported operating system $host_os. Currently only Linux and Darwin/OSX are supported. Please add support for $host_os to configure.ac and Makefile.am in all source code directories.])
esac
AC_MSG_RESULT([$os])

AM_CONDITIONAL([OS_LINUX], [test "x$os" = "xlinux"])
AM_CONDITIONAL([OS_DARWIN], [test "x$os" = "xdarwin"])

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
    CXXFLAGS="$CXXFLAGS -O3"
fi

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

if test "x$os" = xdarwin; then
# OSX Darwin: no more checking, just link the frameworks
   LIBS="$LIBS -framework AGL -framework OpenGL"
else
# Linux: check presence of OpenGL library and headers
   AC_CHECK_LIB(GL, main, , unset HAVE_GL, -lGL)
   AC_CHECK_LIB(GLU, main, , unset HAVE_GL, -lGL)
#   AC_CHECK_LIB(GLUT, main, , unset HAVE_GL,-lGL)

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
       ])
fi

if test -z "$HAVE_GL"; then
    AC_MSG_ERROR([Can't find OpenGL libraries, or can't link a program to it.
please use --with-gl=<directory> to specify a custom installation directory for OpenGL.])
fi


# check for PLIB library and headers
HAVE_PLIB=yes
AC_MSG_CHECKING([whether a location for the plib library was specified])
AC_ARG_WITH([plib],
[AS_HELP_STRING([--with-plib=<directory>],
                [Specify location the plib library. Configure will expect
to find headers in <directory>/include/plib and libraries in <directory>/lib.])],
                [PLIBDIR=$with_plib])
if test -z "$PLIBDIR" ; then
    AC_MSG_RESULT([no])
else
    AC_MSG_RESULT([$PLIBDIR])
    LIBS="$LIBS -L$PLIBDIR/lib"
    CPPFLAGS="$CPPFLAGS -I$PLIBDIR/include/plib"
fi

#AC_CHECK_LIB(plibnet, main, , unset HAVE_PLIB)
#AC_CHECK_HEADER(netSocket.h, , unset HAVE_PLIB)
#if test -z "$HAVE_PLIB"; then
#    AC_MSG_ERROR([Can't find plib library, or can't link a program to it.
#please use --with-plib=<directory> to specify a custom installation directory for plib.])
#fi


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

AC_CHECK_LIB(z, main)

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h netinet/in.h stdlib.h string.h sys/time.h sys/ioctl.h sys/socket.h unistd.h])

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
#AC_SUBST(AM_LDFLAGS)

if test "x$prefix" = xNONE; then
    prefix=`pwd`;
fi
AC_MSG_NOTICE([opengc will be installed in $prefix])


AC_CONFIG_FILES([Makefile
src/base/Makefile
src/datasources/Makefile
src/gauges/Makefile
src/main/Makefile
src/navigation/Makefile])

AC_OUTPUT

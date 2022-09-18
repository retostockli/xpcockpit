#!/bin/sh

./configure --enable-debug --build x86_64-pc-linux-gnu --host x86_64-w64-mingw32

# First compile FLTK for minGW and install in minGW directories
# cd fltk-*
# remove "test" in DIRS of Makefile
# ./configure --build x86_64-pc-linux-gnu --host x86_64-w64-mingw32 --prefix=/usr/x86_64-w64-mingw32
# make
# sudo make install

# Compile Freetype for minGW and install in minGW directories
# ./configure --build x86_64-pc-linux-gnu --host x86_64-w64-mingw32 --prefix=/usr/x86_64-w64-mingw32 --enable-freetype-config --without-png --without-zlib
# make
# sudo make install

# Compile ftgl for minGW and install in minGW directories
# first replace the search for standard GL/GLU wtih windows GL/GLU
# sed -i 's/lGLU/lglu32/g' configure
# sed -i 's/lGL/lopengl32/g' configure
# ./configure --build x86_64-pc-linux-gnu --host x86_64-w64-mingw32 --with-ft-prefix=/usr/x86_64-w64-mingw32 --prefix=/usr/x86_64-w64-mingw32
# make
# sudo make install
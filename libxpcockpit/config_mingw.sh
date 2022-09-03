#!/bin/sh
# Configure script for compiling with minGW on Linux
# For compiling with minGW on Windows please use the regular configure)

# 64 bit
./configure --build x86_64-pc-linux-gnu --host x86_64-w64-mingw32
# 32 bit
#./configure --build x86_64-pc-linux-gnu --host i686-w64-mingw32 --enable-32



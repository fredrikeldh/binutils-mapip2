#!/bin/sh

# Note:
# Make sure the GCC source code has UNIX (LF) line endings.
# CRLF line endings will cause mysterious configuration bugs.

#---------------------------------------------------------------------------------
# Source and Install directories
#---------------------------------------------------------------------------------

builddir=../build/release

# The sourcecode dir,
# relative to the builddir.
# This must be specified in the format shown here
# as one of the tools built during the process will fail
# if absolute paths are specified.
# The example here assumes that the source directory
# is at the same level as the script.
SRCDIR=../../gdb

LIBDECNUMBERDIR=../../libdecnumber

#---------------------------------------------------------------------------------
# set the path for the installed binutils
#---------------------------------------------------------------------------------

#export PATH=${PATH}:/c/cross-gcc/mosync/bin

#---------------------------------------------------------------------------------
# set the target and compiler flags
#---------------------------------------------------------------------------------

target=mapip2-unknown-elf
progpref=mapip2-

export CFLAGS='-O2 -pipe'
export CXXFLAGS='-O2 -pipe'

#---------------------------------------------------------------------------------
# build and install just the c compiler
#---------------------------------------------------------------------------------

mkdir -p $builddir/gdb
cd $builddir/gdb

mkdir -p ../libdecnumber
cd ../libdecnumber
../$LIBDECNUMBERDIR/configure

cd ../gdb
../$SRCDIR/configure \
	--target=$target \
	--program-prefix=$progpref

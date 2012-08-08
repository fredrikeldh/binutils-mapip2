#!/bin/sh

builddir=./build/release
SRCDIR=../..

mkdir -p $builddir
cd $builddir

export CFLAGS='-O2 -pipe'
export CXXFLAGS='-O2 -pipe'

$SRCDIR/configure \
	--target=mapip2-unknown-elf \
	--prefix=/usr/mapip2 \
	--program-prefix=mapip2- \
	--enable-gas \
	--enable-ld \
	--enable-lto \
	--disable-gdb \
	--with-system-zlib \
	-v

make

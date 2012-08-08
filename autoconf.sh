#!/bin/sh

autoconf-2.64 && \
cd bfd && autoconf-2.64 && autoheader-2.64 && automake-1.11 && cd .. && \
cd opcodes && autoconf-2.64 && autoheader-2.64 && automake-1.11 && cd .. && \
cd libiberty && autoconf-2.64 && autoheader-2.64 && cd .. && \
cd binutils && autoconf-2.64 && autoheader-2.64 && automake-1.11 && cd .. && \
cd gas && autoconf-2.64 && autoheader-2.64 && automake-1.11 && cd .. && \
cd ld && autoconf-2.64 && autoheader-2.64 && automake-1.11 && cd .. && \
cd gdb && autoconf-2.64 && autoheader-2.64 && cd .. && \
true

#!/bin/sh

CC=gcc
OUTFILE="run"
CFLAGS="-O2 -lraylib main.c"
echo "compiling: $CC $CFLAGS"
$CC $CFLAGS -o $OUTFILE


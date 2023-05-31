#!/bin/sh

CC=gcc
OUTFILE="run"
CFLAGS="-O2 -lraylib -lpng main.c"
echo "compiling: $CC $CFLAGS"
$CC $CFLAGS -o $OUTFILE
#echo "$CC $CFLAGS -DTHREADED -o multi"
#$CC $CFLAGS -DTHREADED -o multi
#echo "$CC $CFLAGS -o single"
#$CC $CFLAGS -o single


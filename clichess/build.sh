#!/usr/bin/sh

LDFLAGS=""

INCLUDES=""

CFLAGS="-Wall -Werror -Wno-unused-variable -Wno-unused-const-variable -g"

gcc main.c -o clichess $CFLAGS $INCLUDES $LDFLAGS


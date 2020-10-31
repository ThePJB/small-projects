#!/usr/bin/sh

LDFLAGS="-lSDL2 -lSDL2_image"

INCLUDES="-I/usr/include/SDL2 -Iinc/"

CFLAGS="-Wall -Werror -Wno-unused-variable -Wno-unused-const-variable -g"

gcc main.c -o particle $CFLAGS $INCLUDES $LDFLAGS


#LDFLAGS = -lSDL2 -lSDL2_image -lrt -lasound -ljack -lpthread -lportaudio -lm
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf
INCLUDES = -I/usr/include/SDL2 -Iinc/ -Irules/ -I.
CFLAGS = -Wall -Werror -Wno-unused-variable -Wno-unused-const-variable -g
SRCS = $(wildcard *.c)
SRCS += $(wildcard rules/*.c)

memesweeper: $(SRCS)
	clang $(SRCS) -o  agent $(CFLAGS) $(INCLUDES) $(LDFLAGS)

.PHONY: clean
clean:

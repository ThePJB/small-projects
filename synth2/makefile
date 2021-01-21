LDFLAGS = -lSDL2 -lSDL2_image -lrt -lasound -ljack -lpthread -lportaudio -lm

INCLUDES = -I/usr/include/SDL2 -Iinc/ -I. -Imodules/
CFLAGS = -Wall -Werror -Wno-unused-variable -Wno-unused-const-variable -Wno-unused-function -g
SRCS = $(wildcard *.c)
SRCS += $(wildcard modules/*.c) 

synth: $(SRCS)
	clang $(SRCS) -o  synth $(CFLAGS) $(INCLUDES) $(LDFLAGS)

.PHONY: clean
clean:

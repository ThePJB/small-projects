#LDFLAGS = -lSDL2 -lSDL2_image -lrt -lasound -ljack -lpthread -lportaudio -lm
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
INCLUDES = -I/usr/include/SDL2 -Iinc/ -Idanklib/ -I.
CFLAGS = -Wall -Werror -Wno-unused-variable -Wno-unused-const-variable -g -O3 -fsanitize=address -Wfatal-errors
SRCS = $(wildcard *.c)
SRCS += $(wildcard danklib/*.c)

snowkoban: $(SRCS)
	clang $(SRCS) -o  island $(CFLAGS) $(INCLUDES) $(LDFLAGS)

.PHONY: clean
clean:

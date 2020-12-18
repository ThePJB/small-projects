#LDFLAGS = -lSDL2 -lSDL2_image -lrt -lasound -ljack -lpthread -lportaudio -lm
LDFLAGS = -lSDL2 -lSDL2_image
INCLUDES = -I/usr/include/SDL2 -Iinc/
CFLAGS = -Wall -Werror -Wno-unused-variable -Wno-unused-const-variable -g
SRCS = $(wildcard *.c)

memesweeper: $(SRCS)
	clang $(SRCS) -o  memesweeper $(CFLAGS) $(INCLUDES) $(LDFLAGS)

.PHONY: clean
clean:
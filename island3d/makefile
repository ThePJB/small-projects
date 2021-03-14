LDFLAGS = -lSDL2 -lSDL2_image -lGL -ldl -lm -lfreetype

INCLUDES = -I/usr/include/SDL2

CFLAGS = -Wall -Werror -Wno-unused-variable -Wno-unused-const-variable -Wno-missing-braces -g -O3
CFLAGS += -fsanitize=address

SRCS = $(wildcard *.c)
SRCS += $(wildcard *.cpp)

island3d: $(SRCS)
	clang++ $(SRCS) -o island3d $(CFLAGS) $(INCLUDES) $(LDFLAGS)

.PHONY: clean
clean:
	rm island3d

CC		:= gcc
CFLAGS	:= -std=c11 -Wall -Werror -pedantic -g
TARGET  := myshell
SRCS	:= $(wildcard src/*.c)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

debug: CFLAGS += -fsanitize=address, undefined
debug: clean $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean debug
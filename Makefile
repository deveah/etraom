
CC=gcc
CFLAGS=-ansi -Wall -Wextra -g
LIBS=-lcurses -lm

SOURCES=*.c
EXECUTABLE=etraom

all:
	$(CC) $(SOURCES) $(CFLAGS) $(LIBS) -o $(EXECUTABLE)

clean:
	rm -rf $(EXECUTABLE)


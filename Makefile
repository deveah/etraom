
CC=gcc
CFLAGS=-ansi -Wall -Wextra -g
LIBS=-lcurses -lm

SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=etraom

all: $(OBJECTS)
	@$(CC) $(CFLAGS) $(OBJECTS) $(LIBS) -o $(EXECUTABLE)
	@echo Successfully built Etraom.

%.o: %.c
	@echo $@
	@$(CC) $(CFLAGS) $(LIBS) -c $^ -o $@

clean:
	@rm -rf $(EXECUTABLE) $(OBJECTS)


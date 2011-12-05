CC = gcc
CFLAGS = -Wall
OBJECTS = main.o algorithms.o

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o md6

%.o: %.c
	$(CC) $(CFLAGS) -c $<

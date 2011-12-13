CC = cc
CFLAGS = -Wall -g
LDADD = -lrt
OBJECTS = main.o algorithms.o tests.o ./bmp/bmp.o

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o md6 $(LDADD)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

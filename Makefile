CC = cc
CFLAGS = -Wall -O3
LDADD = -lrt
OBJECTS = main.o algorithms.o tests.o

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o md6 $(LDADD)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

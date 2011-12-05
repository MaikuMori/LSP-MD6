CC = cc
CFLAGS = -Wall
LDADD = -lrt
OBJECTS = main.o algorithms.o

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o md6 $(LDADD)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

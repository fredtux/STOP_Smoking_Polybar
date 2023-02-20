CC = gcc
CFLAGS = -Wall
LFLAGS =

all: stopsmoking-config stopsmoking example-fifo

stopsmoking-config: stopsmoking-config.c
	$(CC) $(CFLAGS) -o stopsmoking-config stopsmoking-config.c $(LFLAGS)

stopsmoking: stopsmoking.c
	$(CC) $(CFLAGS) -o stopsmoking stopsmoking.c $(LFLAGS)

example-fifo: example-fifo.c
	$(CC) $(CFLAGS) -o example-fifo example-fifo.c $(LFLAGS)

clean:
	$(RM) stopsmoking-config stopsmoking example-fifo
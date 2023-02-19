CC = gcc
CFLAGS = -Wall
LFLAGS = -lm

all: stopsmoking-config stopsmoking

stopsmoking-config: stopsmoking-config.c
	$(CC) $(CFLAGS) -o stopsmoking-config stopsmoking-config.c $(LFLAGS)

stopsmoking: stopsmoking.c
	$(CC) $(CFLAGS) -o stopsmoking stopsmoking.c $(LFLAGS)

clean:
	$(RM) stopsmoking-config stopsmoking
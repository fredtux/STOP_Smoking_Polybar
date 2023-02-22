CC = gcc
CFLAGS = -Wall
LFLAGS =

.PHONY: all clean

all: stopsmoking-config stopsmoking example-fifo stopsmoking-polybar

stopsmoking-config: stopsmoking-config.c
	$(CC) $(CFLAGS) -o stopsmoking-config stopsmoking-config.c $(LFLAGS)

stopsmoking: stopsmoking.c
	$(CC) $(CFLAGS) -o stopsmoking stopsmoking.c $(LFLAGS)

example-fifo: example-fifo.c
	$(CC) $(CFLAGS) -o example-fifo example-fifo.c $(LFLAGS)

stopsmoking-polybar: stopsmoking-polybar.c
	$(CC) $(CFLAGS) -D_XOPEN_SOURCE=700 -o stopsmoking-polybar stopsmoking-polybar.c $(LFLAGS)

clean:
	$(RM) stopsmoking-config stopsmoking example-fifo stopsmoking-polybar stopsmoking.service run_polybar_stopsmoking.sh

install:
	./installer.sh

uninstall:
	./uninstaller.sh
CC = gcc
CFLAGS = -Wall

all: rate edf

rate: real.c
	$(CC) $(CFLAGS) -DPROGRAM_NAME=\"rate\" real.c -o rate

edf: real.c
	$(CC) $(CFLAGS) -DPROGRAM_NAME=\"edf\" real.c -o edf

clean:
	rm -f rate edf
CC = gcc
CFLAGS = -O2 -Wall

MTSRC = mt19937ar-cok.c
MTOBJ = $(MTSRC:.c=.o)

all: internalclock similarity

internalclock: internalclock.o $(MTOBJ)
	$(CC) $(CFLAGS) -o $@ $@.o $(MTOBJ) -lm

similarity: similarity.o
	$(CC) $(CFLAGS) -o $@ $@.o -lgd -lm

clean:
	rm -f *.o internalclock similarity

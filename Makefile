CC=gcc
CFLAGS=-Wall -std=c2x
VPATH=test

.PHONY: all

all: CFLAGS+=-O3 -g
all: clib-test clib-testv

debug: CFLAGS+=-O0 -g
debug: clean clib-test clib-testv

clib-testv: testv.o
	$(CC) -o $@ $^  $(CFLAGS) $(EXTFLAGS)

clib-test: test.o 
	$(CC) -o $@ $^  $(CFLAGS) $(EXTFLAGS)

%.o: %.c
	$(CC) -o $@ -c $<  $(CFLAGS) $(EXTFLAGS)


clean:
	rm -rf *.o clib-test

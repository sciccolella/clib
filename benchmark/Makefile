CC=gcc
CFLAGS=-Wall -std=c2x 
BENCHOBJS=bhash32 bhash64

.PHONY: all

all: CFLAGS+=-O3 -g
all: $(BENCHOBJS)

bhash32: b-hash.c ../chash.h
	$(CC) -o $@ $^ $(CFLAGS) $(ARCH)

bhash64: b-hash64.c ../chash.h
	$(CC) -o $@ $^ $(CFLAGS) $(ARCH)

clean:
	rm -rf *.o $(BENCHOBJS)

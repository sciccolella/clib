CC=gcc
CFLAGS=-Wall -std=c2x
VPATH=test
TESTOBJS=clib-test-vect clib-test-vec clib-test-hash

.PHONY: all

all: CFLAGS+=-O3 -g
all: $(TESTOBJS)

debug: CFLAGS+=-O0 -g
debug: clean $(TESTOBJS)

t2: CFLAGS+=-O0 -g 
t2: test-hash2.c chash.h
	$(CC) -o $@ $^ $(CFLAGS)

clib-test-hash: test-hash.c chash.h
	$(CC) -o $@ $^ $(CFLAGS)

clib-test-vect: test-vect.c cvect.h
	$(CC) -o $@ $^ $(CFLAGS)

clib-test-vec: test-vec.c cvec.h
	$(CC) -o $@ $^ $(CFLAGS)

# clib-testv: testv.o
# 	$(CC) -o $@ $^  $(CFLAGS) $(EXTFLAGS)
#
# clib-test: test.o 
# 	$(CC) -o $@ $^  $(CFLAGS) $(EXTFLAGS)
#
# %.o: %.c
# 	$(CC) -o $@ -c $<  $(CFLAGS) $(EXTFLAGS)


clean:
	rm -rf *.o $(TESTOBJS)

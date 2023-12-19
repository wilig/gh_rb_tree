##
# RBTree
#
# @file
# @version 0.1
CC=gcc
CFLAGS=-g -O0 -std=c11 -Wall -pedantic -Wshadow -Wno-strict-prototypes -Wstrict-overflow

LD_LIBS=-lm -lrt

all: run-test

run-test: test
	./tests

test: tests.c
	$(CC) $(CFLAGS) $(INCLUDE) -o tests $^ $(LD_LIBS)

clean:
	rm -f tests

# end

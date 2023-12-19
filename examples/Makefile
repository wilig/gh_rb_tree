##
# RBTree
#
# @file
# @version 0.1
CC=gcc
CFLAGS=-g -O0 -std=c11 -Wall -pedantic -Wshadow -Wno-strict-prototypes -Wstrict-overflow

LD_LIBS=-lm -lrt

all: simple serenity

simple: simple.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $^ $(LD_LIBS)

serenity: serenity.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $^ $(LD_LIBS)

clean:
	rm -f simple serenity

# end

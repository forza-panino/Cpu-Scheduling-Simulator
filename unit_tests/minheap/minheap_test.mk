CC=gcc
CCOPTS=--std=gnu99 -Wall -D_HEAP_DEBUG_
AR=ar

OBJS=test_with_generics.o
EXT_OBJS=minheap.o

EXT_DIR=../../

HEADERS=$(EXT_DIR)minheap.h

BINS=test_with_generics

.phony: clean all


all:	$(BINS)
	chmod +x $(BINS).out

$(filter-out $(EXT_OBJS), %.o):	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

$(filter-out $(OBJS), %.o):	$(EXT_DIR)%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

test_with_generics:	test_with_generics.c $(EXT_OBJS)
	$(CC) $(CCOPTS) -o $@.out $^

clean:
	rm -rf *.o *~ $(OBJS) $(BINS)
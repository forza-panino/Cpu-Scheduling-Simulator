CC=gcc
CCOPTS=--std=gnu99 -Wall -D_HEAP_DEBUG_ -g -ggdb3
AR=ar

OBJS=test_with_generics.o
EXT_OBJS=minheap.o\
		fake_process.o\
		linked_list.o

EXT_DIR=../../

HEADERS=$(EXT_DIR)minheap.h\
		$(EXT_DIR)fake_process.h\
		$(EXT_DIR)linked_list.h

BINS=test_with_generics test_with_specialised

.phony: clean all


all:	$(BINS)
	chmod +x $(addsuffix .out, $(BINS))

$(filter-out $(EXT_OBJS), %.o):	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

$(filter-out $(OBJS), %.o):	$(EXT_DIR)%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

test_with_generics:	test_with_generics.c $(EXT_OBJS)
	$(CC) $(CCOPTS) -o $@.out $^

test_with_specialised:	test_with_specialised.c $(EXT_OBJS)
	$(CC) $(CCOPTS) -o $@.out $^

clean:
	rm -rf *.o *~ $(OBJS) $(EXT_OBJS) $(addsuffix .out, $(BINS))
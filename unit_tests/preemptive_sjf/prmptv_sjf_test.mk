CC=gcc
CCOPTS=--std=gnu99 -Wall -D_PRMPTV_SJF_TEST_
CCLIBS=-lm
AR=ar

OBJS=prmptv_sjf_test.o
EXT_OBJS=minheap.o\
		fake_process.o\
		linked_list.o\
		fake_os.o

EXT_DIR=../../

HEADERS=$(EXT_DIR)minheap.h\
		$(EXT_DIR)fake_process.h\
		$(EXT_DIR)linked_list.h\
		$(EXT_DIR)fake_os.h

BINS=prmptv_sjf_test sched_sim

.phony: clean all


all:	$(BINS)
	chmod +x $(addsuffix .out, $(BINS))

$(filter-out $(EXT_OBJS), %.o):	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

$(filter-out $(OBJS), %.o):	$(EXT_DIR)%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

prmptv_sjf_test:	prmptv_sjf_test.c $(EXT_OBJS)
	$(CC) $(CCOPTS) -o $@.out $^ $(CCLIBS)

sched_sim:	$(EXT_DIR)sched_sim.c $(EXT_OBJS)
	$(CC) $(CCOPTS) -o $@.out $^ $(CCLIBS)

clean:
	rm -rf *.o *~ $(OBJS) $(EXT_OBJS) $(addsuffix .out, $(BINS))
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fake_os.h"

FakeOS os;

#ifndef _PREDICTION_DEBUG_
#include "minheap.h"
#endif

#ifdef _PREDICTION_DEBUG_
typedef struct {
  int quantum;
} SchedRRArgs;

void schedRR(FakeOS* os, void* args_){
  SchedRRArgs* args=(SchedRRArgs*)args_;

  // look for the first process in ready
  // if none, return
  if (! os->ready.first)
    return;

  FakePCB* pcb=(FakePCB*) List_popFront(&os->ready);
  os->running=pcb;
  
  assert(pcb->events.first);
  ProcessEvent* e = (ProcessEvent*)pcb->events.first;
  assert(e->type==CPU);

  // look at the first event
  // if duration>quantum
  // push front in the list of event a CPU event of duration quantum
  // alter the duration of the old event subtracting quantum
  if (e->duration>args->quantum) {
    ProcessEvent* qe=(ProcessEvent*)malloc(sizeof(ProcessEvent));
    qe->list.prev=qe->list.next=0;
    qe->type=CPU;
    qe->duration=args->quantum;
    e->duration-=args->quantum;
    List_pushFront(&pcb->events, (ListItem*)qe);
  }
};
#else
// TODO: struct
void schedSJF(FakeOS* os, void* args_){
  printf("Hello world from SJF!\n");
};
#endif

int main(int argc, char** argv) {
  FakeOS_init(&os);

  #ifdef _PREDICTION_DEBUG_
  SchedRRArgs srr_args;
  srr_args.quantum=5;
  os.schedule_args=&srr_args;
  os.schedule_fn=schedRR;
  #else
  os.schedule_fn=schedSJF;
  os.schedule_args=0; //TODO
  #endif

  #ifndef _PREDICTION_DEBUG_
  MinHeap heap;
  Heap_init(&heap);
  os.ready=&heap;
  #endif
  
  for (int i=1; i<argc; ++i){
    FakeProcess new_process;
    int num_events=FakeProcess_load(&new_process, argv[i]);
    printf("loading [%s], pid: %d, events:%d",
           argv[i], new_process.pid, num_events);
    if (num_events) {
      FakeProcess* new_process_ptr=(FakeProcess*)malloc(sizeof(FakeProcess));
      *new_process_ptr=new_process;
      List_pushBack(&os.processes, (ListItem*)new_process_ptr);
    }
  }
  printf("num processes in queue %d\n", os.processes.size);
  #ifdef _PREDICTION_DEBUG_
  while(os.running
        || os.ready.first
        || os.waiting.first
        || os.processes.first){
    FakeOS_simStep(&os);
  }
  #else
  while(os.running
        || os.ready->size
        || os.waiting.first
        || os.processes.first){
    FakeOS_simStep(&os);
  }
  #endif
}

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fake_os.h"

FakeOS os;

#ifndef _PREDICTION_DEBUG_
#include "minheap.h"
#endif

#ifdef _PRMPTV_SJF_TEST_
#define QUANTUM 2
#else
#define QUANTUM 2
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
typedef struct {
  #ifndef _MULTI_CORE_
  int quantum;
  int time;
  #else
  int* quantum;
  int* time;
  #endif
} SchedSJFArgs;

#ifdef _MULTI_CORE_
void schedSimDestroy(void* args_) {
  SchedSJFArgs* args=(SchedSJFArgs*)args_;
  if (args->time)     free(args->time);
  if (args->quantum)  free(args->quantum);
  if (os.running)     free(os.running);
}
#endif

void schedSJF(FakeOS* os, void* args_){
  SchedSJFArgs* args=(SchedSJFArgs*)args_;

  #ifndef _MULTI_CORE_
  ++(args->time);
  #else
  for (int i=0; i<os->num_cpus; ++i){
    ++(args->time[i]);
  }
  #endif

  // look for the first process in ready
  // if none, return
  if (!os->ready->size){
    printf("No process in ready\n");
    return;
  }

  #ifndef _MULTI_CORE_
  if ((args->time %= args->quantum) && os->running){
    return;
  }

  args->time=0;
  printf("[SCHEDULER ACTIVATING]\n");

  // look if there is a process in running
  if (os->running) {
    Heap_insert(os->ready, (HeapItem*)os->running);
    os->running=0;
  }

  // extract the first process from ready
  os->running = (FakePCB*)Heap_extractMin(os->ready);
  #else
  for (int i=0; i<os->num_cpus; ++i){
    if ((args->time[i] %= args->quantum[i]) && os->running[i]){
      continue;
    }

    printf("[SCHEDULER ACTIVATING]\n");
    printf("\t[CPU %d]\n", i);

    args->time[i]=0;

    // look if there is a process in running
    if (os->running[i]) {
      Heap_insert(os->ready, (HeapItem*)os->running[i]);
      os->running[i]=0;
    }
  }

  // extract the first process from ready
  for (int i=0; i<os->num_cpus; ++i){
    if (!os->running[i]) {
      os->running[i] = (FakePCB*)Heap_extractMin(os->ready);
    }
  }
  #endif

};
#endif

int main(int argc, char** argv) {
  int ret = 0;
  FakeOS_init(&os);

  #ifdef _PREDICTION_DEBUG_
  SchedRRArgs srr_args;
  srr_args.quantum=5;
  os.schedule_args=&srr_args;
  os.schedule_fn=schedRR;
  #else
  SchedSJFArgs ssjf_args;
  ssjf_args.quantum=0;
  ssjf_args.time=0;
  os.schedule_fn=schedSJF;
  os.schedule_args=&ssjf_args;
  #endif

  #ifndef _PREDICTION_DEBUG_
  os.ready = (MinHeap*)malloc(sizeof(MinHeap));
  Heap_init(os.ready);
  #endif
  
  #ifdef _MULTI_CORE_
  if (argc < 2) {
    printf("Usage: %s <num_cpus> <process1> <process2> ...\n", argv[0]);
    goto  ERROR;
  }
  #endif

  for (int i=1; i<argc; ++i){
    #ifdef _MULTI_CORE_
    if (i==1){
      os.num_cpus=atoi(argv[i]);
      if (os.num_cpus<1){
        printf("Invalid number of cpus\n");
        goto ERROR;
      }
      os.running=(FakePCB**)malloc(sizeof(FakePCB*)*os.num_cpus);
      for (int j=0; j<os.num_cpus; ++j)
        os.running[j]=0;
      ssjf_args.quantum=(int*)malloc(sizeof(int)*os.num_cpus);
      for (int j=0; j<os.num_cpus; ++j)
        ssjf_args.quantum[j]=QUANTUM;
      ssjf_args.time=(int*)malloc(sizeof(int)*os.num_cpus);
      for (int i=0; i<os.num_cpus; ++i)
        ssjf_args.time[i]=-1;
      continue;
    }
    #endif
    #if !defined(_MULTI_CORE_) && !defined(_PREDICTION_DEBUG_)
      ssjf_args.time=-1;
      ssjf_args.quantum=QUANTUM;
    #endif
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
  #ifndef _MULTI_CORE_
  while(os.running
        || os.ready->size
        || os.waiting.first
        || os.processes.first){
    FakeOS_simStep(&os);
  }
  #else
  while(1){
    int running=0;
    for (int i=0; i<os.num_cpus; ++i){
      running+=os.running[i]!=0;
    }
    if (!running && !os.ready->size && !os.waiting.first && !os.processes.first)
      break;
    FakeOS_simStep(&os);
  }
  #endif
  #endif

  goto CLEAN;
#ifdef _MULTI_CORE_
ERROR:
  ret=1;
#endif
CLEAN:
  #ifdef _MULTI_CORE_
  schedSimDestroy(&ssjf_args);
  #endif
  FakeOS_destroy(&os);
  return ret;
}
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "fake_os.h"

#ifdef _PREDICTION_DEBUG_
#define MEASURE_WEIGHT 0.35
#define DEFAULT_PREDICTION 1
#else
#define MEASURE_WEIGHT 0.35
#define DEFAULT_PREDICTION 1
#include "minheap.h"
#include <stdint.h>
uintptr_t offset_heap_list;
#endif

void FakeOS_init(FakeOS* os) {
  os->running=0;
  #ifdef _PREDICTION_DEBUG_
  List_init(&os->ready);
  #else
  os->ready=malloc(sizeof(MinHeap));
  Heap_init(os->ready);
  FakePCB* tmp=(FakePCB*) malloc(sizeof(FakePCB));
  uintptr_t heap = (uintptr_t)&tmp->heap;
  uintptr_t list = (uintptr_t)&tmp->list;
  offset_heap_list = list - heap;
  #endif
  List_init(&os->waiting);
  List_init(&os->processes);
  os->timer=0;
  os->schedule_fn=0;
}

void updatePredictionPCB(FakePCB* pcb) {
  int old_prediction = pcb->predicted_duration;
  int measure = pcb->real_duration;
  int new_prediction = round(MEASURE_WEIGHT*measure + (1-MEASURE_WEIGHT)*old_prediction);
  pcb->predicted_duration = new_prediction;
  pcb->heap.key = new_prediction;
}

void FakeOS_createProcess(FakeOS* os, FakeProcess* p) {
  // sanity check
  assert(p->arrival_time==os->timer && "time mismatch in creation");
  // we check that in the list of PCBs there is no
  // pcb having the same pid
  assert( (!os->running || os->running->pid!=p->pid) && "pid taken");

  #ifdef _PREDICTION_DEBUG_
  ListItem* aux=os->ready.first;
  while(aux){
    FakePCB* pcb=(FakePCB*)aux;
    assert(pcb->pid!=p->pid && "pid taken");
    aux=aux->next;
  }
  #else
  for (int i=0; i < os->ready->size; i++) {
    FakePCB* pcb = (FakePCB*)os->ready->items[i];
    assert(pcb->pid!=p->pid && "pid taken");
  }
  #endif

  #ifdef _PREDICTION_DEBUG_
  aux=os->waiting.first;
  #else
  ListItem* aux=os->waiting.first;
  #endif
  while(aux){
    FakePCB* pcb=(FakePCB*)aux;
    assert(pcb->pid!=p->pid && "pid taken");
    aux=aux->next;
  }

  // all fine, no such pcb exists
  FakePCB* new_pcb=(FakePCB*) malloc(sizeof(FakePCB));
  new_pcb->list.next=new_pcb->list.prev=0;
  #ifndef _PREDICTION_DEBUG_
  new_pcb->heap.key = DEFAULT_PREDICTION;
  #endif
  new_pcb->pid=p->pid;
  new_pcb->events=p->events;
  new_pcb->predicted_duration=DEFAULT_PREDICTION;
  new_pcb->real_duration=0;

  assert(new_pcb->events.first && "process without events");

  // depending on the type of the first event
  // we put the process either in ready or in waiting
  ProcessEvent* e=(ProcessEvent*)new_pcb->events.first;
  switch(e->type){
  case CPU:
    #ifdef _PREDICTION_DEBUG_
    List_pushBack(&os->ready, (ListItem*) new_pcb);
    #else
    Heap_insert(os->ready, (HeapItem*) new_pcb);
    #endif
    break;
  case IO:
    #ifdef _PREDICTION_DEBUG_
    List_pushBack(&os->waiting, (ListItem*) new_pcb);
    #else
    List_pushBack(&os->waiting, &new_pcb->list);
    #endif
    break;
  default:
    assert(0 && "illegal resource");
    ;
  }
}




void FakeOS_simStep(FakeOS* os){
  
  printf("************** TIME: %08d **************\n", os->timer);

  printf("\tprocesses size:%d\n", os->processes.size);
  printf("\twaiting size:%d\n", os->waiting.size);
  //scan process waiting to be started
  //and create all processes starting now
  ListItem* aux=os->processes.first;
  while (aux){
    FakeProcess* proc=(FakeProcess*)aux;
    FakeProcess* new_process=0;
    if (proc->arrival_time==os->timer){
      new_process=proc;
    }
    aux=aux->next;
    if (new_process) {
      printf("\tcreate pid:%d\n", new_process->pid);
      new_process=(FakeProcess*)List_detach(&os->processes, (ListItem*)new_process);
      FakeOS_createProcess(os, new_process);
      free(new_process);
    }
  }

  // scan waiting list, and put in ready all items whose event terminates
  aux=os->waiting.first;
  while(aux) {
    #ifdef _PREDICTION_DEBUG_
    FakePCB* pcb=(FakePCB*)aux;
    #else
    FakePCB* pcb=(FakePCB*)((void*)aux - offset_heap_list);
    #endif
    aux=aux->next;
    ProcessEvent* e=(ProcessEvent*) pcb->events.first;
    printf("\twaiting pid: %d\n", pcb->pid);
    assert(e->type==IO);
    e->duration--;
    printf("\t\tremaining time:%d\n",e->duration);
    if (e->duration==0){
      printf("\t\tend burst\n");
      List_popFront(&pcb->events);
      free(e);
      #ifdef _PREDICTION_DEBUG_
      List_detach(&os->waiting, (ListItem*)pcb);
      #else
      List_detach(&os->waiting, &pcb->list);
      #endif
      if (! pcb->events.first) {
        // kill process
        printf("\t\tend process\n");
        free(pcb);
      } else {
        //handle next event
        e=(ProcessEvent*) pcb->events.first;
        switch (e->type){
        case CPU:
          printf("\t\tmove to ready\n");
          #ifdef _PREDICTION_DEBUG_
          List_pushBack(&os->ready, (ListItem*) pcb);
          #else
          Heap_insert(os->ready, (HeapItem*) pcb);
          printf("\t\t\tready size after insert (io):%d\n", os->ready->size);
          #endif
          break;
        case IO:
          printf("\t\tmove to waiting\n");
          #ifdef _PREDICTION_DEBUG_
          List_pushBack(&os->waiting, (ListItem*) pcb);
          #else
          List_pushBack(&os->waiting, &pcb->list);
          #endif
          break;
        }
      }
    }
  }

  

  // decrement the duration of running
  // increment real duration of runningpcb
  // if event over, destroy event
  // if next event IO, update CPU burst prediction and reset real duration
  // and reschedule process
  // if last event, destroy running
  FakePCB* running=os->running;
  printf("\tready size:%d\n", os->ready->size);
  printf("\trunning pid: %d\n", running?running->pid:-1);
  printf("\tremaining events:%d\n", running?running->events.size:-1);
  if (running) {
    ProcessEvent* e=(ProcessEvent*) running->events.first;
    assert(e->type==CPU);
    e->duration--;
    running->real_duration++;
    printf("\t\tremaining time:%d\n",e->duration);
    if (e->duration==0){
      printf("\t\tend burst\n");
      List_popFront(&running->events);
      free(e);
      if (! running->events.first) {
        printf("\t\tend process\n");
        free(running); // kill process
      } else {
        e=(ProcessEvent*) running->events.first;
        switch (e->type){
        case CPU:
          printf("\t\tmove to ready\n");
          #ifdef _PREDICTION_DEBUG_
          List_pushBack(&os->ready, (ListItem*) running);
          #else
          Heap_insert(os->ready, (HeapItem*) running);
          printf("\t\t\tready size after insert (event):%d\n", os->ready->size);
          #endif
          break;
        case IO:
          printf("\t\tmove to waiting\n");
          printf("\t\told prediction: %d\n", running->predicted_duration);
          printf("\t\treal duration: %d\n", running->real_duration);
          updatePredictionPCB(running);
          printf("\t\tnew prediction: %d\n", running->predicted_duration);
          running->real_duration=0;
          #ifdef _PREDICTION_DEBUG_
          List_pushBack(&os->waiting, (ListItem*) running);
          #else
          List_pushBack(&os->waiting, &running->list);
          #endif
          printf("\t\t\twaiting size after insert (event):%d\n", os->waiting.size);
          break;
        }
      }
      os->running = 0;
    }
  }


  // call schedule, if defined
  #ifdef _PREDICTION_DEBUG_
  if (os->schedule_fn && ! os->running){
    (*os->schedule_fn)(os, os->schedule_args); 
  }
  #endif

  // if running not defined and ready queue not empty
  // put the first in ready to run
  #ifdef _PREDICTION_DEBUG_
  if (! os->running && os->ready.first) {
    os->running=(FakePCB*) List_popFront(&os->ready);
  }
  #else
  if (! os->running && os->ready->size) {
    os->running=(FakePCB*) Heap_extractMin(os->ready);
    printf("\t\t\tready size after extraction:%d\n", os->ready->size);
  }
  #endif

  ++os->timer;

}

void FakeOS_destroy(FakeOS* os) {
}

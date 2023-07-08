#include "fake_process.h"
#include "linked_list.h"
#pragma once

#ifndef _PREDICTION_DEBUG_
#include "minheap.h"
#endif

typedef struct {
  #ifndef _PREDICTION_DEBUG_
  HeapItem heap;
  #endif
  ListItem list;
  int pid;
  ListHead events;

//### added for burst time prediction
  int real_duration;
  int predicted_duration;
//###

} FakePCB;

struct FakeOS;
typedef void (*ScheduleFn)(struct FakeOS* os, void* args);

typedef struct FakeOS{
  #ifndef _MULTI_CORE_
  FakePCB* running;
  #else
  FakePCB** running;
  #endif
  #ifdef _PREDICTION_DEBUG_
  ListHead ready;
  #else
  MinHeap* ready;
  #endif
  ListHead waiting;
  int timer;
  ScheduleFn schedule_fn;
  void* schedule_args;

  #ifdef _MULTI_CORE_
  int num_cpus;
  #endif

  ListHead processes;
} FakeOS;

void FakeOS_init(FakeOS* os);
void FakeOS_simStep(FakeOS* os);
void FakeOS_destroy(FakeOS* os);
void updatePredictionPCB(FakePCB* pcb);

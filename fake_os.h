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
  FakePCB* running;
  #ifdef _PREDICTION_DEBUG_
  ListHead ready;
  #else
  MinHeap* ready;
  #endif
  ListHead waiting;
  int timer;
  ScheduleFn schedule_fn;
  void* schedule_args;

  ListHead processes;
} FakeOS;

void FakeOS_init(FakeOS* os);
void FakeOS_simStep(FakeOS* os);
void FakeOS_destroy(FakeOS* os);
void updatePredictionPCB(FakePCB* pcb);

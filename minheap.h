#pragma once

typedef struct HeapItem {
  int key;
} HeapItem;

typedef struct MinHeap {
  HeapItem** items;
  int size;
  int capacity;
} MinHeap;

void Heap_init(MinHeap* heap);
void Heap_insert(MinHeap* heap, HeapItem* item);
HeapItem* Heap_extractMin(MinHeap* heap);

#ifdef _HEAP_DEBUG_
void Heap_fill_null(MinHeap* heap, int num_nulls);
#endif
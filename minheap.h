#pragma once

typedef struct HeapItem {
  int key;
} HeapItem;

typedef struct Heap {
  HeapItem** items;
  int size;
  int capacity;
} Heap;

void Heap_init(Heap* heap);
void Heap_insert(Heap* heap, HeapItem* item);
HeapItem* Heap_extractMin(Heap* heap);

#ifdef _HEAP_DEBUG_
void Heap_fill_null(Heap* heap, int num_nulls);
#endif
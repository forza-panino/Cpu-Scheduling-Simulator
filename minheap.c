#include "minheap.h"
#include <stdlib.h>

void Heap_init(MinHeap* heap) {
	heap->size=0;
	heap->capacity=1;
	heap->items=(HeapItem**)malloc(sizeof(HeapItem*)*heap->capacity);
}

void Heap_insert(MinHeap* heap, HeapItem* item) {
  if (heap->size==heap->capacity) {
		heap->capacity*=2;
		heap->items=(HeapItem**)realloc(heap->items, sizeof(HeapItem*)*heap->capacity);
	}
  heap->items[heap->size]=item;
  ++heap->size;
  int i=heap->size-1;
  while(i>0) {
    int parent=(i-1)/2;
    if (heap->items[parent]->key > heap->items[i]->key) {
      HeapItem* tmp=heap->items[parent];
      heap->items[parent]=heap->items[i];
      heap->items[i]=tmp;
      i=parent;
    } else {
      break;
    }
  }
}

HeapItem* Heap_extractMin(MinHeap* heap) {
  if (heap->size==0)
    return 0;
  HeapItem* min=heap->items[0];
  heap->items[0]=heap->items[heap->size-1];
  --heap->size;
  int i=0;
  while(i<heap->size) {
    int left=2*i+1;
    int right=2*i+2;
    int min=i;
    if (left<heap->size && heap->items[left]->key < heap->items[min]->key)
      min=left;
    if (right<heap->size && heap->items[right]->key < heap->items[min]->key)
      min=right;
    if (min!=i) {
      HeapItem* tmp=heap->items[min];
      heap->items[min]=heap->items[i];
      heap->items[i]=tmp;
      i=min;
    } else {
      break;
    }
  }
  return min;
}

#ifdef _HEAP_DEBUG_
void Heap_fill_null(MinHeap* heap, int num_nulls) {
  for (int i=0; i<num_nulls; ++i){
		if (heap->size==heap->capacity) {
			heap->capacity*=2;
			heap->items=(HeapItem**)realloc(heap->items, sizeof(HeapItem*)*heap->capacity);
		}
  	heap->items[heap->size]=0;
  	++heap->size;
	}
}
#endif
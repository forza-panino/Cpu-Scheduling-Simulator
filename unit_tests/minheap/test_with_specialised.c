#include "../../minheap.h"
#include "../../fake_process.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
    HeapItem heapItem;
    FakeProcess process;
} FakeProcessHeapItem;

int main(int argc, char** argv) {

	FakeProcessHeapItem heapItems[4] = {
    {{.key = 0}, {.pid = 0}},
    {{.key = 1}, {.pid = 1}},
    {{.key = 2}, {.pid = 2}},
    {{.key = 3}, {.pid = 3}}
    };

    MinHeap heap;
	Heap_init(&heap);

    Heap_insert(&heap, (HeapItem*) &heapItems[0]);
    Heap_insert(&heap, (HeapItem*) &heapItems[1]);
    Heap_insert(&heap, (HeapItem*) &heapItems[2]);
    Heap_insert(&heap, (HeapItem*) &heapItems[3]);

    assert(heap.items[0]->key==0);
    assert(heap.items[1]->key==1);
    assert(heap.items[2]->key==2);
    assert(heap.items[3]->key==3);

    HeapItem* min = Heap_extractMin(&heap);
    assert(min->key==0);
    FakeProcess* process = &(((FakeProcessHeapItem*)min)->process);
    assert(process->pid==0);

    min = Heap_extractMin(&heap);
    assert(min->key==1);
    process = &(((FakeProcessHeapItem*)min)->process);
    assert(process->pid==1);

    min = Heap_extractMin(&heap);
    assert(min->key==2);
    process = &(((FakeProcessHeapItem*)min)->process);
    assert(process->pid==2);

    min = Heap_extractMin(&heap);
    assert(min->key==3);
    process = &(((FakeProcessHeapItem*)min)->process);
    assert(process->pid==3);

    min = Heap_extractMin(&heap);
    assert(min==NULL);
}
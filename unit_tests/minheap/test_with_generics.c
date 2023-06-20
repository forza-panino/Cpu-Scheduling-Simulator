#include "../../minheap.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void test_init() {
  MinHeap heap;
  Heap_init(&heap);
  assert(heap.size==0);
  assert(heap.capacity==1);
  assert(heap.items);
}

void test_fill_null() {
  MinHeap heap;
  Heap_init(&heap);
  Heap_fill_null(&heap, 10);
  assert(heap.size==10);
  assert(heap.capacity==16);
  for (int i=0; i<heap.size; ++i)
    assert(heap.items[i]==0);

  Heap_fill_null(&heap, 20);
  assert(heap.size==30);
  assert(heap.capacity==32);
  for (int i=0; i<heap.size; ++i)
    assert(heap.items[i]==0);
}

void test_insert_1() {
	MinHeap heap;
	Heap_init(&heap);
	HeapItem item1={.key=1};
	HeapItem item2={.key=2};
	HeapItem item3={.key=3};
	HeapItem item4={.key=4};
	HeapItem item5={.key=5};
	HeapItem item6={.key=6};

	Heap_insert(&heap, &item3);
	assert(heap.size==1);
	assert(heap.capacity==1);
	assert(heap.items[0]->key==3);

	Heap_insert(&heap, &item2);
	assert(heap.size==2);
	assert(heap.capacity==2);
	assert(heap.items[0]->key==2);
	assert(heap.items[1]->key==3);

	Heap_insert(&heap, &item1);
	assert(heap.size==3);
	assert(heap.capacity==4);
	assert(heap.items[0]->key==1);
	assert(heap.items[1]->key==3);
	assert(heap.items[2]->key==2);

	Heap_insert(&heap, &item6);
	assert(heap.size==4);
	assert(heap.capacity==4);
	assert(heap.items[0]->key==1);
	assert(heap.items[1]->key==3);
	assert(heap.items[2]->key==2);
	assert(heap.items[3]->key==6);

	Heap_insert(&heap, &item5);
	assert(heap.size==5);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==1);
	assert(heap.items[1]->key==3);
	assert(heap.items[2]->key==2);
	assert(heap.items[3]->key==6);
	assert(heap.items[4]->key==5);

	Heap_insert(&heap, &item4);
	assert(heap.size==6);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==1);
	assert(heap.items[1]->key==3);
	assert(heap.items[2]->key==2);
	assert(heap.items[3]->key==6);
	assert(heap.items[4]->key==5);
	assert(heap.items[5]->key==4);

}

void test_insert_2() {
	MinHeap heap;
	Heap_init(&heap);
	HeapItem item5={.key=5};
	HeapItem item7={.key=7};
	HeapItem item6={.key=6};
	HeapItem item3={.key=3};
	HeapItem item2={.key=2};
	HeapItem item1={.key=1};

	Heap_insert(&heap, &item5);
	assert(heap.size==1);
	assert(heap.capacity==1);
	assert(heap.items[0]->key==5);

	Heap_insert(&heap, &item7);
	assert(heap.size==2);
	assert(heap.capacity==2);
	assert(heap.items[0]->key==5);
	assert(heap.items[1]->key==7);

	Heap_insert(&heap, &item6);
	assert(heap.size==3);
	assert(heap.capacity==4);
	assert(heap.items[0]->key==5);
	assert(heap.items[1]->key==7);
	assert(heap.items[2]->key==6);

	Heap_insert(&heap, &item3);
	assert(heap.size==4);
	assert(heap.capacity==4);
	assert(heap.items[0]->key==3);
	assert(heap.items[1]->key==5);
	assert(heap.items[2]->key==6);
	assert(heap.items[3]->key==7);

	Heap_insert(&heap, &item2);
	assert(heap.size==5);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==2);
	assert(heap.items[1]->key==3);
	assert(heap.items[2]->key==6);
	assert(heap.items[3]->key==7);
	assert(heap.items[4]->key==5);

	Heap_insert(&heap, &item1);
	assert(heap.size==6);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==1);
	assert(heap.items[1]->key==3);
	assert(heap.items[2]->key==2);
	assert(heap.items[3]->key==7);
	assert(heap.items[4]->key==5);
	assert(heap.items[5]->key==6);

}

void test_extractMin_1() {
	MinHeap heap;
	Heap_init(&heap);
	HeapItem item1={.key=1};
	HeapItem item2={.key=2};
	HeapItem item3={.key=3};
	HeapItem item4={.key=4};
	HeapItem item5={.key=5};
	HeapItem item6={.key=6};

	Heap_insert(&heap, &item3);
	Heap_insert(&heap, &item2);
	Heap_insert(&heap, &item1);
	Heap_insert(&heap, &item6);
	Heap_insert(&heap, &item5);
	Heap_insert(&heap, &item4);

	/*
	* avoiding asserts after each insert since they are tested before
	*/

	assert(heap.size==6);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==1);
	assert(heap.items[1]->key==3);
	assert(heap.items[2]->key==2);
	assert(heap.items[3]->key==6);
	assert(heap.items[4]->key==5);
	assert(heap.items[5]->key==4);

	HeapItem* min = Heap_extractMin(&heap);
	assert(min->key==1);
	assert(heap.size==5);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==2);
	assert(heap.items[1]->key==3);
	assert(heap.items[2]->key==4);
	assert(heap.items[3]->key==6);
	assert(heap.items[4]->key==5);

	min = Heap_extractMin(&heap);
	assert(min->key==2);
	assert(heap.size==4);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==3);
	assert(heap.items[1]->key==5);
	assert(heap.items[2]->key==4);
	assert(heap.items[3]->key==6);

	min = Heap_extractMin(&heap);
	assert(min->key==3);
	assert(heap.size==3);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==4);
	assert(heap.items[1]->key==5);
	assert(heap.items[2]->key==6);

	min = Heap_extractMin(&heap);
	assert(min->key==4);
	assert(heap.size==2);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==5);
	assert(heap.items[1]->key==6);

	min = Heap_extractMin(&heap);
	assert(min->key==5);
	assert(heap.size==1);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==6);

	min = Heap_extractMin(&heap);
	assert(min->key==6);
	assert(heap.size==0);
	assert(heap.capacity==8);
	
	min = Heap_extractMin(&heap);
	assert(min==NULL);
	assert(heap.size==0);
	assert(heap.capacity==8);

}

void test_extractMin_2() {
	MinHeap heap;
	Heap_init(&heap);
	HeapItem item5={.key=5};
	HeapItem item7={.key=7};
	HeapItem item6={.key=6};
	HeapItem item3={.key=3};
	HeapItem item2={.key=2};
	HeapItem item1={.key=1};

	Heap_insert(&heap, &item5);
	Heap_insert(&heap, &item7);
	Heap_insert(&heap, &item6);
	Heap_insert(&heap, &item3);
	Heap_insert(&heap, &item2);
	Heap_insert(&heap, &item1);

	/*
	* avoiding asserts after each insert since they are tested before
	*/

	assert(heap.size==6);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==1);
	assert(heap.items[1]->key==3);
	assert(heap.items[2]->key==2);
	assert(heap.items[3]->key==7);
	assert(heap.items[4]->key==5);
	assert(heap.items[5]->key==6);
	
	HeapItem* min = Heap_extractMin(&heap);
	assert(min->key==1);
	assert(heap.size==5);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==2);
	assert(heap.items[1]->key==3);
	assert(heap.items[2]->key==6);
	assert(heap.items[3]->key==7);
	assert(heap.items[4]->key==5);
	
	min = Heap_extractMin(&heap);
	assert(min->key==2);
	assert(heap.size==4);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==3);
	assert(heap.items[1]->key==5);
	assert(heap.items[2]->key==6);
	assert(heap.items[3]->key==7);

	min = Heap_extractMin(&heap);
	assert(min->key==3);
	assert(heap.size==3);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==5);
	assert(heap.items[1]->key==7);
	assert(heap.items[2]->key==6);
	
	min = Heap_extractMin(&heap);
	assert(min->key==5);
	assert(heap.size==2);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==6);
	assert(heap.items[1]->key==7);

	min = Heap_extractMin(&heap);
	assert(min->key==6);
	assert(heap.size==1);
	assert(heap.capacity==8);
	assert(heap.items[0]->key==7);

	min = Heap_extractMin(&heap);
	assert(min->key==7);
	assert(heap.size==0);
	assert(heap.capacity==8);
	
	min = Heap_extractMin(&heap);
	assert(min==NULL);
	assert(heap.size==0);
	assert(heap.capacity==8);
}

int main(int argc, char** argv) {
	test_init();
	test_fill_null();
	test_insert_1();
	test_insert_2();
	test_extractMin_1();
	test_extractMin_2();
}
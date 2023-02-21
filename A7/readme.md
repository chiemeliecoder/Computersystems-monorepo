Heap Management

What is heap management?

It represents the way SYSLINUX handles the dynamic memory allocation requests of modules through malloc() .

In my design where I would attach a metadata to the chunk of memory to be 
I would be using markers to tell if the block is used or not.If the block is free; 1 if the block is used.

Memory Heap Allocation Design

In the design you need to keep in mind the ability reuse the free memory when available. To optimize heap memeory we need to consider a few things like:

1.[Block Splitting]: When memory is re-used, it is very easy to simple mark the block is used to be 1 again even if the block is not a perfect fit. We should be able to split our memeory block to fit what memory is required.

2.[Free Lists]: we need to be able traverse both used and free blocks when walking through our memory.

3.[Memory Merging]: When memory is freed, two blocks of free memory may appear next to one-another we need to combine the blocks together to create one larger block. This solves external fragmentation

Below is an example of metadata attached to chunk of memory allocated to keep track of if the memory has been freed so it can be resused or merged or splited to fit exactly the requested memory i.e I reuest for 128 chunk of memeory and you have 500 you have to slipt that memmory down to 128 to fit 128 chunk I need.

```c
Metadata code for Heap manageent

struct block{
 size_t size; //the size of the block
 int free; //This flag is used to know whether the block described by the metadata structure. It is set to 1 else 0
 struct block *next; //Pointer to the next metadata

};

//Pointing to the main block of memory which is initially free no storage allocation yet

struct block* freeList=(struct block*)mem;

void init();
void frag(struct block *slot,size_t size);
void *pm_malloc(size_t noOfBytes);
void mergefrag();
void pm_free(void* ptr);
```

```c
void *a = malloc(256);  // 256 == 0x 100 bytes
void *b = malloc(256);
void *c = malloc(128);
void *d = malloc(256);
void *e = malloc(128);
/* Line 6 */
free(a);
free(b);
free(d);
/* Line 10 */
```

![hepa1](https://user-images.githubusercontent.com/77821039/220267199-e93ea146-93c7-49ed-bd0c-faebc71e2348.PNG)

A diagram showing a, b and d being freed.

![heap2](https://user-images.githubusercontent.com/77821039/220267276-5eb6e920-ea7a-4483-b18a-0357391761fa.PNG)



```c
#include <stdlib.h>

int main() {
  void *a = malloc(256);  // 256 == 0x 100 bytes
  void *b = malloc(256);
  void *c = malloc(128);
  void *d = malloc(256);
  void *e = malloc(128);
  /* Line 6 */
  free(a);
  free(b);
  free(d);
  /* Line 10 */
  void *r1 = malloc(10);
  void *r2 = malloc(10);
  void *r3 = malloc(300);
  void *r4 = malloc(250);  
}
```

a and b can be merged together if I request to use a memory of 512 and memory a and b that was freed can occupy a chunk of memory of 128. if I required 128 chunk of memeory I can use my function frag to split a and b memory of 256 to 128 to fit exactly to what i requested and avoid wasting memory

![heeepa4](https://user-images.githubusercontent.com/77821039/220267352-6f031992-6073-4734-8f8d-33aea0ef0fb9.PNG)




resources:
Memory allocation. CS 240: Introduction to Computer Systems (Fall 2020). (n.d.). Retrieved February 21, 2023, from https://courses.engr.illinois.edu/cs240/fa2020/notes/buildingMalloc.html

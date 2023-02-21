Heap Management

What is heap management?

It represents the way SYSLINUX handles the dynamic memory allocation requests of modules through malloc() .

In my design where I would attach a metadata to the chunk of memory to be 
I would be using markers(free) to tell if the block is used or not.It is set to 1 when free and 0 when not free

Memory Heap Allocation Design

In the design you need to keep in mind the ability reuse the free memory when available. To optimize heap memeory we need to consider a few things like:

1.[Block Splitting]: When memory is re-used, it is very easy to simple mark the block is used to be 1 again even if the block is not a perfect fit. We should be able to split our memeory block to fit what memory is required.

2.[Free Lists]: we need to be able traverse both used and free blocks when walking through our memory.

3.[Memory Merging]: When memory is freed, two blocks of free memory may appear next to one-another we need to combine the blocks together to create one larger block. This solves external fragmentation

Below is an example of metadata attached to chunk of memory allocated to keep track of if the memory has been freed so it can be resused or merged or splited to fit exactly the requested memory i.e I reuest for 128 chunk of memeory and you have 500 you have to slipt that memmory down to 128 to fit 128 chunk I need.

```c
Metadata code Heap managment

struct block{
 size_t size; //the size of the block
 int free; //This flag is used to know whether the block described by the metadata structure.It is set to 1 when free and 0 when not free
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
void *a = pm_malloc(256);  // 256 == 0x 100 bytes
void *b = pm_malloc(256);
void *c = pm_malloc(128);
void *d = pm_malloc(256);
void *e = pm_malloc(128);
/* Line 6 */
pm_free(a);
pm_free(b);
pm_free(d);
/* Line 10 */
```
![stack](https://user-images.githubusercontent.com/77821039/220288365-8c110435-fcbd-4390-b3a9-17a26d50c78b.PNG)


A diagram showing a, b and d being freed.

![freeheap](https://user-images.githubusercontent.com/77821039/220288440-c346ec5f-b717-4bda-9a6b-d07b211a583c.PNG)




```c
#include <stdlib.h>

int main() {
  void *a = pm_malloc(256);  // 256 == 0x 100 bytes
  void *b = pm_malloc(256);
  void *c = pm_malloc(128);
  void *d = pm_malloc(256);
  void *e = pm_malloc(128);
  /* Line 6 */
  pm_free(a);
  pm_free(b);
  pm_free(d);
  /* Line 10 */
  void *r1 = pm_malloc(10);
  void *r2 = pm_malloc(10);
  void *r3 = pm_malloc(300);
  void *r4 = pm_malloc(250);  
}
```

a and b can be merged together if I request to use a memory of 512 and memory a and b that was freed can occupy a chunk of memory of 128. if I required 128 chunk of memeory I can use my function frag to split a and b memory of 256 to 128 to fit exactly to what i requested and avoid wasting memory

![startstack](https://user-images.githubusercontent.com/77821039/220288592-20f83483-6c07-4d79-a48b-9f35883e36cd.PNG)

The final allocation
```c
Inside: malloc(250):
-- Start of Heap (0x56009a9ec000) --
metadata for memory 0x56009a9ec008: (0x56009a9ec000, size=256, free=1)
metadata for memory 0x56009a9ec110: (0x56009a9ec108, size=256, free=1)
metadata for memory 0x56009a9ec218: (0x56009a9ec210, size=128, free=0)
metadata for memory 0x56009a9ec2a0: (0x56009a9ec298, size=256, free=1)
metadata for memory 0x56009a9ec3a8: (0x56009a9ec3a0, size=128, free=0)
metadata for memory 0x56009a9ec430: (0x56009a9ec428, size=10, free=0)
metadata for memory 0x56009a9ec442: (0x56009a9ec43a, size=10, free=0)
metadata for memory 0x56009a9ec454: (0x56009a9ec44c, size=300, free=0)
-- End of Heap (0x56009a9ec580) --
```






resources:
Memory allocation. CS 240: Introduction to Computer Systems (Fall 2020). (n.d.). Retrieved February 21, 2023, from https://courses.engr.illinois.edu/cs240/fa2020/notes/buildingMalloc.html

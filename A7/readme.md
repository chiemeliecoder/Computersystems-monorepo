Heap Management

I would use a paged design where I would attach a metadata to the chunck of memeory to be 
I would be using markers to tell if the block is used or not.If the block is free; 1 if the block is used.

Memory Heap Allocation Design

In the design you need to keep in mind the ability reuse the free memory when available. To optimize heap memeory we need to consider a few things like:
1.[Block Splitting]: When memory is re-used, it is very easy to simple mark the block isUsed to be 1 again even if the block is not a perfect fit. We should be able to split our memeory block to fit what memory is required.
2.[Free Lists]: we need to be able traverse both used and free blocks when walking through our memory.

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



![heap2](https://user-images.githubusercontent.com/77821039/220267276-5eb6e920-ea7a-4483-b18a-0357391761fa.PNG)



```y
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



![heeepa4](https://user-images.githubusercontent.com/77821039/220267352-6f031992-6073-4734-8f8d-33aea0ef0fb9.PNG)




resources:
Memory allocation. CS 240: Introduction to Computer Systems (Fall 2020). (n.d.). Retrieved February 21, 2023, from https://courses.engr.illinois.edu/cs240/fa2020/notes/buildingMalloc.html

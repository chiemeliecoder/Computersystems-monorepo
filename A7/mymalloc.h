#include<stdio.h>
#include<stddef.h>
#ifndef MYMALLOC_H
#define MYMALLOC_H
#endif

#define HER 20000

char mem[HER];

struct block{
 //the size of the block
 size_t size;
 //This flag is used to know whether the block described by the metadata structure. It is set to 1 else 0
 int free;
 //Pointer to the next metadata
 struct block *next; 

};

//Pointing to the main block of memory which is initially free no storage allocation yet

struct block* freeList=(struct block*)mem;

//Initializing the block of memory
void initialize();
void split(struct block *fitting_slot,size_t size);
void *MyMalloc(size_t noOfBytes);
void merge();
void MyFree(void* ptr);

#include<stdio.h>
#include<stddef.h>
#ifndef MYMALLOC_H
#define MYMALLOC_H
#endif

#define PAGE_SIZE 4096

char mem[PAGE_SIZE];

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
void *pm_malloc_lock(size_t noOfBytes);
void mergefrag();
void pm_free(void* ptr);
void pm_free_lock(void* ptr);

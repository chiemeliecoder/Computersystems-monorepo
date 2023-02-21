#include<stdio.h>
#include<stddef.h>
#ifndef MYMALLOC_H
#define MYMALLOC_H
#endif

#define HER 20000

char mem[HER];

struct block{
 size_t size;
 int free;
 struct block *next; 

};

struct block* freeList=(struct block*)mem;

void initialize();
void split(struct block *fitting_slot,size_t size);
void *MyMalloc(size_t noOfBytes);
void merge();
void MyFree(void* ptr);

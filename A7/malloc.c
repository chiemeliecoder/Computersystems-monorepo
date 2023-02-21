#include<stdio.h>
#include<stddef.h>
#include "mymalloc.h"

//Initializing the block of memory
void initialize(){
 freeList->size=20000-sizeof(struct block);
 //this a flag if free, it is set to 1 else 0
 freeList->free=1;
 freeList->next=NULL;
}
//to make sure that the memeory allocated fits properly and that the space for the requested allocated memory is not bigger or smaller but exact.
// The arguments accepts a pointer to the metadata block whose size is more than required.A new metadata block pointer is created it points to  the location provided by passing a block of memory.

void split(struct block *fitting_slot,size_t size){
 struct block *new=(void*)((void*)fitting_slot+size+sizeof(struct block));
 new->size=(fitting_slot->size)-size-sizeof(struct block);
 new->free=1;
 new->next=fitting_slot->next;
 fitting_slot->size=size;
 fitting_slot->free=0;
 fitting_slot->next=new;
}

//If this condition below is met, the memory exactly fits the required size. set free flag to 0 showing the memory is allocated and return the starting address of the block of memory allocated.
//If this condition is met. The memory size is greater than what is required. call the frag function to cut the memory to make it fit to the required sizeand return the starting address of the block of memory allocated.


void *MyMalloc(size_t noOfBytes){
 struct block *curr,*prev;
 void *result;
 if(!(freeList->size)){
  initialize();
  printf("Memory initialized\n");
 }
 curr=freeList;
 while((((curr->size)<noOfBytes)||((curr->free)==0))&&(curr->next!=NULL)){
  prev=curr;
  curr=curr->next;
  printf("One block checked\n");
 }
 if((curr->size)==noOfBytes){
  curr->free=0;
  result=(void*)(++curr);
  printf("Exact fitting block allocated\n");
  return result;
 }
 else if((curr->size)>(noOfBytes+sizeof(struct block))){
  split(curr,noOfBytes);
  result=(void*)(++curr);
  printf("Fitting block allocated with a split\n");
  return result;
 }
 else{
  //no sufficient memory to allocate and return a null pointer.
  result=NULL;
  printf("Sorry. No sufficient memory to allocate\n");
  return result;
 }
}

//function below helps solve external fragmentation by joining the consecutive free blocks by removing the metablocks lying in between
void merge(){
 struct block *curr,*prev;
 curr=freeList;
 while((curr->next)!=NULL){
  if((curr->free) && (curr->next->free)){
   curr->size+=(curr->next->size)+sizeof(struct block);
   curr->next=curr->next->next;
  }
  prev=curr;
  curr=curr->next;
 }
}

//deallocate memory
void MyFree(void* ptr){
 if(((void*)mem<=ptr)&&(ptr<=(void*)(mem+20000))){
  struct block* curr=ptr;
  --curr;
  curr->free=1;
  merge();
 }
 else printf("Please provide a valid pointer allocated by MyMalloc\n");
}

#include<stdio.h>
#include<stddef.h>
#include <pthread.h>
#include "mymalloc.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//Initializing the block of memory
void init(){
 freeList->size=PAGE_SIZE-sizeof(struct block);
 freeList->free=1;//this a flag c
 freeList->next=NULL;
}

//to make sure that the memeory allocated fits properly and that the space for the requested allocated memory is not bigger or smaller but exact.


void frag(struct block *slot,size_t size){
  // The arguments accepts a pointer to the metadata block whose size is more than required.A new metadata block pointer is created it points to  the location provided by passing a block of memory.
 struct block *new=(void*)((void*)slot+size+sizeof(struct block));
 new->size=(slot->size)-size-sizeof(struct block);
 new->free=1;
 new->next=slot->next;
 slot->size=size;
 slot->free=0;
 slot->next=new;
}


//Thread-safe malloc with locks 

void *pm_malloc_lock(size_t noOfBytes){
  //pointers to traverse through the freeList
 struct block *current,*prev;
  //the starting address of the allocated memory
 void *res;
 pthread_mutex_lock(&mutex);
 if(!(freeList->size)){
  init();
  pthread_mutex_unlock(&mutex);
  printf("Memory initialized\n");
 }
  //temporary pointer that points to the start of the metablock
 current=freeList;
  //If this condition below is met, the metablock cannot be used for the allocation
 while((((current->size)<noOfBytes)||((current->free)==0))&&(current->next!=NULL)){
  prev=current;
  current=current->next;
  printf("One block checked\n");
 }
  //If this condition below is met, the memory exactly fits the required size. set free flag to 0 showing the memory is allocated and return the starting address of the block of memory allocated.
 pthread_mutex_unlock(&mutex);
 if((current->size)==noOfBytes){
  current->free=0;
  res=(void*)(++current);
  printf("Exact fitting block allocated\n");
  return res;
 }
 else if((current->size)>(noOfBytes+sizeof(struct block))){
   //If this condition is met. The memory size is greater than what is required. call the frag function to cut the memory to make it fit to the required sizeand return the starting address of the block of memory allocated.
  frag(current,noOfBytes);
  res=(void*)(++current);
  printf("Fitting block allocated with a split\n");
  return res;
 }
 else{
   //no sufficient memory to allocate and return a null pointer.
  res=NULL;
  printf("Sorry. No sufficient memory to allocate\n");
  pthread_mutex_unlock(&mutex);
  return res;
 }
}


void *pm_malloc(size_t noOfBytes){
  //pointers to traverse through the freeList
 struct block *current,*prev;
  //the starting address of the allocated memory
 void *res;
 if(!(freeList->size)){
  init();
  printf("Memory initialized\n");
 }
  //temporary pointer that points to the start of the metablock
 current=freeList;
  //If this condition below is met, the metablock cannot be used for the allocation
 while((((current->size)<noOfBytes)||((current->free)==0))&&(current->next!=NULL)){
  prev=current;
  current=current->next;
  printf("One block checked\n");
 }
  //If this condition below is met, the memory exactly fits the required size. set free flag to 0 showing the memory is allocated and return the starting address of the block of memory allocated.
 if((current->size)==noOfBytes){
  current->free=0;
  res=(void*)(++current);
  printf("Exact fitting block allocated\n");
  return res;
 }
 else if((current->size)>(noOfBytes+sizeof(struct block))){
   //If this condition is met. The memory size is greater than what is required. call the frag function to cut the memory to make it fit to the required sizeand return the starting address of the block of memory allocated.
  frag(current,noOfBytes);
  res=(void*)(++current);
  printf("Fitting block allocated with a split\n");
  return res;
 }
 else{
   //no sufficient memory to allocate and return a null pointer.
  res=NULL;
  printf("Sorry. No sufficient memory to allocate\n");
  return res;
 }
}

//function below helps solve external fragmentation by joining the consecutive free blocks by removing the metablocks lying in between

void mergefrag(){
  //pointers to traverse through the freeList
 struct block *curr,*prev;
  //temporary pointer that points to the start of the metablock
 curr=freeList;
  //The current next isnt null
 while((curr->next)!=NULL){
   //the current and current next is free
  if((curr->free) && (curr->next->free)){
    //current size
   curr->size+=(curr->next->size)+sizeof(struct block);
    //current next is the next next
   curr->next=curr->next->next;
  }
    //preivous becomes current
  prev=curr;
    //current is current next 
  curr=curr->next;
 }
}

//deallocate memory with locks
void pm_free_lock(void* ptr){
  pthread_mutex_lock(&mutex);
  if(((void*)mem<=ptr)&&(ptr<=(void*)(mem+PAGE_SIZE))){
    struct block* curr=ptr;
    --curr;
    curr->free=1;
    mergefrag();
    pthread_mutex_unlock(&mutex);
  }else{
    pthread_mutex_unlock(&mutex);
    printf("Please provide a valid pointer allocated by MyMalloc\n");}
}

//deallocate memory no locks

void pm_free(void* ptr){
  if(((void*)mem<=ptr)&&(ptr<=(void*)(mem+PAGE_SIZE))){
    struct block* curr=ptr;
    --curr;
    curr->free=1;
    mergefrag();
  }else{
    printf("Please provide a valid pointer allocated by MyMalloc\n");}
}

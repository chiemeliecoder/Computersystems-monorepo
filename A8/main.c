#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mcheck.h>
#include <assert.h>
#include <pthread.h>
#include "malloc.h"
#include "threadmalloc.h"



struct block* heap;

/**created a thread function for unittest 8  to create a random sizes to malloc then creating a void pointer to malloc the random size and also to check if the pointer is null and then free the pointer and return null I used this function to create concurrency between the two functions MALLOC and FREE trying to do both in the same function.**/

void* thread_func() {
  size_t size = rand() % 100 + 1;
  void* ptr = MALLOC(size);
  if (ptr == NULL) {
      fprintf(stderr, "Error: failed to allocate memory\n");
      exit(EXIT_FAILURE);
  }
  
  FREE(ptr);
  return NULL;
}

//This unit test will create a segmentation fault or exit 1 due to no memeory allocation

void unitTest8(){
   //normal allocation and deallocation where it checks the memory to see if there is any chunk of memory that would for what I requested for exactly. Memory is initialized then split to fit 
 
  int *p= (int*)thread_func((int*)256);
  //char *q= (char*)thread_func((char*)256);
  //int *r= (int)thread_func(128*sizeof(int));
  
  pm_free_lock(p);
  // char *w= (char*)thread_func((char*)2);
  
  //pm_free_lock(q);
  int *k= (int)thread_func((int*)10);
  printf("Allocation and deallocation is done successfully for unitTest thread function!\n");
  
}


void unitTest1(){
   //normal allocation and deallocation where it checks the memory to see if there is any chunk of memory that would for what I requested for exactly. Memory is initialized then split to fit 
  int *p= (int)MALLOC(100*sizeof(int));
  char *q= (char)MALLOC(250*sizeof(char));
  int *r= (int)MALLOC(1000*sizeof(int));
  
  FREE(p);
  char *w= (char)MALLOC(700);
  
  FREE(r);
  int *k= (int)MALLOC(500*sizeof(int));
  printf("Allocation and deallocation is done successfully for unitTest 1!\n");
  
}



int main(){

  
  

  printf("before threading and concurrency\n");
 
  
  pthread_barrier_init(&barry, NULL, NUM_THREADS);
  //creating first thread
  for (int i=0; i < NUM_THREADS; i++) {
    printf("=======================unitTest%d=====================\n",i);
    thread_id[i] = i;
    pthread_create(&threads[i], NULL, unitTest1, (void *)(&thread_id[i]));
  
  }

  printf("=======================================================================\n");
  printf("                      First threading set is done                      \n");
  printf("=======================================================================\n");


  //this thread would run with thread 1 but the issue is that due to no memory it could come up with an exit 1 or segmentation fault due to no memeory management
  //creating second thread
  for(int i = 0; i < NUM_THREADS; i++){
    printf("=======================unitTestforthreadfunc%d=====================\n",i);
    thread_id[i] = i;
    
    if((pthread_create(&threads2[i], NULL, unitTest8, (void *)(&thread_id[i])))!= NULL){
      fprintf(stderr, "Error: failed to create thread\n");
      exit(EXIT_SUCCESS);
    }
  }

  
  //join for first thread

  for (int i=0; i < NUM_THREADS; i++) {
    if((pthread_join(threads[i], NULL) != 0)){
      fprintf(stderr, "Error: failed to join thread\n");
      exit(EXIT_FAILURE);
    }
    
  }

 

  //join for second thread

  for (int i=0; i < NUM_THREADS; i++) {
    if((pthread_join(threads2[i], NULL) != 0)){
      fprintf(stderr, "Error: failed to join thread\n");
      exit(EXIT_FAILURE);
    }
    
  }

  //a check that forms of heap corruption are handled

  size_t size = 12;
  void *mems = MALLOC(size);
  printf("Successfully malloc'd %zu bytes at addr %p\n", size, mems);
  assert(mems != NULL);
  FREE(mems);
  printf("Successfully free'd %zu bytes from addr %p\n", size, mems);

  
  printf("after concurrency has occured\n");
  

  
  pthread_exit(NULL);

  
  return 0;


 
}

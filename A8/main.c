#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "malloc.h"
#include "threadmalloc.h"



struct block* heap;

void* thread_func(void* arg) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        size_t size = rand() % 100 + 1;
        void* ptr = MALLOC(size);
        if (ptr == NULL) {
            fprintf(stderr, "Error: failed to allocate memory\n");
            exit(EXIT_FAILURE);
        }
        FREE(ptr);
    }
    return NULL;
}


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
  
  for (int i=0; i < NUM_THREADS; i++) {
    printf("=======================unitTest%d=====================\n",i);
    thread_id[i] = i;
    pthread_create(&threads[i], NULL, unitTest1, (void *)(&thread_id[i]));
  
  }

  printf("=======================================================================\n");
  printf("                      First threading set is done                      \n");
  printf("=======================================================================\n");


  for(int i = 0; i < NUM_THREADS; i++){
    printf("=======================unitTestforthreadfunc%d=====================\n",i);
    thread_id[i] = i;
    
    if((pthread_create(&threads2[i], NULL, unitTest8, (void *)(&thread_id[i])))!= NULL){
      fprintf(stderr, "Error: failed to create thread\n");
      exit(EXIT_SUCCESS);
    }
  }

  

  for (int i=0; i < NUM_THREADS; i++) {
    if((pthread_join(threads[i], NULL) != 0)){
      fprintf(stderr, "Error: failed to join thread\n");
      exit(EXIT_FAILURE);
    }
    
  }

 

  

  for (int i=0; i < NUM_THREADS; i++) {
    if((pthread_join(threads2[i], NULL) != 0)){
      fprintf(stderr, "Error: failed to join thread\n");
      exit(EXIT_FAILURE);
    }
    
  }
  
  
  printf("after concurrency has occured\n");

  
  pthread_exit(NULL);
  return 0;


 
}
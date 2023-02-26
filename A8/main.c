#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "malloc.h"


// void unitTest2(){
//   //normal allocation and deallocation 
//   int *p= (int)pm_malloc(100*sizeof(int));
//   pm_free(p);
//   char *q= (char)pm_malloc(250*sizeof(char));
//   printf("Allocation and deallocation is done successfully for unitTest 2!\n");
  
// }

// void unitTest3(){
//   //normal allocation and deallocation where it checks the memory to see if there is any chunk of memory that would for what I requested for exactly if not found would split it at a point it would not be able to free and would spit out Please provide a valid pointer allocated by MyMalloc when freeing
//   int *p= (int)pm_malloc(100*sizeof(int));
//   pm_free(p);
//   char *q= (char)pm_malloc(250*sizeof(char));
//   pm_free(q);
//   printf("Allocation and deallocation is done successfully for unitTest 3!\n");

  
// }
// void unitTest4(){
//   //normal allocation and deallocation where it checks the memory to see if there is any chunk of memory that would for what I requested for exactly if not found would split it at a point it would not be able to free and would spit out Please provide a valid pointer allocated by MyMalloc when freeing q
//   int *p= (int)pm_malloc(100*sizeof(int));
//    pm_free(p);
//   char *q= (char)pm_malloc(250*sizeof(char));
//   int *r= (int)pm_malloc(1000*sizeof(int));
//   pm_free(q);
//   char *w= (char)pm_malloc(700);
//   int *k= (int)pm_malloc(500*sizeof(int));
//   printf("Allocation and deallocation is done successfully for unitTest 4!\n");
  
// }
// void unitTest5(){
//   //normal allocation and deallocation where it checks the memory to see if there is any chunk of memory that would for what I requested for exactly if not found would split it 
//   int *p= (int)pm_malloc(100*sizeof(int));
//    pm_free(p);
  
//   printf("Allocation and deallocation is done successfully for unitTest 5!\n");
  
// }

// void unitTest6(){
//    //normal allocation and deallocation where it checks the memory to see if there is any chunk of memory that would for what I requested for exactly. Memory is initialized then split to fit and would trigger no sufficient memory to allocate and return a null pointer.uncomment the code to run it,
//   // int *a= (int)pm_malloc(100*sizeof(int));
//   // //char *b= (char)pm_malloc(250*sizeof(char));
//   // int *c= (int)pm_malloc(100*sizeof(int));
  
//   // pm_free(a);
//   // char *d= (char)pm_malloc(700);
  
//   // pm_free(c);
//   // int *e= (int)pm_malloc(500*sizeof(int));
//   printf("Allocation and deallocation is done successfully for unitTest 6!\n");

  
  
// }
#define NUM_THREADS 4
#define NUM_ITEMS 5

pthread_t threads[NUM_THREADS];
int  thread_id[NUM_THREADS];

pthread_barrier_t barry;

struct malloc_list {
  size_t bytes;
  int address;
  int free;
};


struct malloc_list* malloc_items[NUM_THREADS * NUM_ITEMS];

void* lets_allocate(int thread_id){
  int i;
  int index;
  int counter = 0;

  int threadstartingid = thread_id * NUM_ITEMS;

  pthread_barrier_wait(&barry);

  for(i = 0; i < NUM_ITEMS; i++){
    index = i + threadstartingid;
    malloc_items[index]->address = (int)pm_malloc(malloc_items[index]->bytes);
    malloc_items[index]->free = 0;

    if((i % 10) == 0){
      pm_free(malloc_items[index]->address);
      malloc_items[index]->free = 1;
      counter++;
      
    }
    
  }

  pthread_barrier_wait(&barry);
  
}

void *allocate(void *arg){
  int id = *((int *) arg);
  lets_allocate(id);
  return NULL;
  
}


void unitTest1(){
   //normal allocation and deallocation where it checks the memory to see if there is any chunk of memory that would for what I requested for exactly. Memory is initialized then split to fit 
  int *p= (int)pm_malloc(100*sizeof(int));
  char *q= (char)pm_malloc(250*sizeof(char));
  int *r= (int)pm_malloc(1000*sizeof(int));
  
  pm_free(p);
  char *w= (char)pm_malloc(700);
  
  pm_free(r);
  int *k= (int)pm_malloc(500*sizeof(int));
  printf("Allocation and deallocation is done successfully for unitTest 1!\n");
  
}

int main(){

  int i,j;
  int start, end, tgt_start, tgt_end;
  int fail = 0;
  
  printf("before\n");
 //  printf("=======================unitTest1=====================\n");
	// unitTest1();
  pthread_barrier_init(&barry, NULL, NUM_THREADS);
  for (i=0; i < NUM_THREADS; i++) {
    thread_id[i] = i;
    pthread_create(&threads[i], NULL, allocate, (void *)(&thread_id[i]));
  }

  
  printf("after\n");

  for (i=0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
    printf("size=%zdB\n", malloc_items[i]->bytes);
  }

  
  // for (i=0; i < NUM_THREADS * NUM_ITEMS; i++) {
  //   if (malloc_items[i]->free == 1) continue;
  //   start = malloc_items[i]->address;
  //   end   = start + (malloc_items[i]->bytes / sizeof(int));

  //   for (j=0; j < NUM_THREADS * NUM_ITEMS; j++) {
  //     if (malloc_items[j]->free == 1) continue;
  //     if (i == j) continue;
  //     tgt_start = malloc_items[j]->address;
  //     tgt_end   = tgt_start + (malloc_items[j]->bytes / sizeof(int));
  //     if (((start >= tgt_start) && (start <= tgt_end)) || ((end >= tgt_start) && (end <= tgt_end))) {
  //       fail = 1;
  //       break;
  //     }
  //   }

  //   if (fail == 1) break;
  // }

  if (fail == 0) {
    printf("No overlapping allocated regions found!\n");
    printf("Test passed\n");
  } else {
    printf("Found 2 overlapping allocated regions.\n");
    printf("Region 1 bounds: start=%d, end=%d, size=%zdB, idx=%d\n", start, end, malloc_items[i]->bytes, i);
    printf("Region 2 bounds: start=%d, end=%d, size=%zdB, idx=%d\n", tgt_start, tgt_end, malloc_items[j]->bytes, j);
    printf("Test failed\n");
  }

  // for (i=0; i < NUM_THREADS * NUM_ITEMS; i++) {
  //   pm_free(malloc_items[i]->address);
  // } 
  pthread_exit(NULL);
  return 0;
 //  printf("=======================unitTest1=====================\n");
	// unitTest1();
 //  printf("=======================unitTest2=====================\n");
	// unitTest2();
 //  printf("=======================unitTest3=====================\n");
	// unitTest3();
 //  printf("=======================unitTest4=====================\n");
	// unitTest4();
 //   printf("=======================unitTest5=====================\n");
	// unitTest5();
 //  printf("=======================unitTest6=====================\n");
	// unitTest6();
  //for i

  

  // pthread_t tid,tid2,tid3,tid4,tid5;
  // void* point;
  // printf("before\n");
  // pthread_create(&tid, NULL, unitTest1,(void *)&tid);
  // pthread_create(&tid2, NULL, unitTest2,(void *)&tid2);
  // pthread_create(&tid3, NULL, unitTest3,(void *)&tid3);
  // pthread_create(&tid4, NULL, unitTest4,(void *)&tid4);
  // pthread_create(&tid5, NULL, unitTest5,(void *)&tid5);
  // pthread_join(&tid, NULL);
  // printf("after\n");
  // pthread_join(&tid2, &point);
  // pthread_join(&tid3, NULL);
  // pthread_join(&tid5, NULL);
  // pthread_join(&tid4, NULL);
  // printf("after\n");
 
}

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "malloc.h"
#ifndef THREADMALLOC_H
#define THREADMALLOC_H
#endif


#define MALLOC(sz) pm_malloc_lock(sz)
#define FREE(p)    pm_free_lock(p)



#define NUM_THREADS 4
#define NUM_ITEMS 100

pthread_t threads[NUM_THREADS];
pthread_t threads2[NUM_THREADS];
int  thread_id[NUM_THREADS];

pthread_barrier_t barry;

struct malloc_list {
  size_t bytes;
  int address;
  int free;
};


struct malloc_list* malloc_items[NUM_THREADS * NUM_ITEMS];

void* lets_allocate(int thread_id);
void *allocate(void *arg);

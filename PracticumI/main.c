#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <stdint.h>

#include <mcheck.h>

#include <assert.h>

#include <pthread.h>

#include <stdbool.h>

#include <fcntl.h>

#include "mymalloc.h"

#include <sys/mman.h>

struct block* heap;
PageTable page_table;

int unitTest1(){
  void *mem = pm_malloc_lock(PAGE_SIZE); 
  if (mem == NULL) { 
    printf("Failed to allocate memory\n"); 
    return 1; 
  } 
  char *c = (char *)mem; 
  *c = 'A'; 
  pm_free_lock(c); 
  return 0;
}

void unitTest2(){
  size_t size = NUM_PAGES * PAGE_SIZE * 2; // Allocate more memory than the physical memory size 

  

  void *ptr = pm_malloc_lock(size); 
  
  if (ptr == NULL) { 
  
    printf("Failed to allocate memory.\n"); 
  
    return; 
  
  } 

  

  // Write data to allocated memory 

  memset(ptr, 'A', size); 



  // Read back written data 

  char *p = (char *)ptr; 

  for (size_t i = 0; i < size; i++) { 

    if (*p != 'A') { 

      printf("Failed to write data to memory.\n"); 

      return; 

    } 

    p++; 

  } 



  // Verify that pages have been swapped out to virtual memory 

  for (int i = 0; i < NUM_PAGES; i++) { 

    if ((pageing[i].status & USEDBIT) != USEDBIT || (pageing[i].status & DIRTYBIT) != DIRTYBIT) { 

      printf("Page %d not swapped out to virtual memory.\n", i); 

      return; 

    } 

  } 



  printf("Test passed.\n"); 
}

// void unitTest1(){
  
//    //normal allocation and deallocation where it checks the memory to see if there is any chunk of memory that would for what I requested for exactly. Memory is initialized then split to fit 
//   int *p= (int)pm_malloc_lock(100*sizeof(int));
//   char *q= (char)pm_malloc_lock(250*sizeof(char));
//   int *r= (int)pm_malloc_lock(1000*sizeof(int));
  
//   pm_free_lock(p);
//   char *w= (char)pm_malloc_lock(700);
  
//   pm_free_lock(r);
//   int *k= (int)pm_malloc_lock(500*sizeof(int));
//   printf("Allocation and deallocation is done successfully for unitTest 1!\n");
  
// }

// void unitTest2(){
//   PageTable page_table;
//   init_page_table(&page_table, NUM_PAGES);
//   load_page(&page_table, 1, 1); // load page 1 into memory at time 1 
//   load_page(&page_table, 2, 2); // load page 2 into memory at time 2 
// }


void unitTest3(){
  init_page_table(&page_table, NUM_PAGES);
  init();
  char data[] = "Hello, world!";
  write_page(1,data);
  char data2[] = "King Gnu";
  write_page(2, data2);

  read_page(1, physical_memory);
  read_page(2, physical_memory);
   // Allocate and write to heap memory
  char *p = (char*)pm_malloc_lock(16);
  strcpy(p, "Hello, heap!");
  printf("Heap memory contains: %s\n", p); 
  //loading
  load_page(&page_table, 1, 1); // load page 1 into memory at time 1 
  load_page(&page_table, 2, 2); // load page 2 into memory at time 2 
  // Set page 0 as not used and dirty 
  set_page_used(0, false); 
  set_page_dirty(0, false); 
   // Set page 1 and 2 as used and dirty 

  set_page_used(1, true); 
  set_page_dirty(1, true); 
  set_page_used(2, true); 
  set_page_dirty(2, true); 
  
  // Check status of pages 

  if (is_page_used(0)) {
    printf("Page 0 is used\n"); 
  }else if(is_page_used(1)){
    printf("Page 1 is used\n"); 
  }else if(is_page_used(2)){
    printf("Page 2 is used\n"); 
  }

  if(is_page_dirty(0)) {
    printf("Page 0 is dirty\n"); 
  }else if(is_page_dirty(1)){
    printf("Page 1 is dirty\n"); 
  }else if(is_page_dirty(2)){
    printf("Page 2 is dirty\n"); 
  }

  
  //free memory
  pm_free_lock(physical_memory);
  close(swap_fd);
  unlink(SWAP_FILE); 
}

void unitTest4(){
  // Allocate memory 

  void *ptr = pm_malloc_lock(10); 


  // Check if memory allocation is successful 

  if (ptr == NULL) { 

    printf("Failed to allocate memory\n"); 

    exit(1); 

  } 

  // Write some data to the allocated memory 

  char *str = "Hello"; 

  memcpy(ptr, str, strlen(str) + 1); 

  // Free the allocated memory 

  pm_free_lock(ptr); 

  // Try to read from the freed memory location 

  char c = *((char*)ptr); // This should result in a segmentation fault error 
}

void simulate_page_access(int num_pages, int* page_refs, int page_size, char* pm_mem) {
    for (int i = 0; i < num_pages; i++) {
        int page_num = page_refs[i];
        char* page_addr = pm_mem + page_num * page_size;
        *page_addr = 'a';  // Write to page to simulate access
    }
}


void unitTest5() { 
  

  // Allocate more memory than the size of the physical memory 

  void *virtual_memory = mmap(NULL, PAGE_SIZE * NUM_PAGES * 2, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 

  if (virtual_memory == MAP_FAILED) { 

      perror("Failed to allocate virtual memory with mmap"); 

      exit(1); 

  } 



  // Access different pages to verify that the least recently used page is swapped to virtual memory 

  char *p1 = (char *)(virtual_memory + PAGE_SIZE); 

  char *p2 = (char *)(virtual_memory + PAGE_SIZE * 2); 

  char *p3 = (char *)(virtual_memory + PAGE_SIZE * 3); 

  char *p4 = (char *)(virtual_memory + PAGE_SIZE * 4); 



  *p1 = 'a'; 

  *p2 = 'b'; 

  *p3 = 'c'; 

  *p4 = 'd'; 



  // Allocate memory and simulate memory usage 
  
  void *pm_mem = pm_malloc_lock(PAGE_SIZE * NUM_PAGES); if (pm_mem == NULL) { perror("Failed to allocate physical memory with pm_malloc_lock"); exit(1); } 
  
   
  // Free physical memory 
  pm_free_lock(pm_mem); 
   
  // Unmap virtual memory 
  if (munmap(virtual_memory, PAGE_SIZE * NUM_PAGES * 2) == -1) { 
      perror("Failed to unmap virtual memory with munmap"); 
      exit(1); 
  } 
 

} 

void *thread_func(void *arg) { 
  

  int i; 

  void *mem[LOOP_COUNT]; 

  

  for (i = 0; i < LOOP_COUNT; i++) { 

    size_t size = rand() % 1024 + 1; 

    mem[i] = pm_malloc_lock(size); 

    if (mem[i] == NULL) { 

      perror("pm_malloc_lock"); 

      exit(1); 

    } 

  } 

  

  for (i = 0; i < LOOP_COUNT; i++) { 

    pm_free_lock(mem[i]); 

  } 

  

  return NULL; 

} 



pthread_t threads[NUM_THREADS];
pthread_t threads2[NUM_THREADS];
int  thread_id[NUM_THREADS];

pthread_barrier_t barry;

int main() { 

  unitTest1();
  printf("************second test******************");
  unitTest2();
  
  printf("before threading and concurrency\n");
 
  
  pthread_barrier_init(&barry, NULL, NUM_THREADS);

  for (int i=0; i < NUM_THREADS; i++) {
    printf("=======================unitTest%d=====================\n",i);
    thread_id[i] = i;
    if(pthread_create(&threads[i], NULL, thread_func,NULL)!= 0){
      perror("pthread_create"); 

      exit(1); 
    }
  
  }
  printf("Hello World\n");

  //join for first thread

  for (int i=0; i < NUM_THREADS; i++) {
    if((pthread_join(threads[i], NULL) != 0)){
      fprintf(stderr, "Error: failed to join thread\n");
      exit(EXIT_FAILURE);
    }
    
  }

  pthread_exit(NULL);
  return 0;
}

#include "mymalloc.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void init() { 
  freeList->size=PAGE_SIZE-sizeof(struct block);
  freeList->free=1;//this a flag c
  freeList->next=NULL;

  // Initialize swap file
  swap_fd = open(SWAP_FILE, O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
  if (swap_fd == -1) {
      perror("Failed to open swap file");
      exit(1);
  }
  if (ftruncate(swap_fd, PAGE_SIZE * NUM_PAGES) == -1) {
      perror("Failed to resize swap file");
      exit(1);
  }
  memset(pages, 0, sizeof(pages));
}

void frag(void *slot, size_t size) { 
  
  // The arguments accepts a pointer to the metadata block whose size is more than required.A new metadata block pointer is created it points to  the location provided by passing a block of memory.
  struct block *new = (struct block *)((char *)slot + size + sizeof(struct block));
  new->size = ((struct block *)slot)->size - size - sizeof(struct block);
  new->free = 1;
  new->next = ((struct block *)slot)->next;
  ((struct block *)slot)->size = size;
  ((struct block *)slot)->free = 0;
  ((struct block *)slot)->next = new;

  if (new->size < sizeof(struct block)) {
    // The remaining memory is too small to contain another metadata block
    // Merge the remaining memory with the next block if it is free
    if (new->next && new->next->free) {
        new->size += new->next->size + sizeof(struct block);
        new->next = new->next->next;
    }
    return;
  }
}


void *pm_malloc_lock(size_t noOfBytes) { 
  printf("Inside: pm_malloc(%lu):\n", noOfBytes);
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
 prev = current;
  //If this condition below is met, the metablock cannot be used for the allocation
 while((((current->size)<noOfBytes)||((current->free)==0))&&(current->next!=NULL)){
  prev=current;
  current=current->next;
  printf("One block checked\n");
 }
  if (!current) {
    // No suitable block found, allocate more memory
    size_t size = (noOfBytes > PAGE_SIZE) ? noOfBytes : PAGE_SIZE;
    void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        perror("Failed to allocate memory with mmap");
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    // Add new block to free list
    struct block *new_block = (struct block *)phys_mem;
    new_block->size = size - sizeof(struct block);
    new_block->free = 1;
    new_block->next = NULL;
    prev->next = new_block;

    // Update current pointer
    current = new_block;
  }
  
  //If this condition below is met, the memory exactly fits the required size. set free flag to 0 showing the memory is allocated and return the starting address of the block of memory allocated.
 // pthread_mutex_unlock(&mutex);
 if((current->size)==noOfBytes){
  current->free=0;
  res=(void*)(++current);
  pthread_mutex_unlock(&mutex);
  printf("Exact fitting block allocated\n");
  return res;
 }
 else if((current->size)>(noOfBytes+sizeof(struct block))){
   //If this condition is met. The memory size is greater than what is required. call the frag function to cut the memory to make it fit to the required sizeand return the starting address of the block of memory allocated.
  frag(current,noOfBytes);
  res=(void*)(current+1);
  pthread_mutex_unlock(&mutex);
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



void mergefrag() {
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

void pm_free_lock(void *ptr) { 
  pthread_mutex_lock(&mutex);
  if(((void*)phys_mem<=ptr)&&(ptr<=(void*)(phys_mem+PAGE_SIZE))){
    struct block* curr=ptr;
    --curr;
    curr->free=1;
    mergefrag();
    pthread_mutex_unlock(&mutex);
  }else{
    pthread_mutex_unlock(&mutex);
    printf("Please provide a valid pointer allocated by MyMalloc\n");}
  pthread_mutex_unlock(&mutex);
}


void read_page(int page_num, char *buf) {
  if (page_num < 0 || page_num >= NUM_PAGES) {
    fprintf(stderr, "Invalid page number: %d\n", page_num); 
    exit(1); 

  }
  memcpy(buf, physical_memory + PAGE_SIZE * page_num, PAGE_SIZE);

}

void write_page(int page_num, char *data) {
  if (page_num < 0 || page_num >= NUM_PAGES) {
    fprintf(stderr, "Invalid page number: %d\n", page_num); 

    exit(1); 

  } 
  memcpy(physical_memory + PAGE_SIZE * page_num, data, PAGE_SIZE); 
}

void init_page_table(PageTable *page_table, int num_pages) {
  page_table->pages = (Page*) pm_malloc_lock(sizeof(Page) * num_pages);
  for (int i = 0; i < num_pages; i++) {
    page_table->pages[i].id = i; 
    page_table->pages[i].last_access_time = 0; 

  }
  page_table->num_pages = num_pages; 
}

int find_lru_page(PageTable *page_table) {
  int lru_page = 0; 

  for (int i = 1; i < page_table->num_pages; i++) {
    
    if (page_table->pages[i].last_access_time < page_table->pages[lru_page].last_access_time) {
      lru_page = i; 

    } 

  } 

  return lru_page; 
  printf("1");
}

void access_page(PageTable *page_table, int page_id, int current_time) {
  page_table->pages[page_id].last_access_time = current_time; 
}

void load_page(PageTable *page_table, int page_id, int current_time) {
  int free_space = -1; 

  for (int i = 0; i < page_table->num_pages; i++) {
    if (page_table->pages[i].id == page_id) {
      access_page(page_table, i, current_time);
      return; 

    }
    if (page_table->pages[i].last_access_time == 0) {
      free_space = i;
    } 

  } 

  if (free_space != -1) {
    page_table->pages[free_space].id = page_id;
    access_page(page_table, free_space, current_time); 

  }else{
    int lru_page = find_lru_page(page_table);
    page_table->pages[lru_page].id = page_id;
    access_page(page_table, lru_page, current_time); 

  } 
}

void *access_tracker(void *arg){
  access_tracker_t *tracker = (access_tracker_t *) arg;
  while (true) {
    // Sleep for 1 second to allow program to access pages 
    sleep(1); 

    // Check access status of each page 

    for (size_t i = 0; i < tracker->num_pages; i++) {
      if (tracker->pages[i]) {
        printf("Page %zu has been accessed\n", i); 

      }
      // Reset access status of page
      tracker->pages[i] = false; 

    } 

  } 

  return NULL;
}

// Function to get the status of a page
bool is_page_used(uint8_t page_num){
  return pages[page_num].status & USEDBIT; 
  printf("1");
}

bool is_page_dirty(uint8_t page_num){
  return pages[page_num].status & DIRTYBIT; 
  printf("1");
}

// Function to set the status of a page
void set_page_used(uint8_t page_num, bool used){
  if (used) {
    pages[page_num].status |= USEDBIT; 

  }else{
    pages[page_num].status &= ~USEDBIT; 

  } 
  //printf("1");
}

void set_page_dirty(uint8_t page_num, bool dirty){
  if (dirty) {
    pages[page_num].status |= DIRTYBIT; 

  }else{
    pages[page_num].status &= ~DIRTYBIT; 

  }
  
  // printf("1");
}

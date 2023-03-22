#include "mymalloc.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char phys_mem[PAGE_SIZE];

char *physical_memory = phys_mem;

struct block *freeList = (struct block *)phys_mem;

int swap_fd;

page_status_t pageing[NUM_PAGES];

void init() { 
  freeList->size=PAGE_SIZE-sizeof(struct block);
  freeList->free=1;//this a flag c
  freeList->next=NULL;

  PageTable p;
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
  memset(p.pages, 0, NUM_PAGES * sizeof(page_status_t));
  
  
}

void frag(void *slot, size_t size) { 
  
  // The function accepts a pointer to the metadata block whose size is more than required.A new metadata block pointer is created it points to  the location provided by passing a block of memory.
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

// Function to allocate a physical page and map it to a virtual page 
void *allocate_page(int page_num) { 
  // Allocate physical memory 
  void *page = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
  if (page == MAP_FAILED) { 
    return NULL; 
  } 
// Map virtual page to physical page 
  pageing[page_num].status = USEDBIT; 
  pageing[page_num].status |= DIRTYBIT; 
// Mark page as dirty to force swap-out on first access 
  if (mmap(page, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE, swap_fd, page_num * PAGE_SIZE) == MAP_FAILED) { 
    munmap(page, PAGE_SIZE); 
    return NULL; 
  } 
  return page; 
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
    void *res = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (res == MAP_FAILED) {
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

  for(int page_num = 0; page_num < NUM_PAGES; page_num++){ 

    if(pageing[page_num].status == USEDBIT){ 
      
      allocate_page(page_num); 

    } 

  } 
  //If this condition below is met, the memory exactly fits the required size. set free flag to 0 showing the memory is allocated and return the starting address of the block of memory allocated.
 // pthread_mutex_unlock(&mutex);
 if((current->size)==noOfBytes){
  current->free=0;
  res=(void*)(++current);
  pthread_mutex_unlock(&mutex);
  printf("Exact fitting block allocated\n");
  // Write to memory 

  memset(res, 'A', noOfBytes); 

  // Read from memory 

  printf("Data at allocated memory location: %s\n", (char*)res); 

     
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
  int lru_page_id = find_lru_page(page_table); 
  
  // Swap out the least recently used page  
  
  Page *lru_page = &page_table->pages[lru_page_id];  
   // Update the last access time of the page
  page_table->pages[page_id].last_access_time = current_time; 
   // Load the page into physical memory if it is not resident 
  load_page(page_table, page_id, current_time); 
  // Set the "used" bit for the page 
  set_page_used(page_id, true);
    // Check if there is enough free memory to load the page 
  int free_page_id = -1; 
  for (int i = 0; i < NUM_PAGES; i++) { 
    if (!is_page_used(i)) { 
      // Found a free page 
      free_page_id = i; 
      break; 
    } 
  } 

  if (free_page_id == -1) { perror("Failed to find a page to swap out"); exit(1); } 
   
  // Check if the least recently used page is dirty  
    
  if (is_page_dirty(lru_page_id)) {  
    
    // Write the page to the swap file 
  if (pwrite(swap_fd, physical_memory + (lru_page_id * PAGE_SIZE), PAGE_SIZE, page_table->pages[lru_page_id].disk_location) != PAGE_SIZE) {  
    
    perror("Failed to write page to swap file"); exit(1); } } // Mark the page as not resident in physical memory 
  page_table->pages[lru_page_id].resident = false;  
    
    // Load the new page into physical memory  
    
  char buf[PAGE_SIZE];  
  
  if (pread(swap_fd, buf, PAGE_SIZE, page_table->pages[page_id].disk_location) != PAGE_SIZE) { perror("Failed to read page from swap file"); exit(1); }  
  
  memcpy(physical_memory + (free_page_id * PAGE_SIZE), buf, PAGE_SIZE);  
    
    // Update the page table  
    
  lru_page->id = page_id; 
  
  lru_page->resident = true; 
  
  lru_page->disk_location = page_table->pages[page_id].disk_location;  
    
  page_table->pages[page_id].resident = true;  
  
  page_table->pages[page_id].id = free_page_id;  
  
  page_table->pages[page_id].last_access_time = current_time; 
    
  // Mark the swapped out page as unused  
    
  set_page_used(lru_page_id, false); 
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

  if (!page_table->pages[page_id].resident) { 
    // Page is not resident in physical memory 
    // Load it from swap file 
    char buf[PAGE_SIZE]; 
    if (pread(swap_fd, buf, PAGE_SIZE, page_table->pages[page_id].disk_location) != PAGE_SIZE) { 
      perror("Failed to read page from swap file"); 
      exit(1); 
    } 
    // Copy the page data to physical memory 
    memcpy(physical_memory + (page_id * PAGE_SIZE), buf, PAGE_SIZE); 
    // Update the page table 
    page_table->pages[page_id].resident = true; 
  } 
  // Update the last access time of the page 
  page_table->pages[page_id].last_access_time = current_time; 

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
  return pageing[page_num].status & USEDBIT; 
  printf("1");
}

bool is_page_dirty(uint8_t page_num){
  return pageing[page_num].status & DIRTYBIT; 
  printf("1");
}

// Function to set the status of a page
void set_page_used(uint8_t page_num, bool used){
  if (used) {
    pageing[page_num].status |= USEDBIT; 

  }else{
    pageing[page_num].status &= ~USEDBIT; 

  } 
  //printf("1");
}

void set_page_dirty(uint8_t page_num, bool dirty){
  if (dirty) {
    pageing[page_num].status |= DIRTYBIT; 

  }else{
    pageing[page_num].status &= ~DIRTYBIT; 

  }
  
  // printf("1");
}

bool is_page_in_memory(PageTable *page_table, int page_id) { 

  for (int i = 0; i < page_table->num_pages; i++) { 

    if (page_table->pages[i].id == page_id && page_table->pages[i].last_access_time != -1) { 

      return true; 

    } 

  } 

  return false; 

} 

int replace_page(PageTable *page_table, int page_id) { 

  int evicted_page_id = -1; 

  int oldest_access_time = INT_MAX; 

  for (int i = 0; i < page_table->num_pages; i++) { 

    if (page_table->pages[i].last_access_time < oldest_access_time) { 

      evicted_page_id = page_table->pages[i].id; 

      oldest_access_time = page_table->pages[i].last_access_time; 

    } 

  } 

  // write the contents of the evicted page to the swap file 

  char *buf = physical_memory + evicted_page_id * PAGE_SIZE; 

  write(swap_fd, buf, PAGE_SIZE); 

  // update the Page struct with the new page id and access time 

  for (int i = 0; i < page_table->num_pages; i++) { 

    if (page_table->pages[i].id == evicted_page_id) { 

      page_table->pages[i].id = page_id; 

      page_table->pages[i].last_access_time = time(NULL); 

      break; 

    } 

  } 

  return evicted_page_id; 

} 

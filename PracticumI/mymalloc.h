#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <stdint.h>

#include <stdbool.h>

#include <fcntl.h>

#ifndef MYMALLOC_H
#define MYMALLOC_H
#endif

#define NUM_THREADS 4


#define USEDBIT 0x80
#define DIRTYBIT 0x40

// Define page size and number of pages

#define PAGE_SIZE 4096

#define NUM_PAGES 16

// Define the number of bytes to allocate for each malloc 
#define ALLOC_SIZE 256 
 
// Define the number of mallocs to perform 
#define NUM_MALLOCS 10


// Define swap file name

#define SWAP_FILE "swapfile"


// Pre-allocate your "heap" memory from the static store

extern char phys_mem[PAGE_SIZE];

// Define physical memory and swap file descriptors

extern char *physical_memory;

extern int swap_fd;

struct block {

  size_t size; // the size of the block

  int free; // This flag is used to know whether the block described by the
            // metadata structure. It is set to 1 else 0

  struct block *next; // Pointer to the next metadata
};

// Pointing to the main block of memory which is initially free no storage
// allocation yet

extern struct block *freeList;

typedef struct {

  int id; // unique identifier for page

  int last_access_time; // time when page was last accessed

} Page;

typedef struct {

  Page *pages; // array of pages

  int num_pages; // number of pages in table

} PageTable;

// Bitfield struct to store used and dirty status of a page
typedef struct {
    uint8_t status;
} page_status_t;

// Global array of pages
extern page_status_t pageing[NUM_PAGES];

typedef union {
    int i;
    char c[3];
    struct {
        bool allocated : 1;
        bool resident : 1;
        bool dirty : 1;
        bool used : 1;
    };
} page_data_t;

// Define page access status as a boolean value 

typedef bool access_status_t; 

  

// Define page access tracker as a bit vector 
typedef struct access_tracker { 

    access_status_t *pages; 

    size_t num_pages; 

} access_tracker_t; 

// access_tracker_t access_tracker = {
//     .pages = NULL,
//     .num_pages = NUM_PAGES,
// };

void init();

void frag(void *slot, size_t size);

void *pm_malloc_lock(size_t noOfBytes);

void pm_free_lock(void *ptr);

void mergefrag();

void read_page(int page_num, char *buf);

void write_page(int page_num, char *data);

void init_page_table(PageTable *page_table, int num_pages);

int find_lru_page(PageTable *page_table);

void access_page(PageTable *page_table, int page_id, int current_time);

void load_page(PageTable *page_table, int page_id, int current_time);

// thread function for page access tracking 

void *access_tracker(void *arg); 

// Function to get the status of a page
bool is_page_used(uint8_t page_num);

bool is_page_dirty(uint8_t page_num);

// Function to set the status of a page
void set_page_used(uint8_t page_num, bool used);

void set_page_dirty(uint8_t page_num, bool dirty);

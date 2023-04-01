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

//defines num of threads
#define NUM_THREADS 4


// #define USEDBIT 0x80
// #define DIRTYBIT 0x40


//number of pages
#define NUM_PAGES 256


// Define the size of the heap
#define PAGE_SIZE 1024 * 1024 * 16


// Static heap of memory
extern char phys_mem[PAGE_SIZE];

//assigning heap to phys_mem
static char* heap = phys_mem;

// Pointer to the next free block of memory
static char* free_ptr = NULL;

// Structure to store the metadata of each block of memory
typedef struct block {
    size_t size;
    int free;
    int pageNum;    // new field
    struct block* next;
    void *data;
} block_t;


//The structure page_table_entry defines a node for a linked list that represents a page table. Each node contains a virtual_address and physical_address, which are pointers to the virtual and physical addresses of a page in memory, respectively. The dirty and referenced variables are used to keep track of whether the page has been modified or accessed, respectively. The next variable is a pointer to the next node in the linked list.
struct page_table_entry {
  void *virtual_address;
  void *physical_address;
  int dirty;
  int referenced;
  struct page_table_entry *next;
};

// The static keyword in static struct page_table_entry *page_table_head = NULL; specifies that the page_table_head variable is only accessible within the file it is defined in. This variable is a pointer to the first node in the linked list, and is initialized to NULL to indicate an empty list.

static struct page_table_entry *page_table_head = NULL;


// Macro to get the metadata of a block of memory
#define BLOCK(ptr) ((block_t*)((char*)(ptr) - sizeof(block_t)))

//an array for pagetable
extern int pageTab[NUM_PAGES];
//an array for dirt pages
extern int dirty[NUM_PAGES];
//what the page number is 
extern int pageNumber;





/*
    Defines a Virtual Memory Addressing table that can be
    represented as either a TLB cache or Page Table
*/
typedef struct vmTable_t {
    int *pageNumArr; // page number array
    int *frameNumArr; // frame number array for this
    int *entryAgeArr; // Age of each index
    int length;
    int pageFaultCount;
    int tlbHitCount;
    int tlbMissCount;
} vmTable_t;



//function to malloc memory
void* thread_safe_malloc(size_t size);
//function to free memory
void thread_safe_free(void* ptr);
//initilize memeory
void init_heap();
//function to store values at allocated memory,
void pm_put(void *ptr, void *data, size_t size);
//function to retrieve data at allocated memory
void pm_get(void *ptr, void *data, size_t size);


// This function creates a new Virtual Memory Table for
// Logical address referencing -- Can represent either the TLB or Page Table Cache
vmTable_t* createVMtable(int length);

// This function prints contents of the vmTable
void displayTable(vmTable_t** tableToView);

/* DECIDED NOT TO USE -- Too Complex to Implement, but keeping JIC I want to use/fix later
// This function creates a physical memory sparse matrix
dramMatrix* createDRAMmatrix(int frameCount, int frameSize);
*/
// This function frees dynamically allocated memory
void freeVMtable(vmTable_t** table);

// Accepts an int double pointer for creating simulated physical memory space
int** dramAllocate(int frameCount, int blockSize);

// Will free dram memory after usage
void freeDRAM(int*** dblPtrArr, int frameCount);


// 32-Bit masking function to extract page number
int getPageNumber(int mask, int value, int shift);

// 32-Bit masking function to extract page offset
int getOffset(int mask, int value);

//swapping pages
void swap_page(int page_num, char* phys_mem);

//save the dirty page
void save_dirty_page(int page_number, char* page_data);

//save the dirty page to the disk 
void save_dirty_page_to_disk(int** pageTable);

//load dirty page from the disk
int* load_page_from_disk(int pageNumber);

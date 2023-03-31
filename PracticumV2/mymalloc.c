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
#include <sys/statvfs.h>
#include <errno.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static struct block* head = NULL; 


char phys_mem[PAGE_SIZE];

int pageTab[NUM_PAGES];

int dirty[NUM_PAGES];

int pageNumber = 0;

// char *physical_memory = phys_mem;

// struct block *freeList = (struct block *)phys_mem;

// int swap_fd;

// page_status_t pageing[NUM_PAGES];

void init_heap(){
  //head = (struct block*)heap;
  int prot = PROT_READ | PROT_WRITE;
  int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE | MAP_POPULATE;
  head = mmap(NULL, PAGE_SIZE, prot, flags, -1, 0);
  if (head == MAP_FAILED) {
    perror("mmap failed");
    exit(EXIT_FAILURE);
  }
  head->next = NULL;
  head->free = 1;
  head->size = PAGE_SIZE - sizeof(struct block);
}     

void* thread_safe_malloc(size_t size) {
    struct block *curr, *prev;
    void* result = NULL;

    if (head == NULL) {
        init_heap();
    }

    pthread_mutex_lock(&mutex);

    curr = head;
    prev = NULL;

    while (curr != NULL) {
        if (curr->free && curr->size >= size) {
            if (curr->size > size + sizeof(struct block)) {
                // Split the block if it is larger than the requested size
                struct block* new_block = (struct block*)((char*)curr + sizeof(struct block) + size);
                new_block->next = curr->next;
                new_block->free = 1;
                new_block->size = curr->size - sizeof(struct block) - size;
                curr->next = new_block;
                curr->size = size;
            } else{
              curr->free = 0;
              curr->data = curr + 1;
            }
            result = curr->data;
            break;
            // curr->free = 0;
            // result = (void*)(curr + 1);
            // break;
        }
        prev = curr;
        curr = curr->next;
    }

    pthread_mutex_unlock(&mutex);

    return result;
}                                                                                                                                                           
void thread_safe_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }

    pthread_mutex_lock(&mutex);

    struct block* block_ptr = (struct block*)ptr - 1;
    block_ptr->free = 1;

    pthread_mutex_unlock(&mutex);
}


// Function to allocate a block of memory from the heap



// // Function to free a block of memory allocated from the heap


void pm_put(void *ptr, void *data, size_t size) {
    struct block *block_ptr = (struct block *) ptr - 1;
    if (block_ptr->size != size) {
        fprintf(stderr, "Error: Invalid size for data to store in block\n");
        return;
    }
    memcpy(block_ptr->data, data, size);
}

void pm_get(void *ptr, void *data, size_t size) {
    struct block *block_ptr = (struct block *) ptr - 1;
    if (block_ptr->size != size) {
        fprintf(stderr, "Error: Invalid size for data to retrieve from block\n");
        return;
    }
    memcpy(data, block_ptr->data, size);
}




/*
    Creates a Table that Contains array indexable values for Page
    Number and Frame Number for direct translation lookaside buffer (TLB)
    or page table entry (PTE).
    @Param  {length}       Represents the int length of the constructed internal arrays
    @Return {vmTable_t*}   A pointer to the the created table which will represent either the TLB or Page Table
*/
vmTable_t* createVMtable(int length)
{
    vmTable_t* new_table =thread_safe_malloc(sizeof(vmTable_t));
  
    if (new_table == NULL) {
        return NULL;
    }
  
    new_table->length = length;
    new_table->pageNumArr = thread_safe_malloc(sizeof(int) * length);
    new_table->frameNumArr = thread_safe_malloc(sizeof(int) * length);
    new_table->entryAgeArr = thread_safe_malloc(sizeof(int) * length);
  
    if (new_table->pageNumArr == NULL || new_table->frameNumArr == NULL || new_table->entryAgeArr == NULL) {
        thread_safe_free(new_table);
        return NULL;
    }
  
    new_table->pageFaultCount = 0;
    new_table->tlbHitCount = 0;
    new_table->tlbMissCount = 0;

    for (int i = 0; i < length; i++) {
        new_table->pageNumArr[i] = 0;
    }

    // If there is not enough memory on the heap to make a call to malloc() // Notify and Exit
    if(new_table == NULL || new_table->pageNumArr == NULL || new_table->frameNumArr == NULL) {
        printf("Error - Could not allocate a new Virtual Memory Addressing Table!\r\n");
        exit(-1);
    }
    return new_table;
}
/*
    Method to free dynamically allocated memory
    @Param {table} The TLB or Page Table we want to clear from memory
*/
void freeVMtable(vmTable_t** table)
{
    if ((*table)->pageNumArr != NULL) {
        thread_safe_free((*table)->pageNumArr);
    }
    if ((*table)->frameNumArr != NULL) {
        thread_safe_free((*table)->frameNumArr);
    }
    if ((*table)->entryAgeArr != NULL) {
        thread_safe_free((*table)->entryAgeArr);
    }
    thread_safe_free(*table);
}
/*
    This function prints contents of the vmTable
    @Param {tableToView} The TLB or Page Table who's contents we want to view in console
*/
void displayTable(vmTable_t** tableToView)
{
    printf("\n********************* SEQUENCE START ****************************\n ");
    for (int i = 0; i < (*tableToView)->length; i++) {
        printf("Index(%d) := Page Number: %d\tFrame Number: %d\n", i, (*tableToView)->pageNumArr[i], (*tableToView)->frameNumArr[i]);
    }
    printf("\n********************* SEQUENCE END ***************************\n ");
}

void save_dirty_page_to_disk(int** pageTable) {
    int* dirtyPage = pageTable[pageNumber]; 
    FILE* file = fopen("DirtyFile.txt", "rb+");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open file %s for writing\n", file);
        exit(EXIT_FAILURE);
    }
    fseek(file, pageNumber * PAGE_SIZE, SEEK_SET);
    fwrite(dirtyPage, sizeof(int), PAGE_SIZE / sizeof(int), file);
    fflush(file);
    fclose(file);
    dirty[pageNumber] = 0; // mark page as clean
}


int* load_page_from_disk(int pageNumber) {
    int* page = thread_safe_malloc(sizeof(int) * PAGE_SIZE / sizeof(int));
    if (page == NULL) {
        fprintf(stderr, "Error: Out of memory\n");
        exit(EXIT_FAILURE);
    }
    FILE* file = fopen("DirtyFile.txt", "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open file %s for reading\n", file);
        exit(EXIT_FAILURE);
    }
    fseek(file, pageNumber * PAGE_SIZE, SEEK_SET);
    fread(page, sizeof(int), PAGE_SIZE / sizeof(int), file);
    fclose(file);
    return page;
}

/*
    Creating simulated physical memory space
    @Param  {frameCount} The number of frames we want to represent in physical memory
    @Param  {blockSize}  The number of offsets we want per physical memory frame
    @Return {int**}      The dynamically allocated physical memory space
*/
int** dramAllocate(int frameCount, int blockSize)
{
    int** temp;
    temp = thread_safe_malloc(frameCount * sizeof(int*));
    if (temp == NULL) {
        fprintf(stderr, "Error: Out of memory\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < frameCount; i++) {
        temp[i] = (int*)thread_safe_malloc(sizeof(int) * blockSize);
        if (temp[i] == NULL) {
          for (int j = 0; j < i; j++) {
            thread_safe_free(temp[j]);
          }
          thread_safe_free(temp);
          fprintf(stderr, "Error: Out of memory\n");
          exit(EXIT_FAILURE);
        }
        for(int j = 0; j < blockSize; j++) {
            temp[i][j] = 0;
        }
    }
    // If there is not enough memory to make call to malloc() // Notify and exit
    if(temp == NULL) {
        printf("Error - Could not allocate a new Physical Memory Matrix using dramAllocate() function!\r\n");
        exit(-1);
    }
    return temp;
}
/*
    Will free dram memory after usage
    @Param {dblPtrArr}  The physical memory we want to clear
    @Param {frameCount} The number of frames in the specified physical memory
*/
void freeDRAM(int*** dblPtrArr, int frameCount)
{
  if (*dblPtrArr == NULL) {
        return;
    }
  
  for (int i = 0; i < frameCount; i++) {
      if ((*dblPtrArr)[i] != NULL) {
          thread_safe_free((*dblPtrArr)[i]);
      }
  }
  thread_safe_free(*dblPtrArr);
}

/*
    32-Bit masking function to extract page number
    This function assumes a high order page number and
    a low order page offset
    @Param {mask}   The int masking value we will use to perform AND operation w.r.t. value
    @Param {value}  The int value we wish to mask
    @Param {shift}  The relative number of bits we want to shift right after the bitwise operation
    @Return {int}   The int representation for Page Number
*/
int getPageNumber(int mask, int value, int shift) {
   return ((value & mask)>>shift);
}

/*
    32-Bit masking function to extract physical memory offset
    This function assumes a high order page number and
    a low order page offset
    @Param {mask}   The int masking value we will use to perform AND operation w.r.t. value
    @Param {value}  The int value we wish to mask
    @Return {int}   The int representation for physical memory offset
*/
int getOffset(int mask, int value) {
   return value & mask;
}

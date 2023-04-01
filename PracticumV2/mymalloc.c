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
#include <assert.h>
#include <sys/statvfs.h>
#include <errno.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static struct block* head = NULL; 


char phys_mem[PAGE_SIZE];

int pageTab[NUM_PAGES];

int dirty[NUM_PAGES];

int pageNumber = 0;



// The body of the function performs the following operations:

// int prot = PROT_READ | PROT_WRITE;
// This line sets the protection flags for the memory page to be allocated. In this case, the page is marked as readable and writable.

// int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE | MAP_POPULATE;
// This line sets the flags for the mmap system call. The MAP_PRIVATE flag ensures that changes to the memory page are not visible to other processes. The MAP_ANONYMOUS flag tells the system to allocate an anonymous memory region instead of mapping a file. The MAP_NORESERVE flag tells the system not to reserve swap space for the memory page. The MAP_POPULATE flag tells the system to prepopulate the memory page with zeros.

// head = mmap(NULL, PAGE_SIZE, prot, flags, -1, 0);
// This line calls the mmap system call to allocate a single memory page. The arguments are:

// NULL: The address at which to start the mapping (the system will choose a suitable address).
// PAGE_SIZE: The size of the memory page to be allocated.
// prot: The protection flags for the memory page.
// flags: The flags for the mmap system call.
// -1: The file descriptor for the file to be mapped (not relevant for anonymous memory regions).
// 0: The offset within the file to start the mapping (not relevant for anonymous memory regions).
// if (head == MAP_FAILED) { perror("mmap failed"); exit(EXIT_FAILURE); }
// This line checks if the mmap system call succeeded. If it failed, it prints an error message using perror and exits the program with a failure code.

// for(int i = 0; i < NUM_PAGES; i++) { pageTab[i] = -1; }
// This line initializes the page table by setting all entries to -1. The page table is an array that stores the index of the block that owns each page in the heap.

// head->next = NULL;
// This line initializes the head of the linked list by setting its next pointer to NULL. The head represents the entire heap.

// head->free = 1;
// This line sets the free flag of the head to 1, indicating that the entire heap is currently free.

// head->size = PAGE_SIZE - sizeof(struct block);
// This line sets the size of the head block to the size of the memory page minus the size of the block header. The block header contains metadata about the block, such as its size and whether it is free.


void init_heap(){
  //head = (struct block*)heap;
  int prot = PROT_READ | PROT_WRITE;
  int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE | MAP_POPULATE;
  head = mmap(NULL, PAGE_SIZE, prot, flags, -1, 0);
  if (head == MAP_FAILED) {
    perror("mmap failed");
    exit(EXIT_FAILURE);
  }
  // Initialize pageTab to -1
  for(int i = 0; i < NUM_PAGES; i++) {
    pageTab[i] = -1;
  }
  head->next = NULL;
  head->free = 1;
  head->size = PAGE_SIZE - sizeof(struct block);
}     


// Function to allocate a block of memory from the heap
//the function is correctly allocating memory and handling page faults, and it returns a valid pointer to the allocated memory.
 // thread_safe_malloc(), is a thread-safe memory allocation function that returns a pointer to a block of memory of the requested size. The function looks for a free block in a linked list of blocks. If a suitable block is found, it is split into two blocks if it is larger than the requested size. Otherwise, the block is marked as used and returned. If no suitable block is found, the function returns NULL. The function also checks if the requested memory is already in physical memory. If it is, it updates the referenced bit in the page table. If it is not, it allocates a new page and updates the page table.

   void* thread_safe_malloc(size_t size) {
    static int pageNumber = 0;
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
  
    // Check if a suitable block was found
    if (result == NULL) {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
   

    // Check if the requested memory is already in physical memory
    int page_num = ((uintptr_t)result - (uintptr_t)phys_mem) / PAGE_SIZE;
    if (page_num >= 0 && page_num < PAGE_SIZE && pageTab[page_num] != -1) {
      // Page is already in physical memory, update referenced bit
      struct page_table_entry *pte = page_table_head;
      while (pte != NULL) {
        if (pte->virtual_address == result) {
          pte->referenced = 1;
          break;
        }
        pte = pte->next;
      }
    } else if (page_num >= 0 && page_num < NUM_PAGES) {
      // Page is not in physical memory, allocate a new page and update page table
      void *page = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      if (page == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
      }
      // printf("value %d\n", page_num);
      // printf("value %d\n", NUM_PAGES);
      // assert(page_num >= 0 && page_num < NUM_PAGES);

      if (page_num < 0 || page_num >= NUM_PAGES) {
        pthread_mutex_unlock(&mutex);
        return NULL; // or return an appropriate error code
      }

      pageTab[page_num] = pageNumber;
      pageNumber++;
      dirty[page_num] = 0;
      struct page_table_entry *pte = malloc(sizeof(struct page_table_entry));
      pte->virtual_address = result;
      pte->physical_address = page;
      pte->dirty = 0;
      pte->referenced = 1;
      pte->next = page_table_head;
      page_table_head = pte;
      memcpy(page, result, size);
    }

    pthread_mutex_unlock(&mutex);

    return result;
}     


// The pm_put() function is used to store data in a block of memory allocated from the heap. It takes a pointer to the block, a pointer to the data to store, and the size of the data. The function first checks that the size of the data matches the size of the block. If it does, the function copies the data to the block.

void pm_put(void *ptr, void *data, size_t size) {
    struct block *block_ptr = (struct block *) ptr - 1;
    if (block_ptr->size != size) {
        fprintf(stderr, "Error: Invalid size for data to store in block\n");
        return;
    }
    memcpy(block_ptr->data, data, size);
}



// Function to free a block of memory allocated from the heap
// The thread_safe_free() function frees a block of memory allocated from the heap. The function first checks if the block is dirty. If it is, the function saves the block to disk and clears the dirty flag for the page. The function then checks if there is enough disk space. Finally, the function removes the page from the page table.

void thread_safe_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }

    pthread_mutex_lock(&mutex);

    struct block* block_ptr = (struct block*)ptr - 1;

    // Check if the block is dirty
    if (dirty[block_ptr->pageNum]) {
        // Save the block to disk
        int fd = open("pagefile.bin", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            perror("Error opening pagefile.bin");
            exit(EXIT_FAILURE);
        }
        if (lseek(fd, block_ptr->pageNum * PAGE_SIZE, SEEK_SET) == -1) {
            perror("Error seeking to page position in pagefile.bin");
            exit(EXIT_FAILURE);
        }
        if (write(fd, block_ptr->data, PAGE_SIZE) == -1) {
            perror("Error writing page to pagefile.bin");
            exit(EXIT_FAILURE);
        }
        close(fd);

        // Clear the dirty flag for the page
        dirty[block_ptr->pageNum] = 0;
         // Check if there is enough disk space
        struct statvfs buf;
        if (statvfs(".", &buf) == -1) {
            perror("Error getting filesystem statistics");
            exit(EXIT_FAILURE);
        }
        unsigned long long free_space = buf.f_bsize * buf.f_bfree;
        if (free_space < PAGE_SIZE) {
            fprintf(stderr, "Error: Not enough disk space\n");
            exit(EXIT_FAILURE);
        }
    }
       // Remove the page from the page table
    int page_num = ((uintptr_t)block_ptr->data - (uintptr_t)phys_mem) / PAGE_SIZE;
    struct page_table_entry *pte = page_table_head;
    struct page_table_entry *prev_pte = NULL;
    while (pte != NULL) {
      if (pte->virtual_address == block_ptr->data) {
        if (prev_pte == NULL) {
          page_table_head = pte->next;
        } else {
          prev_pte->next = pte->next;
        }
        free(pte);
        break;
      }
      prev_pte = pte;
      pte = pte->next;
    }
  
  
    block_ptr->free = 1;

   

    pthread_mutex_unlock(&mutex);
}




// the pm_get The function takes three parameters:

// ptr: a void pointer to a block of memory
// data: a void pointer to a block of memory where the retrieved data will be stored
// size: the size of the data to be retrieved
// The function retrieves data from a block of memory pointed to by ptr. The block of memory is assumed to have been previously allocated with malloc() and to contain a header struct block followed by the actual data. The header contains information about the size of the block and whether it is free or in use. If the size of the block does not match the size specified by the size parameter, an error message is printed and the function returns without retrieving the data.

// If the block is marked as free, the function reads the page containing the data from a file on disk and stores it in the block's data field. The pageNum field of the block is used to determine which page to read.

// Finally, the function copies the retrieved data to the memory pointed to by data using memcpy().


void pm_get(void *ptr, void *data, size_t size) {
    struct block *block_ptr = (struct block *) ptr - 1;
    if (block_ptr->size != size) {
        fprintf(stderr, "Error: Invalid size for data to retrieve from block\n");
        return;
    }

    if (block_ptr->free) {
        // Load the page from disk
        int fd = open("pagefile.bin", O_RDONLY);
        if (fd == -1) {
            perror("Error opening pagefile.bin");
            exit(EXIT_FAILURE);
        }
        if (lseek(fd, block_ptr->pageNum * PAGE_SIZE, SEEK_SET) == -1) {
            perror("Error seeking to page position in pagefile.bin");
            exit(EXIT_FAILURE);
        }
        if (read(fd, block_ptr->data, PAGE_SIZE) == -1) {
            perror("Error reading page from pagefile.bin");
            exit(EXIT_FAILURE);
        }
        close(fd);
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

     // Check for errors in memory allocation
    if (new_table->pageNumArr == NULL || new_table->frameNumArr == NULL || new_table->entryAgeArr == NULL) {
        thread_safe_free(new_table->pageNumArr);
        thread_safe_free(new_table->frameNumArr);
        thread_safe_free(new_table->entryAgeArr);
        thread_safe_free(new_table);
        return NULL;
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


//void save_dirty_page_to_disk(int** pageTable): This function takes in a two-dimensional integer array pageTable as input. The pageTable array represents a page table that stores the mappings between virtual and physical memory. The function saves the contents of a dirty page to disk by writing the page data to a file named DirtyFile.txt. It first retrieves the dirty page by accessing pageTable using the pageNumber parameter. It then opens the file in binary read/write mode and writes the page data to the file using fwrite(). The function then marks the page as clean by setting the corresponding dirty bit in the dirty array to 0.


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


//int* load_page_from_disk(int pageNumber): This function takes in an integer pageNumber as input and returns a pointer to an integer array. The function loads a page from disk by reading the corresponding data from a file named testinput.txt. It first allocates memory for the page data using thread_safe_malloc() and opens the file in binary read mode. The function then reads the page data from the file using fread() and closes the file. The page data is returned as a pointer to an integer array.


int* load_page_from_disk(int pageNumber) {
    int* page = thread_safe_malloc(sizeof(int) * PAGE_SIZE / sizeof(int));
    if (page == NULL) {
        fprintf(stderr, "Error: Out of memory\n");
        exit(EXIT_FAILURE);
    }
    FILE* file = fopen("testinput.txt", "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open file %s for reading\n", file);
        exit(EXIT_FAILURE);
    }
    fseek(file, pageNumber * PAGE_SIZE, SEEK_SET);
    fread(page, sizeof(int), PAGE_SIZE / sizeof(int), file);
    fclose(file);
    return page;
}


//void swap_page(int page_num, char* phys_mem): This function takes in an integer page_num and a pointer to a character array phys_mem as input. The function swaps a page from virtual memory to physical memory by mapping a new page to the current page number and unmapping the previous page. It first unmaps the page at the current page number using munmap() and then maps a new page to the current page number using mmap(). The function sets the protection flags to allow read and write access to the page, and sets the mapping flags to make the mapping private, anonymous, and non-reserved, and to populate the page with data.


void swap_page(int page_num, char* phys_mem) {
    // unmap the page at the current page number
    if (munmap(phys_mem + page_num * PAGE_SIZE, PAGE_SIZE) < 0) {
        perror("munmap failed");
        exit(EXIT_FAILURE);
    }

    // map a new page to the current page number
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE | MAP_POPULATE;
    void* new_page = mmap(phys_mem + page_num * PAGE_SIZE, PAGE_SIZE, prot, flags, -1, 0);
    if (new_page == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
}


//void save_dirty_page(int page_number, char* page_data): This function takes in an integer page_number and a pointer to a character array page_data as input. The function saves the contents of a dirty page to disk by writing the page data to a file named page_file. It first opens the file in write-only mode and writes the page data to the file using write(). The function then marks the page as clean by setting the corresponding dirty bit in the dirty array to 0.


void save_dirty_page(int page_number, char* page_data) {
    if (dirty[page_number]) {
        // Write the contents of the page to disk
        int fd = open("page_file", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        off_t offset = page_number * PAGE_SIZE;
        if (lseek(fd, offset, SEEK_SET) == -1) {
            perror("lseek");
            exit(EXIT_FAILURE);
        }
        if (write(fd, page_data, PAGE_SIZE) != PAGE_SIZE) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        if (close(fd) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }
        // Mark the page as clean
        dirty[page_number] = 0;
    }
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
        // Initialize memory to 0
        memset(temp[i], 0, blockSize * sizeof(int));
      
        
        
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

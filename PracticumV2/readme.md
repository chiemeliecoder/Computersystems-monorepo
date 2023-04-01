Practicum I

-------------------------------------------------------------------------------------------------------------------------------------------------------------------
# 1 introduction and explanation of code
-------------------------------------------------------------------------------------------------------------------------------------------------------------------

The code is a C program that implements memory management and virtual memory.

The program includes several header files for standard input/output, memory allocation, and synchronization primitives. The code defines several global variables and data structures for tracking the state of allocated memory blocks and virtual memory addresses.


The code defines a mutex variable for thread safety, and initializing a linked list called head.


# The init_heap() 

function initializes a linked list of blocks representing the heap of available memory, using the mmap() system call to reserve a page of memory from the operating system. The thread_safe_malloc() function allocates a block of memory from the heap using a thread-safe locking mechanism, splitting larger blocks as needed to satisfy the request. The thread_safe_free() function frees a previously allocated block of memory from the heap.

The code also includes functions for managing a virtual memory addressing table, including creating and freeing the table and performing lookups based on page numbers and frame numbers.

# int** dramAllocate(int frameCount, int blockSize):

This function allocates dynamic memory for a 2D integer array with frameCount rows and blockSize columns.
It returns a pointer to the allocated memory.
If there is not enough memory to allocate, the function prints an error message and exits the program.

# void freeDRAM(int*** dblPtrArr, int frameCount):

This function frees the dynamically allocated memory pointed to by dblPtrArr.
It takes frameCount as a parameter, which specifies the number of frames in the memory to be freed.
If dblPtrArr is already NULL, the function returns without doing anything.

# vmTable_t* createVMtable(int length):

This function allocates dynamic memory for a vmTable_t struct, which contains three dynamically allocated integer arrays of size length.
It initializes the fields of the struct to appropriate values.
It returns a pointer to the newly allocated struct.
If there is not enough memory to allocate, the function returns NULL.



# thread_safe_malloc() 

is a thread-safe wrapper function for allocating memory. It takes a size parameter, and iterates over the linked list of blocks until it finds a block that is both free and large enough to hold the requested size. If the block is larger than the requested size, it splits the block into two new blocks, and returns a pointer to the first block's data. If the block is the exact size of the requested size, it marks the block as allocated and returns a pointer to the block's data.

# thread_safe_free() 

is a thread-safe wrapper function for freeing allocated memory. It takes a pointer to the memory block, sets the block's free flag to 1, and returns.

# pm_put() and pm_get() 

are functions for storing and retrieving data in the memory blocks. pm_put() takes a pointer to the block, a pointer to the data, and the size of the data. It checks if the block's size matches the size of the data, and if so, it copies the data into the block. pm_get() takes the same parameters, but copies data from the block into the data pointer.

# createVMtable() 

is a function for creating a virtual memory table, which is represented as an array of page numbers and frame numbers. The function takes a length parameter, and creates a new vmTable_t struct. The struct contains three arrays for page numbers, frame numbers, and entry ages, as well as counts for page faults, TLB hits, and TLB misses. The function also initializes the arrays to zero.











-------------------------------------------------------------------------------------------------------------------------------------------------------------------
# 2 Testcases
-------------------------------------------------------------------------------------------------------------------------------------------------------------------
# first test case 
Error handled  

the code below handled error showing Error: Out of memory
The allocation of memory has failed in the dramAllocate function, then the function will exit with an error message. The code you provided already has the proper error handling for this case, which is to print an error message and exit the program with a failure status. 

If you want to handle this error in a different way, you could modify the error message or change the exit status. For example, you could change the exit status to a non-negative integer to indicate a specific error code. Or you could print a different error message to provide more information about the failure. 

However, it's important to note that in most cases, the proper handling for a failed memory allocation is to exit the program with a failure status. This is because the program cannot continue to run without the required memory, and any further execution could result in undefined behavior or crashes. 





```c
    tlbTable = createVMtable(TLB_SIZE); // The TLB Structure
    pageTable = createVMtable(PAGE_TABLE_SIZE); // The Page Table
    dram = dramAllocate(TOTAL_FRAME_COUNT, FRAME_SIZE); // Physical Memory
    int translationCount = 0;
    char* algoName;

    // perform basic error checking
    if (argc != 2) {
        fprintf(stderr,"Usage: ./main [input file]\n");
        return -1;
    }

    // open the file containing the backing store
    backing_store = fopen("BACKING_STORE.bin", "rb");

    if (backing_store == NULL) {
        fprintf(stderr, "Error opening BACKING_STORE.bin %s\n","BACKING_STORE.bin");
        return -1;
    }

    // open the file containing the logical addresses
    address_file = fopen(argv[1], "r");

    if (address_file == NULL) {
        fprintf(stderr, "Error opening %s. Expecting [InputFile].txt or equivalent.\n",argv[1]);
        return -1;
    }


    printf("\nWelcome to Don's VM Simulator Version 1.0");
    printf("\nNumber of logical pages: %d\nPage size: %d bytes\nPage Table Size: %d\nTLB Size: %d entries\nNumber of Physical Frames: %d\nPhysical Memory Size: %d bytes", PAGE_TABLE_SIZE, PAGE_READ_SIZE, PAGE_TABLE_SIZE, TLB_SIZE, FRAME_SIZE, PAGE_READ_SIZE * FRAME_SIZE);


    do {
        printf("\nDisplay All Physical Addresses? [y/n]: ");
        scanf("\n%c", &display_choice);
    } while (display_choice != 'n' && display_choice != 'y');

    do {
        printf("Choose TLB Replacement Strategy [1: FIFO, 2: LRU]: ");
        scanf("\n%c", &algo_choice);
    } while (algo_choice != '1' && algo_choice != '2');



    // Read through the input file and output each virtual address
    while (fgets(addressReadBuffer, MAX_ADDR_LEN, address_file) != NULL) {
        virtual_addr = atoi(addressReadBuffer); // converting from ascii to int

        // 32-bit masking function to extract page number
        page_number = getPageNumber(PAGE_MASK, virtual_addr, SHIFT);

        // 32-bit masking function to extract page offset
        offset_number = getOffset(OFFSET_MASK, virtual_addr);

        // Get the physical address and translatedValue stored at that address
        translateAddress(algo_choice);
        translationCount++;  // increment the number of translated addresses
    }

    // Determining stdout algo name for Menu
    if (algo_choice == '1') {
        algoName = "FIFO";
    }
    else {
        algoName = "LRU";
    }

    printf("\n-----------------------------------------------------------------------------------\n");
    // calculate and print out the stats
    printf("\nResults Using %s Algorithm: \n", algoName);
    printf("Number of translated addresses = %d\n", translationCount);
    double pfRate = (double)pageTable->pageFaultCount / (double)translationCount;
    double TLBRate = (double)tlbTable->tlbHitCount / (double)translationCount;

    printf("Page Faults = %d\n", pageTable->pageFaultCount);
    printf("Page Fault Rate = %.3f %%\n",pfRate * 100);
    printf("TLB Hits = %d\n", tlbTable->tlbHitCount);
    printf("TLB Hit Rate = %.3f %%\n", TLBRate * 100);
    printf("Average time spent retrieving data from backing store: %.3f millisec\n", getAvgTimeInBackingStore());
    printf("\n-----------------------------------------------------------------------------------\n");

    // close the input file and backing store
    fclose(address_file);
    fclose(backing_store);

    // NOTE: REMEMBER TO FREE DYNAMICALLY ALLOCATED MEMORY
    freeVMtable(&tlbTable);
    freeVMtable(&pageTable);
    freeDRAM(&dram, TOTAL_FRAME_COUNT);
```

# The second test case thread_function


The function is testing the thread-safe memory allocation and deallocation functions by creating multiple threads and having them allocate and free memory concurrently. Each thread is assigned a unique thread ID and allocates 10 bytes of memory using thread_safe_malloc(). It then prints the allocated memory address and frees the memory using thread_safe_free(). The function then exits the thread using pthread_exit(). The purpose of the test is to ensure that the memory allocation and deallocation functions are thread-safe and can be used safely in a multi-threaded environment without causing any memory corruption or other issues.

```c
void *thread_function(void *arg) {
  int thread_id = *(int *)arg;
  printf("Thread %d starting...\n", thread_id);
  void *mem = thread_safe_malloc(10);
  printf("Thread %d allocated memory at %p\n", thread_id, mem);
  thread_safe_free(mem);
  printf("Thread %d freeing memory at %p\n", thread_id, mem);
  pthread_exit(NULL);
}
```

# The third test case thread_func2


The function thread_func2 is testing a variety of functionalities related to synchronization, memory management, and persistence:

Synchronization: The function increments a shared integer variable shared_data by 1, which requires synchronization to avoid race conditions.
Memory management: The function allocates and frees memory using thread_safe_malloc and thread_safe_free functions, respectively. It also uses pm_put and pm_get functions to write and read data from persistent memory.
Persistence: The function saves a dirty page to disk using save_dirty_page_to_disk function, loads a page from disk using load_page_from_disk function, and swaps a page using swap_page function. These operations simulate the behavior of a memory management system with a page replacement policy that stores some pages on disk.


```c
int shared_data = 0;


void *thread_func2(void *arg) {
  int tid = *(int *)arg;
  int data = tid + 1;

  // Perform some work that requires synchronization
  shared_data += 1;
  printf("Thread %d incremented shared data to %d\n", tid, shared_data);
 

  // Perform some memory-related work
  int *ptr = (int *)thread_safe_malloc(sizeof(int));
  *ptr = tid;
  printf("Thread %d allocated memory at %p\n", tid, ptr);

  // Save a dirty page to disk
  save_dirty_page_to_disk(pageTab);

  // Load a page from disk
  int *page = load_page_from_disk(pageNumber);

  // Swap a page
  swap_page(pageNumber, phys_mem);

  // Save a dirty page
  save_dirty_page(pageNumber, (char *)page);

  
  pm_put(ptr, &data, sizeof(int));

  // Retrieve data and print
  int result = 0;
  pm_get(ptr, &result, sizeof(int));
  printf("Thread %d: data = %d\n", tid, result);

  thread_safe_free(page);
  thread_safe_free(ptr);
  pthread_exit(NULL);
}
}
```






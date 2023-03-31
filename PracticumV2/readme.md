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

Error handled  

The allocation of memory has failed in the dramAllocate function, then the function will exit with an error message. The code you provided already has the proper error handling for this case, which is to print an error message and exit the program with a failure status. 

If you want to handle this error in a different way, you could modify the error message or change the exit status. For example, you could change the exit status to a non-negative integer to indicate a specific error code. Or you could print a different error message to provide more information about the failure. 

However, it's important to note that in most cases, the proper handling for a failed memory allocation is to exit the program with a failure status. This is because the program cannot continue to run without the required memory, and any further execution could result in undefined behavior or crashes. 



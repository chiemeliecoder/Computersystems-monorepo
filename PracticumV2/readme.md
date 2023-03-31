Practicum I

-------------------------------------------------------------------------------------------------------------------------------------------------------------------
# 1 introduction and explanation of code
-------------------------------------------------------------------------------------------------------------------------------------------------------------------


int** dramAllocate(int frameCount, int blockSize):

This function allocates dynamic memory for a 2D integer array with frameCount rows and blockSize columns.
It returns a pointer to the allocated memory.
If there is not enough memory to allocate, the function prints an error message and exits the program.
void freeDRAM(int*** dblPtrArr, int frameCount):

This function frees the dynamically allocated memory pointed to by dblPtrArr.
It takes frameCount as a parameter, which specifies the number of frames in the memory to be freed.
If dblPtrArr is already NULL, the function returns without doing anything.
vmTable_t* createVMtable(int length):

This function allocates dynamic memory for a vmTable_t struct, which contains three dynamically allocated integer arrays of size length.
It initializes the fields of the struct to appropriate values.
It returns a pointer to the newly allocated struct.
If there is not enough memory to allocate, the function returns NULL.

This code appears to be a partial implementation of a virtual memory manager. It defines several functions for allocating and freeing memory, and creates a virtual memory table using an array of page numbers and frame numbers.

The code begins by including several header files, defining a mutex variable for thread safety, and initializing a linked list called head.

init_heap() is called to create a new block of memory for the heap if it has not been initialized yet. This function uses mmap() to allocate memory, sets the block to be free, and updates the block's metadata to indicate its size.

thread_safe_malloc() is a thread-safe wrapper function for allocating memory. It takes a size parameter, and iterates over the linked list of blocks until it finds a block that is both free and large enough to hold the requested size. If the block is larger than the requested size, it splits the block into two new blocks, and returns a pointer to the first block's data. If the block is the exact size of the requested size, it marks the block as allocated and returns a pointer to the block's data.

thread_safe_free() is a thread-safe wrapper function for freeing allocated memory. It takes a pointer to the memory block, sets the block's free flag to 1, and returns.

pm_put() and pm_get() are functions for storing and retrieving data in the memory blocks. pm_put() takes a pointer to the block, a pointer to the data, and the size of the data. It checks if the block's size matches the size of the data, and if so, it copies the data into the block. pm_get() takes the same parameters, but copies data from the block into the data pointer.

createVMtable() is a function for creating a virtual memory table, which is represented as an array of page numbers and frame numbers. The function takes a length parameter, and creates a new vmTable_t struct. The struct contains three arrays for page numbers, frame numbers, and entry ages, as well as counts for page faults, TLB hits, and TLB misses. The function also initializes the arrays to zero.

Finally, there is a comment indicating that the code is missing some additional functionality related to swapping and page status tracking.









-------------------------------------------------------------------------------------------------------------------------------------------------------------------
# 2 introduction and explanation of code
-------------------------------------------------------------------------------------------------------------------------------------------------------------------




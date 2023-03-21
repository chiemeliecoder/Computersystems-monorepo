Practicum I

The given code covers the requirement to build a program-managed heap that allows programs to allocate memory from that heap. The code defines a struct called block that stores metadata about the allocated blocks of memory, and uses a linked list of block structs to keep track of free memory.

The code also defines a function pm_malloc_lock that allocates memory from the program-managed heap. 

The code includes various header files and defines several constants such as the number of threads, the page size, the number of pages, the allocation size, and the number of mallocs to perform. It also defines a swap file name and pre-allocates a "heap" memory from the static store.

The code declares several structs and typedefs, including a block struct that stores information about memory blocks and a Page struct that stores information about memory pages. There is also a PageTable struct that contains an array of Page structs and a num_pages field to keep track of the number of pages in the table.

The code defines a bitfield struct to store used and dirty status of a page, as well as a union struct that can be used to access the bits in the page_status_t struct. Additionally, the code defines a typedef for access_status_t, which is a boolean value that indicates whether a page has been accessed or not. There is also a typedef for access_tracker_t, which is a bit vector that tracks the access status of pages.

The code includes a mymalloc.h header file and initializes a mutex variable for thread synchronization. The init function initializes the freeList struct and opens the swap file. The frag function is used to split a block of memory into two smaller blocks if the memory is too large for the requested size. The allocate_page function allocates a physical memory page and maps it to a virtual page.


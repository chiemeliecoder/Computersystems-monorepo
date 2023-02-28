Virtual mem
In this continuation, we demonstrate how to allocate and deallocate pages from the virtual memory. The allocate_page() function returns a pointer to a newly allocated page in the virtual memory, and the deallocate_page() function frees the page at the given page number. The strcpy() function is used to copy the string data to the newly allocated page.

intestmem
we demonstrate how to allocate and free multiple pages at once using loops. We first allocate and write to a single page, as before. Then we allocate and write to five more pages, and read from them to verify that the data was written correctly. Finally, we free all of the allocated pages using a loop.

vmencustomheap we defined a custom heap using the allocate() and deallocate() functions. These functions simply increment or decrement a pointer into the virtual memory, and do not support deallocation. We also define a swap file name, and use the open() and ftruncate() functions to create and resize


for page replacement
In this implementation, we use an array page_table to represent the pages in memory, where each element of the array corresponds to a frame in memory. We also use an integer oldest_page_index to keep track of the index of the oldest page in memory.

We initialize the page table to -1 (no page in memory) and loop over the reference string. For each page in the reference string, we check if the page is already in memory. If the page is not in memory, we replace the oldest page in memory with the new page and update the oldest_page_index. We also increment the num_faults counter to keep track of the number of page faults.

Finally, we print the current state of the page table after each page reference and the total number of page faults at the end.

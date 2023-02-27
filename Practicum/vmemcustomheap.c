#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// Define page size and number of pages
#define PAGE_SIZE 4096
#define NUM_PAGES 16

// Define swap file name
#define SWAP_FILE "swapfile"

// Define physical memory and swap file descriptors
char* physical_memory;
int swap_fd;

// Define heap variables
char* heap_start;
char* heap_end;
char* next_alloc;

// Initialize virtual memory
void init_virtual_memory() {
    // Allocate physical memory
    physical_memory = malloc(PAGE_SIZE * NUM_PAGES);
    if (physical_memory == NULL) {
        perror("Failed to allocate physical memory");
        exit(1);
    }

    // Initialize heap variables
    heap_start = physical_memory;
    heap_end = physical_memory + PAGE_SIZE * NUM_PAGES;
    next_alloc = heap_start;

    // Initialize swap file
    swap_fd = open(SWAP_FILE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (swap_fd == -1) {
        perror("Failed to open swap file");
        exit(1);
    }
    if (ftruncate(swap_fd, PAGE_SIZE * NUM_PAGES) == -1) {
        perror("Failed to resize swap file");
        exit(1);
    }
}

// Read data from a page in virtual memory
void read_page(int page_num, char* buf) {
    if (page_num < 0 || page_num >= NUM_PAGES) {
        fprintf(stderr, "Invalid page number: %d\n", page_num);
        exit(1);
    }
    memcpy(buf, physical_memory + PAGE_SIZE * page_num, PAGE_SIZE);
}

// Write data to a page in virtual memory
void write_page(int page_num, char* data) {
    if (page_num < 0 || page_num >= NUM_PAGES) {
        fprintf(stderr, "Invalid page number: %d\n", page_num);
        exit(1);
    }
    memcpy(physical_memory + PAGE_SIZE * page_num, data, PAGE_SIZE);
}

// Allocate memory from the heap
void* allocate(size_t size) {
    void* result = next_alloc;
    next_alloc += size;
    if (next_alloc > heap_end) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    return result;
}

// Deallocate memory from the heap
void deallocate(void* ptr) {
    // Do nothing, as our heap does not support deallocation
}

int main() {
    // Initialize virtual memory
    init_virtual_memory();

    // Write to page 0
    char data[] = "Hello, world!";
    write_page(0, data);

    // Read from page 0
    char buf[PAGE_SIZE];
    read_page(0, buf);
    printf("Page 0 contains: %s\n", buf);

    // Allocate and write to heap memory
    char* data2 = allocate(16);
    strcpy(data2, "Hello, heap!");
    printf("Heap memory contains: %s\n", data2);

    // Clean up
    free(physical_memory);
    close(swap_fd);
    unlink(SWAP_FILE);

    return 0;
}

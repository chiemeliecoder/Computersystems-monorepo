#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PAGE_SIZE 4096
#define NUM_PAGES 1024
#define SWAP_FILE "swapfile.bin"
#define SWAP_SIZE (NUM_PAGES * PAGE_SIZE)

int page_table[NUM_PAGES];
char *physical_memory;
int swap_fd;

void init_virtual_memory() {
    // Allocate physical memory
    physical_memory = malloc(NUM_PAGES * PAGE_SIZE);
    if (!physical_memory) {
        perror("Failed to allocate physical memory");
        exit(1);
    }

    // Initialize page table
    memset(page_table, -1, sizeof(page_table));
    
    // Open swap file
    swap_fd = open(SWAP_FILE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (swap_fd == -1) {
        perror("Failed to open swap file");
        exit(1);
    }
    
    // Set size of swap file
    if (ftruncate(swap_fd, SWAP_SIZE) == -1) {
        perror("Failed to set size of swap file");
        exit(1);
    }
}

void read_page(int page_num, char *buf) {
    // Check if page is in physical memory
    if (page_table[page_num] != -1) {
        // Page is in physical memory, copy to buffer
        memcpy(buf, physical_memory + (page_table[page_num] * PAGE_SIZE), PAGE_SIZE);
    } else {
        // Page is not in physical memory, read from swap file
        if (lseek(swap_fd, page_num * PAGE_SIZE, SEEK_SET) == -1) {
            perror("Failed to seek to page in swap file");
            exit(1);
        }
        if (read(swap_fd, buf, PAGE_SIZE) != PAGE_SIZE) {
            perror("Failed to read page from swap file");
            exit(1);
        }
    }
}

void write_page(int page_num, char *buf) {
    // Check if page is in physical memory
    if (page_table[page_num] != -1) {
        // Page is in physical memory, copy from buffer
        memcpy(physical_memory + (page_table[page_num] * PAGE_SIZE), buf, PAGE_SIZE);
    } else {
        // Page is not in physical memory, allocate new page
        int free_frame = -1;
        for (int i = 0; i < NUM_PAGES; i++) {
            if (page_table[i] == -1) {
                free_frame = i;
                break;
            }
        }
        if (free_frame == -1) {
            // No free frames, swap out least recently used page
            // (not implemented in this example)
            perror("No free frames");
            exit(1);
        }
        // Read page from swap file into physical memory
        if (lseek(swap_fd, page_num * PAGE_SIZE, SEEK_SET) == -1) {
            perror("Failed to seek to page in swap file");
            exit(1);
        }
        if (read(swap_fd, physical_memory + (free_frame * PAGE_SIZE), PAGE_SIZE) != PAGE_SIZE) {
            perror("Failed to read page from swap file");
            exit(1);
        }
        // Update page table
        page_table[page_num] = free_frame;
        // Copy data to new page
        memcpy(physical_memory + (free_frame * PAGE_SIZE), buf, PAGE_SIZE);
    }
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

    // Write to page 1
    char data2[] = "Goodbye!";
    write_page(1, data2);

    // Read from page 1
    read_page(1, buf);
    printf("Page 1 contains: %s\n", buf);

    // Allocate and write to page 2
    char* data3 = allocate_page();
    strcpy(data3, "This is page 2!");
    write_page(2, data3);

    // Read from page 2
    read_page(2, buf);
    printf("Page 2 contains: %s\n", buf);

    // Deallocate page 2
    deallocate_page(2);

    // Clean up
    free(physical_memory);
    close(swap_fd);
    unlink(SWAP_FILE);

    return 0;
}


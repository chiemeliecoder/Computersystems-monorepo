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

    // Allocate 5 pages and write to them
    char* pages[5];
    for (int i = 0; i < 5; i++) {
        pages[i] = allocate_page();
        sprintf(pages[i], "This is page %d!", i+3);
        write_page(i+3, pages[i]);
    }

    // Read from the allocated pages
    for (int i = 0; i < 5; i++) {
        read_page(i+3, buf);
        printf("Page %d contains: %s\n", i+3, buf);
    }

    // Free the allocated pages
    for (int i = 0; i < 5; i++) {
        deallocate_page(i+3);
    }

    // Clean up
    free(physical_memory);
    close(swap_fd);
    unlink(SWAP_FILE);

    return 0;
}

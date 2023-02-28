#include <stdio.h>
#include <stdlib.h>

#define MAX_FRAMES 3

int main() {
    int reference_string[] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};  // Reference string
    int num_pages = sizeof(reference_string) / sizeof(reference_string[0]);  // Number of pages in reference string
    int page_table[MAX_FRAMES];  // Page table to store pages in memory
    int num_faults = 0;  // Number of page faults
    int oldest_page_index = 0;  // Index of the oldest page in memory

    // Initialize page table to -1 (no page in memory)
    for (int i = 0; i < MAX_FRAMES; i++) {
        page_table[i] = -1;
    }

    // Loop over reference string and handle page faults
    for (int i = 0; i < num_pages; i++) {
        int page_num = reference_string[i];
        int page_index = -1;

        // Check if page is already in memory
        for (int j = 0; j < MAX_FRAMES; j++) {
            if (page_table[j] == page_num) {
                page_index = j;
                break;
            }
        }

        // If page is not in memory, replace the oldest page with the new page
        if (page_index == -1) {
            page_table[oldest_page_index] = page_num;
            num_faults++;
            oldest_page_index = (oldest_page_index + 1) % MAX_FRAMES;  // Update oldest page index
        }

        // Print current state of page table
        printf("Page table after reference to page %d: ", page_num);
        for (int j = 0; j < MAX_FRAMES; j++) {
            if (page_table[j] == -1) {
                printf("- ");
            } else {
                printf("%d ", page_table[j]);
            }
        }
        printf("\n");
    }

    printf("Number of page faults: %d\n", num_faults);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>

#define PAGE_TABLE_SIZE 256  // Size of the page table
#define PAGE_SIZE_4K 4096
#define PAGE_SIZE_8K 8192

unsigned int page_table[PAGE_TABLE_SIZE];

// Function to convert decimal to binary
void decimal_to_binary(unsigned int num, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        printf("%u", (num >> i) & 1);
    }
    printf("\n");
}

// Function to display page table
void display_page_table(unsigned int page_size) {
    printf("Page Table:\n");
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        if (page_table[i] != 0xFFFFFFFF) {
            printf("Page %d: 0x%05X\n", i, page_table[i]);
        }
    }
}

// Function to initialize page table
void initialize_page_table(unsigned int page_size) {
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        page_table[i] = 0xFFFFFFFF;  // Initialize with invalid address
    }
    unsigned int frame_address = 0x00000000;
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        if (frame_address < 0x00080000) {  // Physical memory size of 512K bytes
            page_table[i] = frame_address;
            frame_address += page_size;
        }
    }
}

int main() {
    unsigned int page_size;
    unsigned int virtual_address, physical_address;
    unsigned int page_number, offset;

    printf("Enter page size (4096 for 4K or 8192 for 8K): ");
    scanf("%u", &page_size);

    if (page_size != PAGE_SIZE_4K && page_size != PAGE_SIZE_8K) {
        printf("Invalid page size.\n");
        return 1;
    }

    printf("Page size in binary: ");
    decimal_to_binary(page_size, 16);
    printf("Page size in hexadecimal: 0x%X\n", page_size);

    initialize_page_table(page_size);
    display_page_table(page_size);

    for (int i = 0; i < 10; i++) {
        printf("Enter a virtual memory address (in hexadecimal): ");
        scanf("%x", &virtual_address);

        page_number = virtual_address / page_size;
        offset = virtual_address % page_size;

        if (page_table[page_number] == 0xFFFFFFFF) {
            printf("Virtual address 0x%X is currently on Disk.\n", virtual_address);
        } else {
            physical_address = page_table[page_number] + offset;
            printf("Virtual address 0x%X maps to physical address 0x%X\n", virtual_address, physical_address);
        }
    }

    return 0;
}

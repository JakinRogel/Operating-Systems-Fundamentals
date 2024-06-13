#include <stdio.h>

#define VIRTUAL_MEMORY_SIZE 0x000FFFFF
#define PHYSICAL_MEMORY_SIZE 0x7FFFF

// Function to convert decimal to 16-bit binary and display
void decimalToBinary(int n) {
    for (int i = 15; i >= 0; i--) {
        int bit = (n >> i) & 1;
        printf("%d", bit);
    }
    printf("\n");
}

// Function to display a number in hexadecimal
void displayHex(int n) {
    printf("%X\n", n);
}

// Function to emulate a page table
void populatePageTable(int pageSize) {
    int pageTableSize = VIRTUAL_MEMORY_SIZE / pageSize;

    printf("Page Table Entries for %d-byte pages:\n", pageSize);

    // Print each page number with its PA (Physical Address)
    for (int i = 0; i < pageTableSize; i++) {
        printf("Page %d --> Physical Address: %X\n", i, i * pageSize);
    }
}

// Function to convert virtual memory address to physical memory address
int convertToPhysicalAddress(int virtualAddress, int pageSize) {
    int pageOffsetMask = pageSize - 1;
    int pageOffset = virtualAddress & pageOffsetMask;
    int pageIndex = virtualAddress / pageSize;

    if (pageIndex <= VIRTUAL_MEMORY_SIZE / pageSize) {
        return (pageIndex * pageSize) + pageOffset;
    } else {
        return -1; // Not enough physical memory, currently on disk
    }
}

int main() {
    int pageSize;

    // Prompt user for page size
    printf("Enter page size (4095 or 8191): ");
    scanf("%d", &pageSize);

    if (pageSize != 4095 && pageSize != 8191) {
        printf("Invalid page size.\n");
        return 1;
    }

    // Display page size as 16-bit binary and hexadecimal
    printf("Page Size (16-bit binary): ");
    decimalToBinary(pageSize);
    printf("Page Size (Hexadecimal): ");
    displayHex(pageSize);

    // Emulate page table
    populatePageTable(pageSize);

    // Convert virtual memory addresses to physical memory addresses
    for (int i = 0; i < 10; i++) {
        int virtualAddress;
        printf("\nEnter virtual memory address (Hexadecimal): ");
        scanf("%X", &virtualAddress);

        if (virtualAddress < 0x00000000 || virtualAddress > 0x000FFFFF) {
            printf("Invalid virtual memory address.\n");
            continue;
        }

        printf("Input Virtual Memory Address: %X\n", virtualAddress);

        int physicalAddress = convertToPhysicalAddress(virtualAddress, pageSize);
        if (physicalAddress != -1) {
            printf("Resultant Physical Memory Address: %X\n", physicalAddress);
        } else {
            printf("Result: Currently on Disk.\n");
        }
    }

    return 0;
}

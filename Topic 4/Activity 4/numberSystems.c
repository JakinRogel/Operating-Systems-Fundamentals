#include <stdio.h>

// Function prototypes
void printBinary(int num);
void printHexadecimal(int num);

int main() {
    int num;

    // Prompt the user for input
    printf("Enter a number between 0 and 1000: ");
    scanf("%d", &num);

    // Convert to binary and print
    printf("Binary: ");
    printBinary(num);

    // Convert to hexadecimal and print
    printf("Hexadecimal: ");
    printHexadecimal(num);

    // Perform bitwise operations
    int shifted = num << 10;
    int masked = shifted & ~0x3FF;
    int result = masked | 0x3FF;

    // Print results of bitwise operations
    printf("\nAfter shifting 10 bits to the left, masking and OR-ing with 0x3FF:\n");
    printf("Decimal: %d\n", result);
    printf("Hexadecimal: ");
    printHexadecimal(result);
    printf("Binary: ");
    printBinary(result);

    return 0;
}

void printBinary(int num) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }
    printf("\n");
}

void printHexadecimal(int num) {
    printf("0x%X\n", num);
}

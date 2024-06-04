#include <stdio.h>

int main() {
    // Variable declarations
    int num1, num2, sum;
    
    // Getting input from a user
    printf("Enter two numbers: ");
    scanf("%d %d", &num1, &num2);
    
    // Operators
    sum = num1 + num2;
    
    // Printing formatted output
    printf("The sum of %d and %d is %d\n", num1, num2, sum);
    
    return 0;
}


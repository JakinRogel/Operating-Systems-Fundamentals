#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Define a mutex for synchronizing access to the balance
pthread_mutex_t mutex;
int balance = 1000; // Shared resource

// Function to deposit money into the account
void* deposit(void* arg) {
    int amount = *((int*)arg);
    pthread_mutex_lock(&mutex); // Lock the mutex before accessing the balance
    int temp = balance;
    sleep(1); // Simulate processing time
    temp += amount;
    balance = temp;
    printf("Deposited %d, New Balance: %d\n", amount, balance);
    pthread_mutex_unlock(&mutex); // Unlock the mutex after updating the balance
    return NULL;
}

// Function to withdraw money from the account
void* withdraw(void* arg) {
    int amount = *((int*)arg);
    pthread_mutex_lock(&mutex); // Lock the mutex before accessing the balance
    int temp = balance;
    sleep(1); // Simulate processing time
    if (temp >= amount) { // Check if there are sufficient funds
        temp -= amount;
        balance = temp;
        printf("Withdrew %d, New Balance: %d\n", amount, balance);
    } else {
        printf("Withdrawal of %d failed, Insufficient Balance: %d\n", amount, balance);
    }
    pthread_mutex_unlock(&mutex); // Unlock the mutex after updating the balance
    return NULL;
}

int main() {
    pthread_t t1, t2; // Thread identifiers
    int dep = 500, wd = 300; // Amounts to deposit and withdraw

    printf("Initial Balance: %d\n", balance);
    pthread_mutex_init(&mutex, NULL); // Initialize the mutex

    // Create threads for deposit and withdraw operations
    pthread_create(&t1, NULL, deposit, &dep);
    pthread_create(&t2, NULL, withdraw, &wd);

    // Wait for both threads to finish
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex); // Destroy the mutex
    return 0;
}
//ALL CODE DONE BY JAKIN ROGEL
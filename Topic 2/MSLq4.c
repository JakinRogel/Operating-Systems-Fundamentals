#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Global variable representing the bank balance
long long bank_balance = 0;

// Semaphore for synchronizing access to the bank balance
sem_t semaphore;

// Function to simulate bank deposit transactions
void* deposit(void* arg) {
    for (long long i = 0; i < 1000000; ++i) {
        // Wait until semaphore is available
        sem_wait(&semaphore);
        bank_balance += 1;
        // Signal semaphore after updating the bank balance
        sem_post(&semaphore);
    }
    return NULL;
}

int main() {
    // Initialize the semaphore
    if (sem_init(&semaphore, 0, 1) != 0) {
        printf("Semaphore initialization failed\n");
        return 1;
    }

    // Create two threads
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, deposit, NULL);
    pthread_create(&thread2, NULL, deposit, NULL);

    // Wait for both threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Destroy the semaphore
    sem_destroy(&semaphore);

    // Print the final bank balance
    printf("Final bank balance: %lld\n", bank_balance);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Function for thread 1
void* thread1_function(void* arg) {
    for (int i = 0; i < 10; i++) {
        printf("Thread 1: Message %d\n", i + 1);
        sleep(1); // Sleep for 1 second
    }
    return NULL;
}

// Function for thread 2
void* thread2_function(void* arg) {
    for (int i = 0; i < 10; i++) {
        printf("Thread 2: Message %d\n", i + 1);
        sleep(2); // Sleep for 2 seconds
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // Create thread 1
    if (pthread_create(&thread1, NULL, thread1_function, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }

    // Create thread 2
    if (pthread_create(&thread2, NULL, thread2_function, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }

    // Wait for thread 1 to finish
    if (pthread_join(thread1, NULL) != 0) {
        perror("pthread_join");
        return 1;
    }

    // Wait for thread 2 to finish
    if (pthread_join(thread2, NULL) != 0) {
        perror("pthread_join");
        return 1;
    }

    return 0;
}

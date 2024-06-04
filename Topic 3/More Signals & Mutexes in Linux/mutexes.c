#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Global variables
int counter = 0;       // Shared counter variable
int misses = 0;        // Counter for the number of times monitor thread fails to lock the mutex
pthread_mutex_t counter_mutex; // Mutex to protect the counter variable

// Function executed by the counter thread
void* counterThread(void* duration) {
    int run_time = *((int*)duration) * 60; // Convert duration from minutes to seconds
    for (int i = 0; i < run_time; i++) {
        pthread_mutex_lock(&counter_mutex); // Lock the mutex before accessing the counter
        printf("Counter Thread: Locked mutex for Counter so it can be incremented\n");
        counter++; // Increment the counter
        printf("Counter Thread: Counter value incremented to %d\n", counter);
        pthread_mutex_unlock(&counter_mutex); // Unlock the mutex after incrementing the counter
        printf("Counter Thread: Unlocked mutex for Counter since we are done with the Counter\n");
        sleep(1); // Sleep for a second to allow monitor thread to run
    }
    printf("Final Counter: %d\n", counter); // Print the final value of the counter
    pthread_exit(NULL); // Exit the thread
}

// Function executed by the monitor thread
void* monitorThread(void* duration) {
    int run_time = *((int*)duration) * 60; // Convert duration from minutes to seconds
    for (int i = 0; i < run_time; i += 3) {
        if (pthread_mutex_trylock(&counter_mutex) == 0) { // Try to lock the mutex
            printf("   Monitor Thread: the trylock will lock the mutex so we can safely read the Counter\n");
            printf("   Monitor Thread: The Counter from Monitor is %d\n", counter); // Read the counter value
            pthread_mutex_unlock(&counter_mutex); // Unlock the mutex after reading the counter
            printf("   Monitor Thread: will now unlock the mutex since we are done with the Counter\n");
        } else {
            misses++; // Increment the misses counter if the mutex is already locked
        }
        sleep(3); // Sleep for 3 seconds before the next check
    }
    printf("Final Monitor Thread missed was %d times.\n", misses); // Print the total number of misses
    pthread_exit(NULL); // Exit the thread
}

int main() {
    pthread_t counter_tid, monitor_tid; // Thread identifiers
    int duration;

    printf("Enter duration in minutes: "); // Prompt user for duration
    scanf("%d", &duration); // Read duration from user

    pthread_mutex_init(&counter_mutex, NULL); // Initialize the mutex

    pthread_create(&counter_tid, NULL, counterThread, &duration); // Create the counter thread
    pthread_create(&monitor_tid, NULL, monitorThread, &duration); // Create the monitor thread

    pthread_join(counter_tid, NULL); // Wait for the counter thread to finish
    pthread_join(monitor_tid, NULL); // Wait for the monitor thread to finish

    printf("Misses: %d\n", misses); // Print the final number of misses

    pthread_mutex_destroy(&counter_mutex); // Destroy the mutex

    return 0; // Exit the program
}
//ALL CODE DONE BY JAKIN ROGEL
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#define LOG_FILE "activity_log.txt" // Define the log file name

// Global variables
pthread_mutex_t mutex; // Mutex for protecting shared resource
pthread_mutex_t log_mutex; // Mutex for protecting log file access
pthread_cond_t cond; // Condition variable for thread synchronization
int resource_available = 1; // Flag indicating if the resource is available
FILE* log_file; // Log file pointer

// Structure to pass data to threads
typedef struct {
    int thread_id; // Thread ID
} thread_data_t;

// Thread function
void* threadFunction(void* arg) {
    thread_data_t* data = (thread_data_t*) arg; // Cast argument to thread_data_t
    int thread_id = data->thread_id; // Get thread ID
    int acquired = 0; // Flag to indicate if resource was acquired

    for (int i = 0; i < 4; ++i) { // Loop to simulate multiple attempts to access the resource
        pthread_mutex_lock(&log_mutex); // Lock log mutex
        fprintf(log_file, "Thread %d: Trying to access the file...\n", thread_id); // Log attempt to access file
        pthread_mutex_unlock(&log_mutex); // Unlock log mutex

        pthread_mutex_lock(&mutex); // Lock resource mutex
        while (!resource_available) { // While resource is not available
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts); // Get current time
            ts.tv_sec += 3; // Set timeout for 3 seconds

            int res = pthread_cond_timedwait(&cond, &mutex, &ts); // Wait for condition with timeout
            if (res == ETIMEDOUT) { // If timed out
                pthread_mutex_lock(&log_mutex); // Lock log mutex
                fprintf(log_file, "Thread %d: Timed out. Failed to acquire the file. Retrying...\n", thread_id); // Log timeout
                pthread_mutex_unlock(&log_mutex); // Unlock log mutex
                acquired = 0; // Reset acquired flag
                break; // Exit while loop to retry
            } else { // If signaled
                pthread_mutex_lock(&log_mutex); // Lock log mutex
                fprintf(log_file, "Thread %d: Waiting for file access...\n", thread_id); // Log waiting
                pthread_mutex_unlock(&log_mutex); // Unlock log mutex
            }
        }

        if (resource_available) { // If resource is available
            resource_available = 0; // Mark resource as not available
            acquired = 1; // Set acquired flag

            pthread_mutex_lock(&log_mutex); // Lock log mutex
            fprintf(log_file, "Thread %d: file acquired successfully!\n", thread_id); // Log successful acquisition
            pthread_mutex_unlock(&log_mutex); // Unlock log mutex
        }

        pthread_mutex_unlock(&mutex); // Unlock resource mutex

        if (acquired) { // If resource was acquired
            int work_time = rand() % 6 + 1; // Generate random work time
            pthread_mutex_lock(&log_mutex); // Lock log mutex
            fprintf(log_file, "Thread %d: Working for %d seconds...\n", thread_id, work_time); // Log work time
            pthread_mutex_unlock(&log_mutex); // Unlock log mutex

            sleep(work_time); // Simulate work with the resource

            pthread_mutex_lock(&mutex); // Lock resource mutex
            resource_available = 1; // Mark resource as available
            pthread_cond_signal(&cond); // Signal other threads waiting for the resource
            pthread_mutex_unlock(&mutex); // Unlock resource mutex

            pthread_mutex_lock(&log_mutex); // Lock log mutex
            fprintf(log_file, "Thread %d: Released the file.\n", thread_id); // Log resource release
            pthread_mutex_unlock(&log_mutex); // Unlock log mutex

            acquired = 0; // Reset acquired flag
            sleep(3); // Give time for other thread to acquire the resource
        }
    }

    pthread_mutex_lock(&log_mutex); // Lock log mutex
    fprintf(log_file, "Thread %d finished...\n", thread_id); // Log thread completion
    pthread_mutex_unlock(&log_mutex); // Unlock log mutex

    pthread_exit(NULL); // Exit thread
}

int main() {
    // Open log file in append mode
    log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL) { // Check if log file opened successfully
        perror("Failed to open log file"); // Print error if failed
        exit(EXIT_FAILURE); // Exit program
    }
    fprintf(log_file, "\n\n         ***NEW LOG*** \n"); // Log new session start
    // Seed the random number generator
    srand(time(NULL));

    // Initialize mutex and condition variable
    if (pthread_mutex_init(&mutex, NULL) != 0) { // Initialize resource mutex
        perror("Mutex initialization failed"); // Print error if failed
        exit(EXIT_FAILURE); // Exit program
    }

    if (pthread_mutex_init(&log_mutex, NULL) != 0) { // Initialize log mutex
        perror("Log mutex initialization failed"); // Print error if failed
        exit(EXIT_FAILURE); // Exit program
    }

    if (pthread_cond_init(&cond, NULL) != 0) { // Initialize condition variable
        perror("Condition variable initialization failed"); // Print error if failed
        exit(EXIT_FAILURE); // Exit program
    }

    // Create threads
    pthread_t threads[2]; // Array to hold thread identifiers
    thread_data_t thread_data[2]; // Array to hold thread data structures
    for (int i = 0; i < 2; ++i) { // Loop to create threads
        thread_data[i].thread_id = i + 1; // Set thread ID
        if (pthread_create(&threads[i], NULL, threadFunction, &thread_data[i]) != 0) { // Create thread
            perror("Thread creation failed"); // Print error if failed
            exit(EXIT_FAILURE); // Exit program
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < 2; ++i) { // Loop to join threads
        pthread_join(threads[i], NULL); // Wait for thread to finish
    }

    // Clean up
    pthread_mutex_lock(&log_mutex); // Lock log mutex
    fprintf(log_file, "Clean up started........\n"); // Log cleanup start
    pthread_mutex_unlock(&log_mutex); // Unlock log mutex

    // Destroy mutexes and condition variable
    if (pthread_mutex_destroy(&mutex) != 0) { // Destroy resource mutex
        perror("Mutex destruction failed\n"); // Print error if failed
        exit(EXIT_FAILURE); // Exit program
    }

    if (pthread_mutex_destroy(&log_mutex) != 0) { // Destroy log mutex
        perror("Log mutex destruction failed\n"); // Print error if failed
        exit(EXIT_FAILURE); // Exit program
    }

    if (pthread_cond_destroy(&cond) != 0) { // Destroy condition variable
        perror("Condition variable destruction failed\n"); // Print error if failed
        exit(EXIT_FAILURE); // Exit program
    }

    // Close log file
    fclose(log_file); // Close log file

    // Log clean up finished
    log_file = fopen(LOG_FILE, "a"); // Reopen log file to log cleanup completion
    if (log_file != NULL) { // Check if log file opened successfully
        fprintf(log_file, "Clean up finished........\n"); // Log cleanup completion
        fprintf(log_file, "Program complete.\n"); // Log program completion
        fclose(log_file); // Close log file
    } else { // If failed to reopen log file
        perror("Failed to reopen log file"); // Print error
    }

    return 0; // Return success
}

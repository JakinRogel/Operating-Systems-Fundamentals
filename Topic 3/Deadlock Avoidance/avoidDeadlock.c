#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#define LOG_FILE "activity_log.txt"

// Global variables
pthread_mutex_t mutex;
FILE* log_file;

// Structure to pass data to threads
typedef struct {
    int thread_id;
} thread_data_t;

// Signal handler for timeout
void signalHandler(int signum) {
    fprintf(log_file, "Thread %ld: Timed out. Releasing the file.\n", pthread_self());
    pthread_mutex_unlock(&mutex); // Release the mutex
    pthread_exit(NULL); // Exit the thread
}

// Thread function
void* threadFunction(void* arg) {
    thread_data_t* data = (thread_data_t*) arg;
    int thread_id = data->thread_id;
    fprintf(log_file, "\n\n\n           *** NEW LOG ***\n");
    for (int i = 0; i < 4; ++i) {
        fprintf(log_file, "Thread %d: Trying to access the file...\n", thread_id);

        signal(SIGALRM, signalHandler); // Set up signal handler for SIGALRM

        if (pthread_mutex_lock(&mutex) != 0) {
            perror("pthread_mutex_lock");
            pthread_exit(NULL);
        }

        fprintf(log_file, "Thread %d: file acquired successfully!\n", thread_id);
        alarm(3); // Set a timer of 3 seconds
        int work_time = rand() % 6 + 1; // Generate random work time
        fprintf(log_file, "Thread %d: Working for %d seconds...\n", thread_id, work_time);
        sleep(work_time); // Simulate some work with the resource
        pthread_mutex_unlock(&mutex); // Release the mutex
        alarm(0); // Cancel the alarm
    }
    fprintf(log_file, "Thread %d finished...\n", thread_id);
    pthread_exit(NULL);
}

int main() {
    // Open log file in append mode
    log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }

    // Seed the random number generator
    srand(time(NULL));

    // Initialize mutex
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }

    // Create threads
    pthread_t threads[2];
    thread_data_t thread_data[2];
    for (int i = 0; i < 2; ++i) {
        thread_data[i].thread_id = i + 1;
        if (pthread_create(&threads[i], NULL, threadFunction, &thread_data[i]) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < 2; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Clean up
    fprintf(log_file, "Clean up started........\n");
    // Destroy mutex
    if (pthread_mutex_destroy(&mutex) != 0) {
        perror("Mutex destruction failed\n");
        exit(EXIT_FAILURE);
    }
    // Close log file
    fclose(log_file);
    fprintf(log_file, "Clean up finished........\n");

    fprintf(log_file, "Program complete.\n");
    return 0;
}

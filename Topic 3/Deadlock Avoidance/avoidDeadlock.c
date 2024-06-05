#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#define LOG_FILE "activity_log.txt"

// Global variables
pthread_mutex_t mutex;
pthread_mutex_t log_mutex;
pthread_cond_t cond;
int resource_available = 1;
FILE* log_file;

// Structure to pass data to threads
typedef struct {
    int thread_id;
} thread_data_t;

// Thread function
void* threadFunction(void* arg) {
    thread_data_t* data = (thread_data_t*) arg;
    int thread_id = data->thread_id;
    int acquired = 0;

    fprintf(log_file, "\n\n\n           *** NEW LOG ***\n");

    for (int i = 0; i < 4; ++i) {
        pthread_mutex_lock(&log_mutex);
        fprintf(log_file, "Thread %d: Trying to access the file...\n", thread_id);
        pthread_mutex_unlock(&log_mutex);

        pthread_mutex_lock(&mutex);
        while (!resource_available) {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 3;

            int res = pthread_cond_timedwait(&cond, &mutex, &ts);
            if (res == ETIMEDOUT) {
                pthread_mutex_lock(&log_mutex);
                fprintf(log_file, "Thread %d: Timed out. Releasing the file.\n", thread_id);
                pthread_mutex_unlock(&log_mutex);
                break;
            }
        }

        if (resource_available) {
            resource_available = 0;
            acquired = 1;
        }
        pthread_mutex_unlock(&mutex);

        if (acquired) {
            pthread_mutex_lock(&log_mutex);
            fprintf(log_file, "Thread %d: file acquired successfully!\n", thread_id);
            pthread_mutex_unlock(&log_mutex);

            int work_time = rand() % 6 + 1; // Generate random work time
            pthread_mutex_lock(&log_mutex);
            fprintf(log_file, "Thread %d: Working for %d seconds...\n", thread_id, work_time);
            pthread_mutex_unlock(&log_mutex);

            sleep(work_time); // Simulate some work with the resource

            pthread_mutex_lock(&mutex);
            resource_available = 1;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mutex);

            pthread_mutex_lock(&log_mutex);
            fprintf(log_file, "Thread %d: Released the file.\n", thread_id);
            pthread_mutex_unlock(&log_mutex);

            acquired = 0; // Reset acquired flag
        }
    }

    pthread_mutex_lock(&log_mutex);
    fprintf(log_file, "Thread %d finished...\n", thread_id);
    pthread_mutex_unlock(&log_mutex);

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

    // Initialize mutex and condition variable
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&log_mutex, NULL) != 0) {
        perror("Log mutex initialization failed");
        exit(EXIT_FAILURE);
    }

    if (pthread_cond_init(&cond, NULL) != 0) {
        perror("Condition variable initialization failed");
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
    pthread_mutex_lock(&log_mutex);
    fprintf(log_file, "Clean up started........\n");
    pthread_mutex_unlock(&log_mutex);

    // Destroy mutexes and condition variable
    if (pthread_mutex_destroy(&mutex) != 0) {
        perror("Mutex destruction failed\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_destroy(&log_mutex) != 0) {
        perror("Log mutex destruction failed\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_cond_destroy(&cond) != 0) {
        perror("Condition variable destruction failed\n");
        exit(EXIT_FAILURE);
    }

    // Close log file
    fclose(log_file);

    pthread_mutex_lock(&log_mutex);
    fprintf(log_file, "Clean up finished........\n");
    fprintf(log_file, "Program complete.\n");
    pthread_mutex_unlock(&log_mutex);

    return 0;
}

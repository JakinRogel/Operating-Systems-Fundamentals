#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>
#include <errno.h>

#define LOG_FILE "activity_log.txt"

// Global variables
sem_t* semaphore;
FILE* log_file;

void signalHandler(int signum) {
    fprintf(log_file, "Process %d: Timed out. Releasing the file.\n", getpid());
    sem_post(semaphore); // Release the semaphore
}

void childProcess() {
    // Child process tries to access the resource
    for (int i = 0; i < 4; ++i) {
        fprintf(log_file, "Child Process (%d): Trying to access the file...\n", getpid());
        signal(SIGALRM, signalHandler); // Set up signal handler for SIGALRM
        

        if (sem_wait(semaphore) == -1) {
            if (errno == EINTR) {
                // Timer interrupted the wait operation, continue to next iteration
                continue;
            } else {
                perror("sem_wait");
                exit(EXIT_FAILURE);
            }
        }
        
        fprintf(log_file, "Child Process (%d): file acquired successfully!\n", getpid());
        alarm(3); // Set a timer of 5 seconds
        int work_time = rand() % 6 + 1; // Generate random work time
        fprintf(log_file, "Child Process (%d): Working for %d seconds...\n", getpid(), work_time);
        sleep(work_time); // Simulate some work with the resource
        sem_post(semaphore); // Release the semaphore
        alarm(0); // Cancel the alarm
    }
    fprintf(log_file, "Child Process finished...\n");
    exit(EXIT_SUCCESS); // Exit the child process
}

void parentProcess() {
    // Parent process tries to access the resource
    for (int i = 0; i < 4; ++i) {
        fprintf(log_file, "Parent Process (%d): Trying to access the file...\n", getpid());
        signal(SIGALRM, signalHandler); // Set up signal handler for SIGALRM
        

        if (sem_wait(semaphore) == -1) {
            if (errno == EINTR) {
                // Timer interrupted the wait operation, continue to next iteration
                continue;
            } else {
                perror("sem_wait");
                exit(EXIT_FAILURE);
            }
        }

        fprintf(log_file, "Parent Process (%d): file acquired successfully!\n", getpid());
        alarm(3); // Set a timer of 5 seconds
        int work_time = rand() % 6 + 1; // Generate random work time
        fprintf(log_file, "Parent Process (%d): Working for %d seconds...\n", getpid(), work_time);
        sleep(work_time); // Simulate some work with the resource
        sem_post(semaphore); // Release the semaphore
        alarm(0); // Cancel the alarm
    }
    fprintf(log_file, "Parent Process finished...\n");
    sleep(4);
    exit(EXIT_SUCCESS); // Exit the parent process
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

    // Create shared semaphore
    semaphore = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (semaphore == MAP_FAILED) {
        perror("Semaphore creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize semaphore
    if (sem_init(semaphore, 1, 1) != 0) {
        perror("Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }

    // Fork child process
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process
        childProcess();
    } else { // Parent process
        parentProcess();
    }

    // Clean up
    fprintf(log_file, "Clean up started........\n");
    // Clean up semaphore
    if (sem_destroy(semaphore) != 0) {
        perror("Semaphore destruction failed\n");
        exit(EXIT_FAILURE);
    }
    if (munmap(semaphore, sizeof(sem_t)) != 0) {
        perror("Semaphore memory unmapping failed\n");
        exit(EXIT_FAILURE);
    }
    // Close log file
    fclose(log_file);
    fprintf(log_file, "Clean up finished........\n");

    fprintf(log_file, "Program complete.\n");
    return 0;
}

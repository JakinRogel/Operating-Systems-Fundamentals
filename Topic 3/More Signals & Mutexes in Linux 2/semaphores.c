#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

// Shared semaphore pointer
sem_t *sem;
int child_pid;

// Signal handler for SIGTERM in child process
void sigterm_handler(int signum) {
    printf("Child process: Received SIGTERM signal. Releasing semaphore.\n");
    sem_post(sem); // Release the semaphore
    exit(0);
}

// Monitor thread function
void* monitorThread(void* arg) {
    sleep(10); // Wait for 10 seconds
    printf("Monitor thread: Attempting to acquire semaphore.\n");
    // Attempt to acquire the semaphore
    if (sem_trywait(sem) != 0) {
        // If unable to acquire, kill the child process
        printf("Monitor thread: Unable to get semaphore, killing child process.\n");
        kill(child_pid, SIGTERM); // Use SIGTERM instead of SIGKILL
    } else {
        // If acquired, release the semaphore
        printf("Monitor thread: Semaphore acquired.\n");
        sem_post(sem);
    }
    pthread_exit((void*)1); // Indicate semaphore acquisition failure
}

// Child process function
void childProcess() {
    // Register SIGTERM handler
    signal(SIGTERM, sigterm_handler);
    // Attempt to acquire semaphore
    sem_wait(sem);
    printf("Child process: Acquired semaphore, simulating long-running process.\n");
    sleep(60);  // Simulate stuck process
    // Release semaphore before exiting
    sem_post(sem);
    printf("Child process: Released semaphore.\n");
    exit(0);
}

int main() {
    // Allocate shared memory for semaphore
    sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sem == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Initialize semaphore for process sharing
    sem_init(sem, 1, 1);

    // Fork a child process
    if ((child_pid = fork()) == 0) {
        // Child process
        childProcess();
    } else {
        // Parent process
        pthread_t monitor_tid;
        void *thread_result;
        // Create monitor thread
        pthread_create(&monitor_tid, NULL, monitorThread, NULL);
        pthread_join(monitor_tid, &thread_result);
        
        // Check if the thread was unable to acquire the semaphore
        if (thread_result == (void*)1) {
            printf("Parent process: Child process was killed. Attempting to acquire semaphore again.\n");
            // Wait for the child process to be killed
            int status;
            waitpid(child_pid, &status, 0);
            // Attempt to acquire the semaphore again
            if (sem_trywait(sem) == 0) {
                printf("Parent process: Successfully acquired the semaphore after killing child process.\n");
                sem_post(sem);
            } else {
                printf("Parent process: Unable to acquire the semaphore even after killing child process.\n");
            }
        }

        // Destroy semaphore and clean up
        sem_destroy(sem);
        munmap(sem, sizeof(sem_t));  // Unmap the shared memory
        printf("Parent process finished.\n");
    }

    return 0;
}
//ALL CODE DONE BY JAKIN ROGEL
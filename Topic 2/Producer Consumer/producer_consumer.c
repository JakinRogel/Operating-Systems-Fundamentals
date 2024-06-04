#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>

#define BUFFER_SIZE 10

// Function prototype for generating a random number
int generateRandomNumber();

// Structure for circular buffer
struct CIRCULAR_BUFFER {
    int count;                // Number of elements in the buffer
    int lower;                // Index of the lower bound of the buffer
    int upper;                // Index of the upper bound of the buffer
    int buffer[BUFFER_SIZE];  // Array to hold buffer elements
};

struct CIRCULAR_BUFFER *buffer = NULL;
sem_t *empty; // Semaphore to signal when buffer is not empty
sem_t *full;  // Semaphore to signal when buffer is not full

void producer(int pid);
void consumer(int pid);
void put(int value);
int get();

int main() {
    // Create shared memory for circular buffer
    buffer = (struct CIRCULAR_BUFFER*)mmap(0, sizeof(struct CIRCULAR_BUFFER), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    buffer->count = 0;
    buffer->lower = 0;
    buffer->upper = 0;

    // Initialize semaphores
    empty = mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    full = mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(empty, 1, BUFFER_SIZE); // Initialize to BUFFER_SIZE, indicating buffer is initially empty
    sem_init(full, 1, 0);            // Initialize to 0, indicating buffer is initially full

    // Fork a new process
    pid_t pid = fork();
    if (pid == -1) {
        perror("Can't fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process runs the consumer
        consumer(getpid());
    } else {
        // Parent process runs the producer
        producer(getpid());
        wait(NULL); // Wait for child to terminate
    }

    // Clean up
    munmap(buffer, sizeof(struct CIRCULAR_BUFFER));
    sem_destroy(empty);
    sem_destroy(full);

    return 0;
}

void producer(int pid) {
    int value = 0;
    while (1) {
        if (buffer->count < 10) {
            // Produce an item and add it to the buffer
            printf("\n\n*BUFFER-COUNT-PARENT: %d*\n", buffer->count);
            printf("Parent process: %d\n", pid);
            put(value);
            printf("Entered Value: %d\n", value);
            sleep(generateRandomNumber()); // Simulate work
            value++; // Increment value after putting it into the buffer
        } else {
            int sem_value;
            sem_getvalue(empty, &sem_value); // Get the current value of the empty semaphore
            printf("BUFFER IS FULL, empty semaphore value: %d\n", sem_value);
            sem_wait(empty); // Wait until buffer is not full
        }
        sem_post(full); // Signal that buffer is not empty
    }
}

void consumer(int pid) {
    while (1) {
        if (buffer->count > 0) {
            // Consume an item from the buffer
            printf("\n\n*BUFFER-COUNT-CHILD: %d*\n", buffer->count);
            printf("Child process: %d\n", pid);
            int value = get();
            printf("Value Got: %d\n", value);
            sleep(generateRandomNumber() + 1); // Simulate work
        } else {
            // Buffer is empty, wait until it has items
            printf("BUFFER IS EMPTY \n");
            sem_wait(full); // Wait until buffer is not empty
        }
        sem_post(empty); // Signal that buffer has available space
    }
}

void put(int value) {
    // Add an item to the buffer
    buffer->buffer[buffer->upper] = value;
    buffer->upper = (buffer->upper + 1) % BUFFER_SIZE;
    buffer->count++;
}

int get() {
    // Remove an item from the buffer
    int value = buffer->buffer[buffer->lower];
    buffer->lower = (buffer->lower + 1) % BUFFER_SIZE;
    buffer->count--;
    return value;
}

int generateRandomNumber() {
    // Generates a random number between 1 and 3
    return (rand() % 3) + 1;
}

//ALL CODE DONE BY JAKIN ROGEL

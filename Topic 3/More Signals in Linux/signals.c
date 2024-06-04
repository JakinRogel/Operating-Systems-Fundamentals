#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define BUFFER_SIZE 256

// Structure for the circular buffer
typedef struct {
    char buffer[BUFFER_SIZE];
    int head;
    int tail;
    int count;
} CircularBuffer;

// Global variable to indicate data ready for reading
volatile sig_atomic_t data_ready = 0;

// Initialize the circular buffer
void initBuffer(CircularBuffer *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
}

// Check if the circular buffer is full
int isBufferFull(CircularBuffer *cb) {
    return cb->count == BUFFER_SIZE;
}

// Check if the circular buffer is empty
int isBufferEmpty(CircularBuffer *cb) {
    return cb->count == 0;
}

// Write data to the circular buffer
void writeBuffer(CircularBuffer *cb, char data) {
    if (!isBufferFull(cb)) {
        cb->buffer[cb->head] = data;
        cb->head = (cb->head + 1) % BUFFER_SIZE;
        cb->count++;
    }
}

// Read data from the circular buffer
char readBuffer(CircularBuffer *cb) {
    char data = '\0';
    if (!isBufferEmpty(cb)) {
        data = cb->buffer[cb->tail];
        cb->tail = (cb->tail + 1) % BUFFER_SIZE;
        cb->count--;
    }
    return data;
}

// Signal handler for SIGUSR1
void signalHandler(int sig) {
    data_ready = 1;
}

// Parent process function
void parentProcess(CircularBuffer *cb, pid_t child_pid) {
    char message[] = "Jakin Rogel :)";
    printf("PARENT STARTING...\n");
    // Iterate through each character in the message
    for (int i = 0; i < strlen(message); i++) {
        // Wait until the buffer is not full
        while (isBufferFull(cb)) {
            sleep(1);  // Wait if the buffer is full
        }
        // Write the character to the circular buffer
        writeBuffer(cb, message[i]);
        // Signal the child process to read the data
        kill(child_pid, SIGUSR1);
        // Ensure child has time to process
        sleep(1);
    }
    // Wait until the buffer is not full
    while (isBufferFull(cb)) {
        sleep(1); // Wait if buffer is full
    }
    // Write null character to indicate end of message
    writeBuffer(cb, '\0');
    // Signal the child process to read the end of message
    kill(child_pid, SIGUSR1);
    // Wait for child process to finish
    wait(NULL);
    // Print message indicating parent process finished
    printf("Parent process finished.\n");
}

// Child process function
void childProcess(CircularBuffer *cb) {
    printf("CHILD STARTING...\n");
    // Set signal handler for SIGUSR1
    signal(SIGUSR1, signalHandler);
    char receivedChar;
    // Continue reading from the circular buffer until end of message
    while (1) {
        // Wait until data is ready to be read
        while (!data_ready) {
            sleep(1); // Wait for signal
        }
        data_ready = 0;
        // Read character from circular buffer
        receivedChar = readBuffer(cb);
        // Break loop if null character is received
        if (receivedChar == '\0') {
            break;
        }
        // Print the received character
        printf("%c", receivedChar);
        fflush(stdout);
    }
    // Print message indicating child process finished
    printf("\nChild process finished.\n");
}

int main() {
    CircularBuffer *cb;
    // Map shared memory for the circular buffer
    cb = mmap(NULL, sizeof(CircularBuffer), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (cb == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    // Initialize the circular buffer
    initBuffer(cb);
    // Set signal handler for SIGUSR1
    signal(SIGUSR1, signalHandler);
    // Fork a child process
    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(1);
    } else if (child_pid == 0) {
        // Child process
        childProcess(cb);
    } else {
        // Parent process
        parentProcess(cb, child_pid);
    }
    // Unmap shared memory
    if (munmap(cb, sizeof(CircularBuffer)) == -1) {
        perror("munmap");
        exit(1);
    }
    return 0;
}
//ALL CODE DONE BY JAKIN ROGEL
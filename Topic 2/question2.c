#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        // Fork failed
        fprintf(stderr, "Fork failed\n");
        return 1;
    } else if (pid == 0) {
        // Child process
        for (int i = 0; i < 10; i++) {
            printf("Child process: Message %d\n", i + 1);
            sleep(1);  // Sleep for 1 second
        }
        printf("Child process exiting...\n");
        exit(0);
    } else {
        // Parent process
        for (int i = 0; i < 10; i++) {
            printf("Parent process: Message %d\n", i + 1);
            sleep(2);  // Sleep for 2 seconds
        }
        printf("Parent process exiting...\n");
        wait(NULL);  // Wait for child process to finish
        exit(0);
    }
}

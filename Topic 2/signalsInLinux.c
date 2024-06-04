#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


// Signal number for WAKEUP
#define WAKEUP SIGUSR1

// Signal handler for the child process
void wakeup_handler(int sig) {
    // Do nothing, just wake up
}

int main() {
    pid_t pid;
    pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // Child process (consumer)
        struct sigaction sa;
        sa.sa_handler = wakeup_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        if (sigaction(WAKEUP, &sa, NULL) == -1) {
            perror("sigaction");
            exit(1);
        }

        // Sleep until WAKEUP signal is received
        pause();

        // Run loop for 20 iterations
        for (int i = 0; i < 20; i++) {
            printf("Child: Iteration %d\n", i + 1);
            sleep(1);
        }

        exit(1);
    } else {
        // Parent process (producer)
        for (int i = 0; i < 30; i++) {
            printf("Parent: Iteration %d\n", i + 1);
            sleep(1);

            if (i == 4) {
                // Send WAKEUP signal to child after 5 iterations
                kill(pid, WAKEUP);
            }
        }

        // Wait for the child process to finish
        wait(NULL);
        exit(1);
    }

    return 0;
}

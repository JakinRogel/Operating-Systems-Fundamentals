#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [arguments...]\n", argv[0]);
        return 1;
    }

    pid_t pid;

    // Prepare the arguments array for posix_spawn
    char **spawn_args = argv + 1;

    // Spawn the process
    if (posix_spawn(&pid, argv[1], NULL, NULL, spawn_args, environ) != 0) {
        perror("posix_spawn");
        return 1;
    }

    // Print the process ID
    printf("Spawned process ID: %d\n", (int)pid);

    // Wait for the process to end
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return 1;
    }

    // Check if the process ended normally
    if (WIFEXITED(status)) {
        printf("Process exited with status: %d\n", WEXITSTATUS(status));
    } else {
        printf("Process exited abnormally\n");
    }

    return 0;
}

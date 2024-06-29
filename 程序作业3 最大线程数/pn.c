#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int max_processes = 0;

    while (1) {
        pid_t pid = fork();

        if (pid < 0) {
            // Error forking
            break;
        } else if (pid == 0) {
            // Child process
            exit(0);
	  
        } else {
            // Parent process
            max_processes++;
        }
    }

    printf("Maximum concurrent processes: %d\n", max_processes);

    return 0;
}


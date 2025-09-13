#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

enum { READ, WRITE };

int main(int argc, char **argv)
{
	int pipes[2];
    pipe(pipes);

    if (0 != fork()) {
        // Padre
        if (0 != fork()) {
            // Padre
            close(pipes[READ]);
            close(pipes[WRITE]);
        } else {
            // Hijo (wc -l)
            close(pipes[WRITE]);
            dup2(pipes[READ], STDIN_FILENO);
            execlp("wc", "wc", "-l", NULL);
        }
    } else {
        // Hijo (ls -al)
        close(pipes[READ]);
        dup2(pipes[WRITE], STDOUT_FILENO);
        execlp("ls", "ls", "-al", NULL);
    }

    wait(NULL);
    wait(NULL);

	return 0;
}

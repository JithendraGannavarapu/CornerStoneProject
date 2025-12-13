#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <program>\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        execl(argv[1], argv[1], NULL);
    } else {
        int status;
        waitpid(pid, &status, 0);
        printf("Program is completed\n");
    }

    return 0;
}

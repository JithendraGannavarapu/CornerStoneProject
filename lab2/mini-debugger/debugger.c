#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <program>\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl(argv[1], argv[1], NULL);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFSTOPPED(status)) {
            printf("child process is stopped by using the signal %d. Signal is called as SIGTRAP\n", WSTOPSIG(status));
        }

        ptrace(PTRACE_CONT, pid, NULL, NULL);
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("The exit status of child is  %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}

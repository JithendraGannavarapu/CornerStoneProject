#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/ptrace.h>

long get_data(pid_t pid, long addr) {
    return ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
}

void put_data(pid_t pid, long addr, long data) {
    ptrace(PTRACE_POKEDATA, pid, addr, data);
}

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
            printf("child process is stopped by using the signal %d\n", WSTOPSIG(status));
        }
        long main_addr = 0x401136; 
        long original = get_data(pid, main_addr);
        long breakpoint = (original & ~0xff) | 0xcc;

        put_data(pid, main_addr, breakpoint);
        printf("Breakpoint set at main()\n");

        ptrace(PTRACE_CONT, pid, NULL, NULL);
        waitpid(pid, &status, 0);

        if (WIFSTOPPED(status)) {
            printf("Hit breakpoint at main()\n");
        }

        // int steps = 0;

        // while (1) {
        //     ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
        //     waitpid(pid, &status, 0);

        //     if (WIFSTOPPED(status)) {
        //         printf("Step %d: child stopped by signal %d\n",steps, WSTOPSIG(status));
        //         struct user_regs_struct regs;
        //         ptrace(PTRACE_GETREGS, pid, NULL, &regs);
        //         printf("Step %d: RIP = 0x%llx\n",steps, regs.rip);
        //         steps++;

        //         if (steps == 5) {
        //             printf("Stopping after 5 steps\n");
        //             break;
        //         }
        //     }

        //     if (WIFEXITED(status)) {
        //         printf("Child exited with status %d\n",
        //             WEXITSTATUS(status));
        //         break;
        //     }
        // }

    }

    return 0;
}

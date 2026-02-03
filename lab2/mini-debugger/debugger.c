#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

static long bp_addr = 0;
static long bp_saved_instr = 0;
static int  bp_enabled = 0;
static int  bp_hits = 0;

static pid_t child_pid = -1;
static int last_signal = 0;

void prompt() {
    printf("dbg> ");
    fflush(stdout);
}

void help() {
    printf(
        "Commands:\n"
        "  break <addr>    set breakpoint at address\n"
        "  clear           remove breakpoint\n"
        "  info bp         show breakpoint info\n"
        "  run / continue  continue execution\n"
        "  step            execute one instruction\n"
        "  regs            show registers\n"
        "  where           show stop reason and RIP\n"
        "  help            show this help\n"
        "  quit            exit debugger\n"
    );
}

void set_breakpoint(pid_t pid, long addr) {
    errno = 0;
    bp_saved_instr = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
    if (errno != 0) {
        perror("ptrace peek");
        return;
    }

    long trap = (bp_saved_instr & ~0xff) | 0xcc;
    ptrace(PTRACE_POKEDATA, pid, addr, trap);

    bp_addr = addr;
    bp_enabled = 1;
    bp_hits = 0;

    printf("Breakpoint set at 0x%lx\n", addr);
}

void clear_breakpoint(pid_t pid) {
    if (!bp_enabled) {
        printf("No breakpoint to clear\n");
        return;
    }
    ptrace(PTRACE_POKEDATA, pid, bp_addr, bp_saved_instr);
    printf("Breakpoint at 0x%lx removed\n", bp_addr);
    bp_enabled = 0;
}

void fix_rip(pid_t pid) {
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    regs.rip -= 1;                
    ptrace(PTRACE_SETREGS, pid, NULL, &regs);
}

void show_regs(pid_t pid) {
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    printf("Registers:\n");
    printf("  RIP: 0x%llx  RSP: 0x%llx\n", regs.rip, regs.rsp);
    printf("  RBP: 0x%llx  RAX: 0x%llx\n", regs.rbp, regs.rax);
    printf("  RBX: 0x%llx  RCX: 0x%llx\n", regs.rbx, regs.rcx);
    printf("  RDX: 0x%llx  RSI: 0x%llx\n", regs.rdx, regs.rsi);
    printf("  RDI: 0x%llx\n", regs.rdi);
}

void show_where(pid_t pid) {
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    printf("Stopped by signal %d at RIP = 0x%llx\n",
           last_signal, regs.rip);
}

void show_bp_info() {
    if (!bp_enabled) {
        printf("No breakpoint set\n");
        return;
    }
    printf("Breakpoint info:\n");
    printf("  Address : 0x%lx\n", bp_addr);
    printf("  Enabled : yes\n");
    printf("  Hits    : %d\n", bp_hits);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <target>\n", argv[0]);
        return 1;
    }

    child_pid = fork();

    if (child_pid == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl(argv[1], argv[1], NULL);
        perror("execl");
        exit(1);
    }

    int status;
    waitpid(child_pid, &status, 0);
    printf("We Loaded & Executed the target binary, We can debug it now..\n");

    char cmd[128];

    while (1) {
        prompt();

        if (!fgets(cmd, sizeof(cmd), stdin))
            break;

        cmd[strcspn(cmd, "\n")] = 0;

        long addr;

        if (sscanf(cmd, "break %lx", &addr) == 1) {
            if (bp_enabled)
                printf("Breakpoint already set\n");
            else
                set_breakpoint(child_pid, addr);
        }
        else if (strcmp(cmd, "clear") == 0) {
            clear_breakpoint(child_pid);
        }
        else if (strcmp(cmd, "info bp") == 0) {
            show_bp_info();
        }
        else if (strcmp(cmd, "step") == 0) {
            ptrace(PTRACE_SINGLESTEP, child_pid, NULL, NULL);
            waitpid(child_pid, &status, 0);

            if (WIFSTOPPED(status)) {
                last_signal = WSTOPSIG(status);
                show_regs(child_pid);
            }
            else if (WIFEXITED(status)) {
                printf("Program exited normally\n");
                break;
            }
        }
        else if (strcmp(cmd, "run") == 0 || strcmp(cmd, "continue") == 0) {
            struct user_regs_struct regs;
            ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);

            if (bp_enabled && regs.rip == bp_addr) {
                ptrace(PTRACE_SINGLESTEP, child_pid, NULL, NULL);
                waitpid(child_pid, &status, 0);

                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    printf("Program exited during step-over\n");
                    break;
                }

                long trap = (bp_saved_instr & ~0xff) | 0xcc;
                ptrace(PTRACE_POKEDATA, child_pid, bp_addr, trap);
            }

            ptrace(PTRACE_CONT, child_pid, NULL, NULL);
            waitpid(child_pid, &status, 0);

            if (WIFSTOPPED(status)) {
                last_signal = WSTOPSIG(status);
                
                if (last_signal == SIGTRAP) {
                    ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);
                    if (bp_enabled && regs.rip == bp_addr + 1) {
                        printf("Hit breakpoint at 0x%lx\n", bp_addr);
                        bp_hits++;
                        ptrace(PTRACE_POKEDATA, child_pid, bp_addr, bp_saved_instr);
                        fix_rip(child_pid);
                    } else {
                        printf("Stopped by SIGTRAP\n");
                    }
                } else {
                    printf("Stopped by signal %d\n", last_signal);
                }
            }
            else if (WIFEXITED(status)) {
                printf("Program exited normally with code %d\n", WEXITSTATUS(status));
                break;
            }
            else if (WIFSIGNALED(status)) {
                printf("Program terminated by signal %d\n", WTERMSIG(status));
                break;
            }
        }
        else if (strcmp(cmd, "regs") == 0) {
            show_regs(child_pid);
        }
        else if (strcmp(cmd, "where") == 0) {
            show_where(child_pid);
        }
        else if (strcmp(cmd, "help") == 0) {
            help();
        }
        else if (strcmp(cmd, "quit") == 0) {
            ptrace(PTRACE_DETACH, child_pid, NULL, NULL);
            printf("Debugger exited\n");
            break;
        }
        else {
            printf("Unknown command (type 'help')\n");
        }
    }

    return 0;
}
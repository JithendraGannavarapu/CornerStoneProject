#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_ARGS 64

int generate_tokens(char *str, char *argv[]) {
    int argc = 0;
    char *ptr = str;
    int in_quote = 0;
    int in_token = 0;
    char *write_ptr = str;

    while (*ptr != '\0' && argc < MAX_ARGS - 1) {
        if (isspace((unsigned char)*ptr) && !in_quote) {
            if (in_token) {
                *write_ptr = '\0';
                write_ptr++;
                in_token = 0;
            }
        } else {
            if (*ptr == '"') {
                in_quote = !in_quote;
            } else {
                if (!in_token) {
                    argv[argc++] = write_ptr;
                    in_token = 1;
                }
                *write_ptr = *ptr;
                write_ptr++;
            }
        }
        ptr++;
    }
    
    if (in_token) {
        *write_ptr = '\0';
    }
    
    argv[argc] = NULL;
    return argc;
}

int redirection_handling(char *argv[], int argc, char **file_in, char **file_out) {
    int result = 0;
    *file_in = NULL;
    *file_out = NULL;
    
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "<") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: No file specified after <\n");
                return -1;
            }
            *file_in = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], ">") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: No file specified after >\n");
                return -1;
            }
            *file_out = argv[i + 1];
            i++;
        } else {
            argv[result] = argv[i];
            result++;
        }
    }
    argv[result] = NULL;
    return result;
}

void run_pipeline(char *argv[], int argc, int pipe_pos) {
    char *argv_cmd1[MAX_ARGS];
    char *argv_cmd2[MAX_ARGS];
    int cmd1_count = 0;
    int cmd2_count = 0;
    
    for (int i = 0; i < pipe_pos; i++) {
        argv_cmd1[cmd1_count++] = argv[i];
    }
    argv_cmd1[cmd1_count] = NULL;
    
    for (int i = pipe_pos + 1; i < argc; i++) {
        argv_cmd2[cmd2_count++] = argv[i];
    }
    argv_cmd2[cmd2_count] = NULL;
    
    if (cmd1_count == 0 || cmd2_count == 0) {
        fprintf(stderr, "Error: Invalid pipeline syntax\n");
        return;
    }
    
    int fd[2];
    if (pipe(fd) < 0) {
        perror("pipe");
        return;
    }
    
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return;
    }
    
    if (pid1 == 0) {
        if (dup2(fd[1], 1) < 0) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(fd[0]);
        close(fd[1]);
        execvp(argv_cmd1[0], argv_cmd1);
        perror("execvp cmd1");
        exit(EXIT_FAILURE);
    }
    
    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        waitpid(pid1, NULL, 0);
        return;
    }
    
    if (pid2 == 0) {
        if (dup2(fd[0], 0) < 0) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(fd[1]);
        close(fd[0]);
        execvp(argv_cmd2[0], argv_cmd2);
        perror("execvp cmd2");
        exit(EXIT_FAILURE);
    }
    
    close(fd[0]);
    close(fd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void print_prompt() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)))
        printf("mini-shell:%s $ ", cwd);
    else
        printf("$ ");
    fflush(stdout);
}

int check_background(char *argv[], int argc) {
    if (argc > 0 && strcmp(argv[argc - 1], "&") == 0) {
        argv[argc - 1] = NULL;
        return 1;
    }
    return 0;
}

int main() {
    char *line = NULL;
    size_t len = 0;
    char *argv[MAX_ARGS];
    char *file_in = NULL;
    char *file_out = NULL;

    signal(SIGINT, SIG_IGN);

    while (1) {
        while (waitpid(-1, NULL, WNOHANG) > 0);

        print_prompt();
        ssize_t nread = getline(&line, &len, stdin);
        if (nread == -1) {
            printf("\n");
            break;
        }

        if (nread > 0 && line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
        }

        if (line[0] == '\0') continue;

        int argc = generate_tokens(line, argv);
        if (argc <= 0) continue;

        if (strcmp(argv[0], "exit") == 0) break;
        
        if (strcmp(argv[0], "cd") == 0) {
            char *next = argv[1];
            if (next == NULL) next = getenv("HOME");
            if (chdir(next) != 0) perror("cd");
            continue;
        }

        int pipeline_position = -1;
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "|") == 0) {
                pipeline_position = i;
                break;
            }
        }

        if (pipeline_position != -1) {
            run_pipeline(argv, argc, pipeline_position);
            continue;
        }

        int background = check_background(argv, argc);
        if (argv[0] == NULL) continue;
        argc = 0;
        while(argv[argc] != NULL) argc++;

        argc = redirection_handling(argv, argc, &file_in, &file_out);
        if (argc < 0) continue;
        if (argc == 0) continue;

        pid_t val = fork();
        if (val < 0) {
            perror("fork");
            continue;
        } else if (val == 0) {
            signal(SIGINT, SIG_DFL);
            setpgid(0, 0); 
            
            if (file_out != NULL) {
                int fd = open(file_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) { perror("open output"); exit(1); }
                dup2(fd, 1);
                close(fd);
            }

            if (file_in != NULL) {
                int fd = open(file_in, O_RDONLY);
                if (fd < 0) { perror("open input"); exit(1); }
                dup2(fd, 0);
                close(fd);
            }

            execvp(argv[0], argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else {
            if (background) {
                printf("[bg] started pid %d\n", val);
            } else {
                int status;
                waitpid(val, &status, 0);
            }
        }
    }

    free(line);
    return 0;
}
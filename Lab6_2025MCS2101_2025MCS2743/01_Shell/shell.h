#ifndef SHELL_H
#define SHELL_H

#include "program_manager.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>

#define MAX_ARGS 64

static void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

class Shell {
private:
    ProgramManager programManager;
    bool running;
    
    std::vector<char*> vectorToArgv(const std::vector<std::string>& tokens) {
        std::vector<char*> args;
        for (const auto& token : tokens) {
            args.push_back(const_cast<char*>(token.c_str()));
        }
        args.push_back(nullptr);
        return args;
    }

    int check_background(std::vector<char*>& argv) {
        int argc = 0;
        while(argv[argc] != nullptr) argc++;
        
        if (argc > 0 && strcmp(argv[argc - 1], "&") == 0) {
            argv[argc - 1] = nullptr;
            return 1;
        }
        return 0;
    }

    int redirection_handling(std::vector<char*>& argv, char **file_in, char **file_out) {
        int argc = 0;
        while(argv[argc] != nullptr) argc++;
        
        int result = 0;
        *file_in = NULL;
        *file_out = NULL;

        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "<") == 0) {
                if (i + 1 >= argc) {
                    std::cerr << "Error: No file specified after <" << std::endl;
                    return -1;
                }
                *file_in = argv[i + 1];
                i++;
            } else if (strcmp(argv[i], ">") == 0) {
                if (i + 1 >= argc) {
                    std::cerr << "Error: No file specified after >" << std::endl;
                    return -1;
                }
                *file_out = argv[i + 1];
                i++;
            } else {
                argv[result] = argv[i];
                result++;
            }
        }
        argv[result] = nullptr;
        return result;
    }

    void run_pipeline(std::vector<char*>& argv, int pipe_pos) {
        int argc = 0;
        while(argv[argc] != nullptr) argc++;

        std::vector<char*> cmd1_args;
        std::vector<char*> cmd2_args;

        for (int i = 0; i < pipe_pos; i++) cmd1_args.push_back(argv[i]);
        cmd1_args.push_back(nullptr);

        for (int i = pipe_pos + 1; i < argc; i++) cmd2_args.push_back(argv[i]);
        cmd2_args.push_back(nullptr);

        if (cmd1_args.size() <= 1 || cmd2_args.size() <= 1) {
            std::cerr << "Error: Invalid pipeline syntax" << std::endl;
            return;
        }

        int fd[2];
        if (pipe(fd) < 0) { perror("pipe"); return; }

        pid_t pid1 = fork();
        if (pid1 < 0) {
            perror("fork");
            close(fd[0]); close(fd[1]);
            return;
        }
        
        if (pid1 == 0) { 
            signal(SIGINT, SIG_DFL);
            signal(SIGCHLD, SIG_DFL);

            if (dup2(fd[1], 1) < 0) { perror("dup2"); exit(EXIT_FAILURE); }
            close(fd[0]); close(fd[1]);
            execvp(cmd1_args[0], cmd1_args.data());
            perror("execvp cmd1"); exit(EXIT_FAILURE);
        }

        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("fork");
            close(fd[0]); close(fd[1]);
            waitpid(pid1, NULL, 0);
            return;
        }
        
        if (pid2 == 0) { 
            signal(SIGINT, SIG_DFL);
            signal(SIGCHLD, SIG_DFL);

            if (dup2(fd[0], 0) < 0) { perror("dup2"); exit(EXIT_FAILURE); }
            close(fd[1]); close(fd[0]);
            execvp(cmd2_args[0], cmd2_args.data());
            perror("execvp cmd2"); exit(EXIT_FAILURE);
        }

        close(fd[0]); close(fd[1]);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    }

    void executeExternalCommand(const std::vector<std::string>& tokens) {
        std::vector<char*> argv = vectorToArgv(tokens);
        int argc = argv.size() - 1;

        int pipeline_position = -1;
        for (int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "|") == 0) {
                pipeline_position = i;
                break;
            }
        }

        if (pipeline_position != -1) {
            run_pipeline(argv, pipeline_position);
            return;
        }

        char *file_in = NULL;
        char *file_out = NULL;
        int new_argc = redirection_handling(argv, &file_in, &file_out);
        if (new_argc < 0) return;

        int background = check_background(argv);
        
        if (argv[0] == nullptr) return;

        pid_t val = fork();
        if (val < 0) {
            perror("fork");
        } else if (val == 0) {
           
            signal(SIGINT, SIG_DFL);
    
            signal(SIGCHLD, SIG_DFL); 

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

            execvp(argv[0], argv.data());
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

    std::vector<std::string> tokenize(const std::string& line) {
        std::vector<std::string> tokens;
        std::string current_token;
        char active_quote = 0; 
        
        for (size_t i = 0; i < line.length(); i++) {
            char c = line[i];
            
            if (active_quote == 0 && isspace(c)) {
                if (!current_token.empty()) {
                    tokens.push_back(current_token);
                    current_token.clear();
                }
            } 
            else if (c == '"' || c == '\'') {
                if (active_quote == 0) {
                    active_quote = c;
                } else if (active_quote == c) {
                    active_quote = 0;
                } else {
                    current_token += c;
                }
            } 
            else {
                current_token += c;
            }
        }
        
        if (!current_token.empty()) {
            tokens.push_back(current_token);
        }
        return tokens;
    }
   
    void handleSubmit(const std::vector<std::string>& args) {
         if (args.size() < 2) { std::cerr << "Usage: submit <filename>" << std::endl; return; }
         ProgramID pid = programManager.submitProgram(args[1]);
         std::cout << "PID = " << pid << std::endl;
         if (programManager.getProgramState(pid) == "ERROR") {
             std::cout << "Error: " << programManager.getProgramOutput(pid) << std::endl; 
         }
    }
    
    void handleRun(const std::vector<std::string>& args) {
        if (args.size() < 2) return;
        ProgramID pid = std::stoi(args[1]);
        if (programManager.runProgram(pid)) {
             std::string out = programManager.getProgramOutput(pid);
             if(!out.empty()) std::cout << out;
        }
    }

    void handleDebug(const std::vector<std::string>& args) {
         if (args.size() < 2) return;
         ProgramID pid = std::stoi(args[1]);
         
         std::cout << "Entering debug mode. Type 'exit' to leave.\n";
         std::string line;
         while(true) {
             std::cout << "(debug) ";
             if(!std::getline(std::cin, line) || line == "exit") break;
             auto tokens = tokenize(line);
             if(tokens.empty()) continue;
             std::vector<std::string> cmdArgs(tokens.begin()+1, tokens.end());
             programManager.debugProgram(pid, tokens[0], cmdArgs);
         }
    }

    void handleGC(const std::vector<std::string>& args) {
        if (args.size() < 2) return;
        programManager.gc(std::stoi(args[1]));
    }

    void handleMemstat(const std::vector<std::string>& args) {
        if (args.size() < 2) return;
        programManager.memstat(std::stoi(args[1]));
    }

    void handleLeaks(const std::vector<std::string>& args) {
        if (args.size() < 2) return;
        programManager.leaks(std::stoi(args[1]));
    }

    void handleList(const std::vector<std::string>&) {
        programManager.listPrograms();
    }
    
    void handleHelp(const std::vector<std::string>&) {
        std::cout << "Available commands:" << std::endl;
        std::cout << "  submit <program>   - Submit a program for execution" << std::endl;
        std::cout << "  run <pid>          - Run a submitted program" << std::endl;
        std::cout << "  debug <pid>        - Enter debug mode for a program" << std::endl;
        std::cout << "  kill <pid>         - Terminate a program" << std::endl;
        std::cout << "  memstat <pid>      - Show memory statistics" << std::endl;
        std::cout << "  gc <pid>           - Run garbage collection" << std::endl;
        std::cout << "  leaks <pid>        - Detect memory leaks" << std::endl;
        std::cout << "  list               - List all programs" << std::endl;
        std::cout << "  help               - Show this help message" << std::endl;
        std::cout << "  exit               - Exit the shell" << std::endl;
    }

public:
    Shell() : running(false) {}
    
    void run() {
        running = true;
        // Signal handling (Parent Ignores Ctrl+C)
        signal(SIGCHLD, sigchld_handler);
        signal(SIGINT, SIG_IGN); 

        std::cout << "=== Integrated System Shell (Lab 6) ===\n";
        
        while (running) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd))) std::cout << "mini-shell:" << cwd << " $ ";
            else std::cout << "$ ";

            std::string line;
            if (!std::getline(std::cin, line)) break;
            if (line.empty()) continue;
            
            auto tokens = tokenize(line);
            if (tokens.empty()) continue;
            
            std::string command = tokens[0];
            
            // 1. Internal Commands (Lab 6)
            if (command == "submit") handleSubmit(tokens);
            else if (command == "run") handleRun(tokens);
            else if (command == "debug") handleDebug(tokens);
            else if (command == "gc") handleGC(tokens);
            else if (command == "memstat") handleMemstat(tokens);
            else if (command == "leaks") handleLeaks(tokens);
            else if (command == "list") handleList(tokens);
            else if (command == "help") handleHelp(tokens);
            else if (command == "exit") running = false;
            
            // 2. Built-in CD (from Lab 1 logic)
            else if (command == "cd") {
                const char* next = (tokens.size() > 1) ? tokens[1].c_str() : getenv("HOME");
                if (chdir(next) != 0) perror("cd");
            }
            
            // 3. External Commands (Lab 1)
            else {
                executeExternalCommand(tokens);
            }
        }
    }
};

#endif 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../include/sequential.h"
#include "../include/parser.h"
#include "../include/arbitrary_commands.h"
#include "../include/redirection.h"
#include "../include/log.h"
#include "../include/utils.h"
#include "../include/reveal.h"
#include "../include/activities.h"
#include "../include/hop.h"
#include "../include/ping.h"

// External variables from main.c
extern char prev_dir[1024];

int is_builtin_command(char *command) {
    return (strcmp(command, "hop") == 0 || 
            strcmp(command, "reveal") == 0 ||
            strcmp(command, "log") == 0 ||
            strcmp(command, "activities") == 0 ||
            strcmp(command, "ping") == 0);
}

int execute_builtin_sequential(int argc, char **args) {
    if (strcmp(args[0], "hop") == 0) {
        char current_dir[1024];
        getcwd(current_dir, sizeof(current_dir));
        hop(argc, args);
        char new_dir[1024];
        getcwd(new_dir, sizeof(new_dir));
        if (strcmp(current_dir, new_dir) != 0) {
            strcpy(prev_dir, current_dir);
        }
        return 1;
    }
    else if (strcmp(args[0], "reveal") == 0) {
        reveal(argc, args, prev_dir);
        return 1;
    }
    else if (strcmp(args[0], "log") == 0) {
        if (argc == 1) {
            print_log();
        }
        else if (argc == 2 && (strcmp(args[1], "purge") == 0)) {
            purge_log();
        }
        else if (argc == 3 && (strcmp(args[1], "execute") == 0)) {
            int index = atoi(args[2]);
            if (index > 0) {
                execute_log(index);
            }
            else {
                fprintf(stderr, "Index overflow\n");
            }
        }
        else {
            fprintf(stderr, "log: Invalid Syntax!\n");
        }
        return 1;
    }
    else if (strcmp(args[0], "activities") == 0) {
        activities();
        return 1;
    }
    else if (strcmp(args[0], "ping") == 0) {
        ping_command(argc, args);
        return 1;
    }
    
    return 0; // Not a built-in command
}

int sequential_execute(char *input) {
    // Make a copy to avoid modifying original input
    char input_copy[1024];
    strncpy(input_copy, input, sizeof(input_copy) - 1);
    input_copy[sizeof(input_copy) - 1] = '\0';
    
    // Split by ';'
    char *commands[64];
    int count = 0;
    
    char *token = strtok(input_copy, ";");
    while (token != NULL && count < 64) {
        commands[count++] = token;
        token = strtok(NULL, ";");
    }
    
    for (int i = 0; i < count; i++) {
        // Trim leading and trailing spaces
        char *cmd = commands[i];
        while (*cmd == ' ' || *cmd == '\t') cmd++;
        
        if (strlen(cmd) == 0) continue; // skip empty commands
        
        // Remove trailing spaces
        char *end = cmd + strlen(cmd) - 1;
        while (end > cmd && (*end == ' ' || *end == '\t' || *end == '\n')) {
            *end = '\0';
            end--;
        }
        
        if (strlen(cmd) == 0) continue; // skip if only whitespace
        
        // Make another copy for this command
        char cmd_copy[1024];
        strncpy(cmd_copy, cmd, sizeof(cmd_copy) - 1);
        cmd_copy[sizeof(cmd_copy) - 1] = '\0';
        
        // Tokenize for argc/argv
        char *args[64];
        int argc = 0;
        char *saveptr; // for strtok_r
        char *tok = strtok_r(cmd_copy, " \t\n", &saveptr);
        while (tok != NULL && argc < 63) {
            args[argc++] = tok;
            tok = strtok_r(NULL, " \t\n", &saveptr);
        }
        args[argc] = NULL;
        
        if (argc == 0) continue;

        // Check if it's a built-in command first
        if (is_builtin_command(args[0])) {
            execute_builtin_sequential(argc, args);
            // Built-in commands don't need logging here as they handle it themselves
            continue;
        }
        
        // Check if command contains pipes - if so, handle separately
        int has_pipes = 0;
        for (int j = 0; j < argc; j++) {
            if (strcmp(args[j], "|") == 0) {
                has_pipes = 1;
                break;
            }
        }
        
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (has_pipes) {
                // Handle piped commands (you'll need to implement execute_pipeline)
                // execute_pipeline(args);
                // fprintf(stderr, "Pipes not yet implemented in sequential\n");
                exit(1);
            } else {
                // Handle simple commands with redirections
                if (input_redirection(&argc, args) < 0) exit(1);
                if (output_redirection(&argc, args) < 0) exit(1);
                
                if (execute_arbitrary_commands(argc, args) == 1) {
                    exit(0);
                } else {
                    exit(1);
                }
            }
        } else if (pid > 0) {
            // Parent process - wait for child to complete
            int status;
            waitpid(pid, &status, 0);
            // Continue to next command regardless of status (requirement)
        } else {
            perror("fork failed");
        }
        
        // Log the command
        add_to_log(cmd);
    }
    
    return 0;
}
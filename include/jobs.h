#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

// Define symbolic constants for process states
#define RUNNING 1
#define STOPPED 0
#define KILLED -1

// Correct structure for a process
typedef struct {
    pid_t pid;
    char command[1024];
    int job_id; // <-- The variable the compiler is looking for
    int status;
} process_t;

// Function prototypes
void fg_command(int argc, char* argv[]);
void bg_command(int argc, char* argv[]);

#endif

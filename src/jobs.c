// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <signal.h>
// #include <sys/wait.h>
// #include <unistd.h>
// #include <errno.h>

// #include "../include/jobs.h"
// // #include "../include/activities.h"
// #include "../include/signals.h"
// // These global variables must be defined here, without 'static'
// // process_t process_table[MAX_PROCESS];
// // int proc_count = 0;
// // int job_counter = 0; // If you are using a global job counter

// // These are your global variables, they must be externed
// extern process_t process_table[1024];
// extern int proc_count;
// extern pid_t foreground_pgid;
// extern int job_counter; // A global counter for job IDs

// // Find a job by its job ID
// process_t* find_job_by_id(int job_id) {
//     for (int i = 0; i < proc_count; i++) {
//         if (process_table[i].job_id == job_id) {
//             return &process_table[i];
//         }
//     }
//     return NULL;
// }

// // Find the most recently added job
// process_t* find_most_recent_job() {
//     if (proc_count == 0) {
//         return NULL;
//     }
//     // Return the last process in the list
//     return &process_table[proc_count - 1];
// }

// // Function to handle the 'fg' command
// void fg_command(int argc, char* argv[]) {
//     int job_id_to_bring_to_fg;
//     process_t* job;

//     if (argc == 1) {
//         job = find_most_recent_job();
//         if (job == NULL) {
//             printf("No jobs to bring to foreground\n");
//             return;
//         }
//     } else if (argc == 2) {
//         job_id_to_bring_to_fg = atoi(argv[1]);
//         job = find_job_by_id(job_id_to_bring_to_fg);
//         if (job == NULL) {
//             printf("No such job\n");
//             return;
//         }
//     } else {
//         printf("Usage: fg [job_number]\n");
//         return;
//     }

//     printf("%s\n", job->command);

//     // Set the process group ID to bring it to the foreground
//     // This is necessary for terminal I/O and signal handling
//     if (setpgid(job->pid, job->pid) < 0) {
//         perror("setpgid failed");
//         return;
//     }
//     tcsetpgrp(STDIN_FILENO, job->pid);

//     // If the job is stopped, resume it
//     if (job->status == STOPPED) {
//         if (kill(-job->pid, SIGCONT) < 0) {
//             perror("kill (SIGCONT) failed");
//             return;
//         }
//     }

//     // Wait for the foreground job to complete or stop again
//     foreground_pgid = job->pid;
//     int status;
//     waitpid(job->pid, &status, WUNTRACED);
//     foreground_pgid = -1;

//     // Remove the job from the background list if it has terminated
//     if (WIFEXITED(status) || WIFSIGNALED(status)) {
//         remove_process(job->pid);
//     } else if (WIFSTOPPED(status)) {
//         job->status = STOPPED;
//     }
    
//     // Return control of the terminal to the shell's process group
//     tcsetpgrp(STDIN_FILENO, getpgrp());
// }

// // Function to handle the 'bg' command
// void bg_command(int argc, char* argv[]) {
//     int job_id_to_resume;
//     process_t* job;

//     if (argc == 1) {
//         job = find_most_recent_job();
//         if (job == NULL) {
//             printf("No jobs to resume\n");
//             return;
//         }
//     } else if (argc == 2) {
//         job_id_to_resume = atoi(argv[1]);
//         job = find_job_by_id(job_id_to_resume);
//         if (job == NULL) {
//             printf("No such job\n");
//             return;
//         }
//     } else {
//         printf("Usage: bg [job_number]\n");
//         return;
//     }

//     // Check if the job is already running
//     if (job->status == RUNNING) {
//         printf("Job already running\n");
//         return;
//     }

//     // Send SIGCONT to the stopped job to resume it in the background
//     if (kill(-job->pid, SIGCONT) < 0) {
//         perror("kill (SIGCONT) failed");
//         return;
//     }

//     // Print the required status message
//     job->status = RUNNING;
//     printf("[%d] %s &\n", job->job_id, job->command);
// }


// // eof

// // ### How to Integrate with Your Shell

// // 1.  **Create the files:** Save the two code blocks above as `job_control.h` and `job_control.c` in your shell's project directory. You might want to place them in your `include/` and `src/` folders respectively, if you have that structure.
// // 2.  **Include the header:** In your `main.c` file, add `#include "../include/job_control.h"`.
// // 3.  **Update `main.c` loop:** In the `main` loop where you handle your commands, add two `else if` statements to check for `fg` and `bg`.

// // ```c
// // // ... in your main.c loop
// // else if (strcmp(args[0], "fg") == 0) {
// //     fg_command(argc, args);
// //     should_log = 0;
// // }
// // else if (strcmp(args[0], "bg") == 0) {
// //     bg_command(argc, args);
// //     should_log = 0;
// // }
// // // ... rest of your command handling

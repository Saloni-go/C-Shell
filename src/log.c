// ---------------------- LLM Generated Code Begins--------------------------

#define _GNU_SOURCE
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

static char *entries[LOG_SIZE]={NULL};
static int count = 0;
static char log_path[PATH_MAX] = {0};

/* --- Helpers --- */
static char *strdup_safe(const char *s) {
    if (!s) return NULL;
    char *p = malloc(strlen(s) + 1);
    if (p) strcpy(p, s);
    return p;
}
static void trim_newline(char *s) {
    if (!s) return;
    size_t L = strlen(s);
    if (L > 0 && s[L-1] == '\n') s[L-1] = '\0';
}

/* Save memory entries -> file */
static void save_log_to_file(void) {
    if (log_path[0] == '\0') return;
    FILE *f = fopen(log_path, "w");
    if (!f) return;
    
    for (int i = 0; i < count; i++) {
        fprintf(f, "%s\n", entries[i]);
    }
    fclose(f);
}

// int is_shell_operator(const char* token) {
//     return (strcmp(token, "|") == 0 ||
//             strcmp(token, ">") == 0 ||
//             strcmp(token, ">>") == 0 ||
//             strcmp(token, "<") == 0 ||
//             strcmp(token, ";") == 0 ||
//             strcmp(token, "&") == 0 ||
//             strcmp(token, "&&") == 0 ||
//             strcmp(token, "||") == 0);
// }

/* Load file -> memory */
static void load_log_from_file(void) {
    if (log_path[0] == '\0') return;
    FILE *f = fopen(log_path, "r");
    if (!f) return;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, f)) != -1) {
        trim_newline(line);
        if (count < LOG_SIZE) {
            entries[count++] = strdup_safe(line);
        } else {
            // drop oldest
            free(entries[0]);
            for (int i = 1; i < LOG_SIZE; i++) {
                entries[i-1] = entries[i];
            }
            entries[LOG_SIZE-1] = strdup_safe(line);
        }
    }
    free(line);
    fclose(f);
}

/* --- Public API --- */
void log_init(const char *filename) {
    // build log file path
    const char *fname = filename ? filename : ".mini_shell_log";
    if (!filename) {
        const char *home = getenv("HOME");
        if (home) {
            snprintf(log_path, sizeof(log_path), "%s/%s", home, fname);
        } else {
            snprintf(log_path, sizeof(log_path), "./%s", fname);
        }
    } else {
        strncpy(log_path, fname, sizeof(log_path)-1);
        log_path[sizeof(log_path)-1] = '\0';
    }

    // Clear memory - use count to know what's valid
    for (int i = 0; i < count; i++) {  // ← Only free up to count
        if (entries[i] != NULL) {
            free(entries[i]);
            entries[i] = NULL;
        }
    }
    // Clear the rest
    for (int i = count; i < LOG_SIZE; i++) {
        entries[i] = NULL;
    }
  
    count=0;
    // load from file if exists
    load_log_from_file();
}

void log_free(void) {
    for (int i = 0; i < count; i++) {
        free(entries[i]);
        entries[i] = NULL;
    }
    count = 0;
    log_path[0] = '\0';
}

static void get_cmd_name(const char *cmd, char *out, size_t outsz) {
    out[0] = '\0';
    if (!cmd) return;
    const char *p = cmd;
    while (*p && (*p == ' ' || *p == '\t')) p++;
    size_t i = 0;
    while (*p && *p != ' ' && *p != '\t' && i+1 < outsz) {
        out[i++] = *p++;
    }
    out[i] = '\0';
}

void add_to_log(const char *shell_cmd) {
    if (!shell_cmd || !*shell_cmd) return;

    // skip if command-name is "log"
    char name[128];
    get_cmd_name(shell_cmd, name, sizeof(name));
    if (strcmp(name, "log") == 0) return;

    // skip if identical to last entry
    if (count > 0 && strcmp(entries[count-1], shell_cmd) == 0) return;

    // add to list
    if (count == LOG_SIZE) {
        free(entries[0]);
        for (int i = 1; i < LOG_SIZE; i++) entries[i-1] = entries[i];
        entries[LOG_SIZE-1] = strdup_safe(shell_cmd);
    } else {
        entries[count++] = strdup_safe(shell_cmd);
    }

    save_log_to_file();
}

void print_log(void) {
    for (int i = 0; i < count; i++) {
        printf("%s\n", entries[i]);
    }
}

void purge_log(void) {
    for (int i = 0; i < count; i++) {
        free(entries[i]);
        entries[i] = NULL;
    }
    count = 0;
    if (log_path[0] != '\0') {
        FILE *f = fopen(log_path, "w");
        if (f) fclose(f);
    }
}

int execute_log(int index) {
  
    // printf("index: %d\n",index);
    if (index < 1 || index > count) {
        fprintf(stderr, "log: invalid index %d (1..%d)\n", index, count);
        return -1;
    }
 
    int idx = count - index; // convert newest→oldest
    const char *cmd = entries[idx];
    if (!cmd) return -1;
    printf("%s\n", cmd); // echo before execution


    // Re-use your shell parser/executor
    char input_copy[1024];
    strncpy(input_copy, cmd, sizeof(input_copy));
    input_copy[sizeof(input_copy)-1] = '\0';

    // parse and execute like normal command
    // printf("1\n");
    parse_shell(input_copy);  
    return 0;
}


//---------------------------- LLM Generated Code Ends--------------------------------------
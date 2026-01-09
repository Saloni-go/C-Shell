//--------------------LLM Generated Code Begins-------------------------------
#ifndef LOG_H
#define LOG_H

#ifndef LOG_SIZE
#define LOG_SIZE 15
#endif

/* Initialize log system. 
   If filename is NULL, default = $HOME/.mini_shell_log */
void log_init(const char *filename);

/* Free all memory used by log. Call before shell exit. */
void log_free(void);

/* Add a shell command string to log (applies rules and persists). */
void add_to_log(const char *shell_cmd);

/* Print stored commands (oldest -> newest). */
void print_log(void);

/* Clear history (memory + file). */
void purge_log(void);

/* Execute the <index>-th most recent command.
   Indexing: 1 = most recent, 2 = 2nd most recent, etc.
   Returns: 0 if executed successfully, -1 on error. */
int execute_log(int index);

#endif /* LOG_H */

//-----------------------------LLM Generated Code Ends------------------------

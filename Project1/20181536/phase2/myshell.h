/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);
void exec_history(long long flag);
void add_history(char *command);
void split_pipe(char **argv, char **first, char **second);
void exec_pipe(char **first, char **second);
/* $begin shellmain */
#include "csapp.h"
#include "myshell.h"
#include<errno.h>
#define MAXARGS   128

FILE *fp_history;
unsigned long long history_num = 0;
int nowar;
char *nowar2;

int main() 
{
    char cmdline[MAXLINE]; /* Command line */
    fp_history = fopen("history.txt", "a+"); //history file open
    fseek(fp_history, 0, SEEK_SET);
    char c;
    while(1) { //count history_num
        c = fgetc(fp_history);
        if(c == '\n') {
            history_num++;
        }
        if(c == EOF) {
            break;
        }
    }

    do {
	/* Read */
	printf("CSE4100-MP-P1> ");                   
	nowar2 = fgets(cmdline, MAXLINE, stdin); 
	if (feof(stdin))
	    exit(0);

	/* Evaluate */
    if(cmdline[0] != '!') { //wrtie history
        add_history(cmdline);
    }
	eval(cmdline);
    } while(1);
}
/* $end shellmain */

/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    int cmd_len = strlen(cmdline);

    for(int i = 0; i < cmd_len; i++) { // remove ', ", ` from cmdline
        if((cmdline[i] == '\'') || (cmdline[i] == '"') || (cmdline[i] == '`')) {
            cmdline[i] = ' ';
        }
    }
    
    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    if (argv[0] == NULL)  
	    return;   /* Ignore empty lines */

    if (!builtin_command(argv)) { //quit -> exit(0), & -> ignore, other -> run
        char filename[MAXLINE + 5] = "";
        if(argv[0][0] != '.') {
            strcat(filename, "/bin/");
        }
        strcat(filename, argv[0]);
        
        if((pid = Fork()) == 0) { // Child process
            if (execve(filename, argv, environ) < 0) {	//ex) /bin/ls ls -al &
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
            exit(0);
        }
	/* Parent waits for foreground job to terminate */
	    if (!bg) {
	        int status;
            Wait(&status);
	    }
	    else//when there is backgrount process!
	        printf("%d %s", pid, cmdline);
    }

    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
    if (!strcmp(argv[0], "quit")) /* quit command */
	    exit(0);  
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
	    return 1;
    if (!strcmp(argv[0], "exit")) /* exit command */
	    exit(0);
    if (!strcmp(argv[0], "cd")) {  /* cd command */
        if(argv[1] == NULL || argv[1][0] == '~') {
            char *path = getenv("HOME");
            nowar = chdir(path);
        }
        else if(argv[1][0] == '/') {
            if(chdir(argv[1]) < 0) {
                printf("%s: %s : No such file or directory\n",argv[0], argv[1]);
            }
        }
        else {
            char path[MAXLINE];
            nowar2 = getcwd(path, sizeof(path));
            strcat(path, "/"); 
            strcat(path, argv[1]);
            nowar = chdir(path);
        }
        return 1;
    }
    if (!strcmp(argv[0], "history")) {  /* history command */
        exec_history(-1);
        return 1;
    }
    if (!strcmp(argv[0], "!!")) { /* !! command */
        exec_history(0);
        return 1;
    }
    if(argv[0][0] == '!') { /* !# command */
        long long n = atoi(&argv[0][1]); 
        exec_history(n);
        return 1;
    }
    return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	    buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
	    argv[argc++] = buf;
	    *delim = '\0';
	    buf = delim + 1;
	    while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
	    return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
	    argv[--argc] = NULL;

    return bg;
}
/* $end parseline */


void exec_history(long long flag) {
    char c;
    char command[MAXLINE];
    int line = 0, temp;
    if(flag == -1) { // history command
        fseek(fp_history, 0, SEEK_SET);
        while((c = fgetc(fp_history)) != EOF) {
            printf("%c",c);
        }
    }
    else if(flag == 0) { // !! command
        fseek(fp_history, 0, SEEK_SET);
        if(history_num != 1) {
            while(1) {
                c = fgetc(fp_history);
                if(c == '\n') {
                    line++;
                    if(line == history_num - 1) {
                        break;
                    }
                }
            }
        }
        nowar = fscanf(fp_history, "%d. ", &temp);
        nowar2 = fgets(command, MAXLINE, fp_history);
        printf("%s", command);
        eval(command);
    }
    else { // !# command
        fseek(fp_history, 0, SEEK_SET);
        if(flag != 1) {
            while(1) {
                c = fgetc(fp_history);
                if(c == '\n') {
                    line++;
                    if(line == flag - 1) {
                        break;
                    }
                }
            }
        }
        nowar = fscanf(fp_history, "%d. ", &temp);
        nowar2 = fgets(command, MAXLINE, fp_history);
        add_history(command);
        printf("%s", command);
        eval(command);
    }
}

void add_history(char *command) { // write history to file
    history_num++;
    fseek(fp_history, 0, SEEK_END);
    fprintf(fp_history, "%lld. %s", history_num, command);
}
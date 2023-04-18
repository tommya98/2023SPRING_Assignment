/* $begin shellmain */
#include "csapp.h"
#include "myshell.h"
#include<errno.h>
#define MAXARGS   128

typedef struct { //job struct for job control
    pid_t pid;
    int jid;
    int state; // 0:running fg, 1: running bg, 2: stopped
    char command[MAXLINE];
}job_t;

FILE *fp_history;
unsigned long long history_num = 0, pipe_num, jnum = 1;
int nowar;
char *nowar2;
job_t jobs[1000]; //job list

int main() 
{
    char cmdline[MAXLINE]; /* Command line */

    Signal(SIGCHLD, sigchld_handler); //set handler
    Signal(SIGINT, sigint_handler);
    Signal(SIGTSTP, sigtstp_handler);

    fp_history = fopen("history.txt", "a+"); // history file open
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

    for(int i = 0; i < 1000; i++) { //init jobs
        jobs[i].pid = -1;
        jobs[i].jid = -1;
        jobs[i].state = -1;
        jobs[i].command[0] = '\0';
    }

    do {
	/* Read */
	printf("CSE4100-MP-P1> ");                   
	nowar2 = fgets(cmdline, MAXLINE, stdin); 
	if (feof(stdin))
	    exit(0);

	/* Evaluate */
    if(cmdline[0] != '!') {  //wrtie history
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
    sigset_t mask;

    for(int i = 0; i < cmd_len; i++) { // remove ', ", ` from cmdline
        if((cmdline[i] == '\'') || (cmdline[i] == '"') || (cmdline[i] == '`')) {
            cmdline[i] = ' ';
        }
    }

    strcpy(buf, cmdline);
    bg = parseline(buf, argv); 
    if (argv[0] == NULL)  
	    return;   /* Ignore empty lines */

    pipe_num = 1;
    for(int i = 0; i < cmd_len; i++) { // count pipe_num
        if(cmdline[i] == '|') {
            pipe_num++;
        }
    }

    if (!builtin_command(argv)) { //quit -> exit(0), & -> ignore, other -> run
        char filename[MAXLINE + 5] = "";
        if(argv[0][0] != '.') {
            strcat(filename, "/bin/");
        }
        strcat(filename, argv[0]);

        Sigemptyset(&mask);
        Sigaddset(&mask, SIGCHLD);
        Sigprocmask(SIG_BLOCK, &mask, NULL);

        if((pid = Fork()) == 0) { // Child process
            Sigprocmask(SIG_UNBLOCK, &mask, NULL);
            if(pipe_num > 1) { //piped command
                char *first[MAXARGS];
                char *second[MAXARGS];
                split_pipe(argv, first, second);
                exec_pipe(first, second);
            }
            else if (execve(filename, argv, environ) < 0) {	//ex) /bin/ls ls -al &
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
            exit(0);
        }

        addjob(pid, bg, cmdline); //add job list
	/* Parent waits for foreground job to terminate */
	    if (!bg) {
            int status;
            Sigprocmask(SIG_UNBLOCK, &mask, NULL);
            if(waitpid(pid, &status, WUNTRACED) > 0) {
                if(WIFSTOPPED(status)) { //if child stopped
                    return;
                }
                else {
                    removejob(pid); 
                }
            }
	    }
	    else {//when there is background process!
            int temp;
            for(int i = 0; i < 1000; i++) {
                if(jobs[i].pid == pid) {
                    temp = jobs[i].jid;
                    break;
                }
            }
	        printf("[%d] %d\n", temp, pid);
            Sigprocmask(SIG_UNBLOCK, &mask, NULL);
        }
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
            char path[MAXLINE + 1];
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
        int n = atoi(&argv[0][1]);
        exec_history(n);
        return 1;
    }
    if(!strcmp(argv[0], "jobs")) { /* jobs command */
        for(int i = 0; i < 1000; i++) {
            if(jobs[i].pid != -1) { //printf every job
                printf("[%d] ", jobs[i].jid);
                if(jobs[i].state == 1) {
                    printf("Running         ");
                }
                else if(jobs[i].state == 2) {
                    printf("Stopped         ");
                }
                printf("%s", jobs[i].command);
            }
        }
        return 1;
    }
    if((!strcmp(argv[0], "bg")) && argv[1][0] == '%') { /* bg command */
        int jid = atoi(&(argv[1][1]));
        for(int i = 0; i < 1000; i++) {
            if(jobs[i].jid == jid) { //find job and change state and send sigcont siganl
                jobs[i].state = 1;
                printf("[%d] %s", jobs[i].jid, jobs[i].command);
                Kill(jobs[i].pid, SIGCONT);
                break;
            }
        }
        return 1;
    }
    if((!strcmp(argv[0], "fg")) && (argv[1][0] == '%')) { /* fg command */
        int jid = atoi(&(argv[1][1]));
        for(int i = 0; i < 1000; i++) {
            if(jobs[i].jid == jid) { //find job and change state and send sigcont siganl
                jobs[i].state = 0;
                printf("%s",jobs[i].command);
                Kill(jobs[i].pid, SIGCONT);
                while(1) {
                    if(jobs[i].state == 0) {
                        sleep(1);
                    }
                    else {
                        return 1;
                    }
                }
                break;
            }
        }
        return 1;
    }
    if((!strcmp(argv[0], "kill")) && (argv[1][0] == '%')) { /* kill command */
        int jid = atoi(&(argv[1][1]));
        for(int i = 0; i < 1000; i++) { //find job and send sigkill signal
            if(jobs[i].jid == jid) {
                printf("[%d] Terminated          %s", jobs[i].jid, jobs[i].command);
                Kill(jobs[i].pid, SIGKILL);
                break;
            }
        }
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
    if ((bg = (*argv[argc-1] == '&')) != 0) {
	    argv[--argc] = NULL;
        return bg;
    }

    int len = strlen(argv[argc - 1]);
    if((bg = (argv[argc-1][len - 1] == '&')) != 0) {
		argv[argc - 1][len - 1] = '\0';
    }

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

void split_pipe(char **argv, char **first, char **second) { // split piped command
    int index;

    for(int i = 0; argv[i] != NULL; i++) { //before | argv to first
        if(!strcmp(argv[i], "|")){
            index = i;
            break;
        }
        first[i] = argv[i];
        first[i + 1] = '\0';
    }

	for(int i = index + 1; argv[i] != NULL; i++){ //remain argv to second
		second[i - index - 1] = argv[i];
        second[i - index] = '\0';
	}
}

void exec_pipe(char **first, char **second) { //execute piped command
    int fd[2];
    char *thrid[MAXARGS];
    char *forth[MAXARGS];

    nowar = pipe(fd); //open pipe
    pipe_num--;

    if(Fork() == 0){ //child process
		dup2(fd[1], 1); //change stdout to fd[1]
		close(fd[0]);
		execvp(first[0], first);
	}
    else{
        int status;
		dup2(fd[0], 0); //change stdin to fd[0]
		close(fd[1]);
        wait(&status);
        if(pipe_num > 1) { // if more piped command
		    split_pipe(second, thrid, forth);
            exec_pipe(thrid, forth);
        }
		else{
			execvp(second[0], second);
		}
    }
}

void sigchld_handler(int sig) {
	int status;
	pid_t pid = -1;
    job_t* job = NULL;

	while(1){
		if((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) <= 0) {
            break;
        }

        for(int i = 0; i < 1000; i++) { //find matching jobs
            if(jobs[i].pid == pid) {
                job = &jobs[i];
                break;
            }
        }

		if(WIFSIGNALED(status) || WIFEXITED(status)) { //terminate siganl
            removejob(pid);
		}
	}
}

void sigint_handler(int sig) { //sigint handler
    pid_t pid = -1;

    for(int i = 0; i < 1000; i++) { //find job
        if(jobs[i].state == 0) {
            pid = jobs[i].pid;
            break;
        }
    }
    if(pid > 0) { //send sigint siganl
        printf("\n");
        Kill(pid, SIGINT);
    }
}

void sigtstp_handler(int sig) { //sigtstp handler
    pid_t pid = -1;

    for(int i = 0; i < 1000; i++) { //find jobs
        if(jobs[i].state == 0) {
            pid = jobs[i].pid;
            jobs[i].state = 2;
            printf("\n[%d] Stopped          %s", jobs[i].jid, jobs[i].command);
            break;
        }
    }

    if(pid > 0) { //sned sigtstp signal
        Kill(pid, SIGTSTP);
    }
}

void addjob(pid_t pid, int flag, char *command) { //add job list
    for(int i = 0; i < 1000; i++) {
        if(jobs[i].pid == -1) {
            jobs[i].pid = pid;
            jobs[i].state = flag;
            strcpy(jobs[i].command, command);
            jobs[i].jid = 0;
            for(int j = 1; j < 1000; j++) { //set jid smallest number
                int flag = 0;
                for(int k = 0; k < 1000; k++) {
                    if (jobs[k].jid == j) {
                        flag = 1;
                        break;
                    }
                }
                if(flag == 0) {
                    jobs[i].jid = j;
                    break;
                }
            }
            return;
        }
    }
}

void removejob(pid_t pid) { //remove job list
    for(int i = 0; i < 1000; i++) {
        if(jobs[i].pid == pid) { //reset job
            jobs[i].pid = -1;
            jobs[i].jid = -1;
            jobs[i].state = -1;
            jobs[i].command[0] = '\0';
            return;
        }
    }
}
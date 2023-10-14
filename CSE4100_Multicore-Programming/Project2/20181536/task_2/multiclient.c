#include "csapp.h"
#include <time.h>
#include <stdio.h>		/* declares printf() */
#include <stdlib.h>		/* declares exit() */
#include <stdint.h>		/* used for casting clock_t to uintmax_t for printf */
#include <sys/time.h>		/* needed for the gettimeofday() system call */

#define MAX_CLIENT 100
#define ORDER_PER_CLIENT 100
#define STOCK_NUM 10
#define BUY_SELL_MAX 10

int main(int argc, char **argv) 
{
	pid_t pids[MAX_CLIENT];
	int runprocess = 0, status, i;

	int clientfd, num_client;
	char *host, *port, buf[MAXLINE], tmp[3];
	rio_t rio;

	struct timeval start;	/* starting time */
	struct timeval end;	/* ending time */
	unsigned long e_usec;	/* elapsed microseconds */
    
  gettimeofday(&start, 0);	/* mark the start time */

	if (argc != 4) {
		fprintf(stderr, "usage: %s <host> <port> <client#>\n", argv[0]);
		exit(0);
	}

	host = argv[1];
	port = argv[2];
	num_client = atoi(argv[3]);

/*	fork for each client process	*/
	while(runprocess < num_client){
		//wait(&state);
		pids[runprocess] = fork();

		if(pids[runprocess] < 0)
			return -1;
		/*	child process		*/
		else if(pids[runprocess] == 0){
			printf("child %ld\n", (long)getpid());

			clientfd = Open_clientfd(host, port);
			Rio_readinitb(&rio, clientfd);
			srand((unsigned int) getpid());

			for(i=0;i<ORDER_PER_CLIENT;i++){
				int option = rand() % 3;
				//int option = 0;
				//int option = 1 + rand() % 2;
				
				if(option == 0){//show
					strcpy(buf, "show\n");
				}
				else if(option == 1){//buy
					int list_num = rand() % STOCK_NUM + 1;
					int num_to_buy = rand() % BUY_SELL_MAX + 1;//1~10

					strcpy(buf, "buy ");
					sprintf(tmp, "%d", list_num);
					strcat(buf, tmp);
					strcat(buf, " ");
					sprintf(tmp, "%d", num_to_buy);
					strcat(buf, tmp);
					strcat(buf, "\n");
				}
				else if(option == 2){//sell
					int list_num = rand() % STOCK_NUM + 1; 
					int num_to_sell = rand() % BUY_SELL_MAX + 1;//1~10
					
					strcpy(buf, "sell ");
					sprintf(tmp, "%d", list_num);
					strcat(buf, tmp);
					strcat(buf, " ");
					sprintf(tmp, "%d", num_to_sell);
					strcat(buf, tmp);
					strcat(buf, "\n");
				}
				//strcpy(buf, "buy 1 2\n");
			
				Rio_writen(clientfd, buf, strlen(buf));
				// Rio_readlineb(&rio, buf, MAXLINE);
				Rio_readnb(&rio, buf, MAXLINE);
				Fputs(buf, stdout);

				//usleep(1000000);
			}

			Close(clientfd);
			exit(0);
		}
		/*	parten process		*/
		/*else{
			for(i=0;i<num_client;i++){
				waitpid(pids[i], &status, 0);
			}
		}*/
		runprocess++;
	}
	for(i=0;i<num_client;i++){
		waitpid(pids[i], &status, 0);
	}


	/*clientfd = Open_clientfd(host, port);
	Rio_readinitb(&rio, clientfd);

	while (Fgets(buf, MAXLINE, stdin) != NULL) {
		Rio_writen(clientfd, buf, strlen(buf));
		Rio_readlineb(&rio, buf, MAXLINE);
		Fputs(buf, stdout);
	}

	Close(clientfd); //line:netp:echoclient:close
	exit(0);*/

	gettimeofday(&end, 0);		/* mark the end time */
	e_usec = ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec);
	printf("elapsed time: %5.5lf milliseconds\n", (double)e_usec/1000);

	return 0;
}
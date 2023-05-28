/* 
 * echoservers.c - A concurrent echo server based on select
 */
/* $begin echoserversmain */
#include "csapp.h"

typedef struct { /* Represents a pool of connected descriptors */
    int maxfd;        /* Largest descriptor in read_set */   
    fd_set read_set;  /* Set of all active descriptors */
    fd_set ready_set; /* Subset of descriptors ready for reading  */
    int nready;       /* Number of ready descriptors from select */   
    int maxi;         /* Highwater index into client array */
    int clientfd[FD_SETSIZE];    /* Set of active descriptors */
    rio_t clientrio[FD_SETSIZE]; /* Set of active read buffers */
} pool;

typedef struct item {
    int ID;
    int left_stock;
    int price;
    struct item *left;
    struct item *right;
}item;
item *root = NULL;
int item_num = 0, write_item_num = 0, connfd_num = 0, flag = 0;

void init_pool(int listenfd, pool *p);
void add_client(int connfd, pool *p);
void check_clients(pool *p);

item *init_item(int ID, int left_stock, int price);
item *add_item(item *node, item *newitem);
void show_item(item *node, char *temp);
item *find_item(item *node, int ID);
void read_stock_data(void);
void write_stock_data(void);
void write_item(item *node, FILE *fd);
void sigint_handler(int signo);

int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    static pool pool; 
    
    Signal(SIGINT, sigint_handler);

    if (argc != 2) {
	    fprintf(stderr, "usage: %s <port>\n", argv[0]);
	    exit(0);
    }
    listenfd = Open_listenfd(argv[1]);
    init_pool(listenfd, &pool);
    read_stock_data();

    while (1) {
	    /* Wait for listening/connected descriptor(s) to become ready */
	    pool.ready_set = pool.read_set;
	    pool.nready = Select(pool.maxfd+1, &pool.ready_set, NULL, NULL, NULL);

	    /* If listening descriptor ready, add new client to pool */
	    if (FD_ISSET(listenfd, &pool.ready_set)) {
            clientlen = sizeof(struct sockaddr_storage);
	        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
            flag = 1;
            connfd_num++;
	        add_client(connfd, &pool);
	    }
	
	    /* Echo a text line from each ready connected descriptor */ 
	    check_clients(&pool);

        if((flag == 1) & (connfd_num == 0)) {
            write_stock_data();
        }
    }
}
/* $end echoserversmain */

/* $begin init_pool */
void init_pool(int listenfd, pool *p) {
    /* Initially, there are no connected descriptors */
    int i;
    p->maxi = -1;                   
    for (i = 0; i< FD_SETSIZE; i++)  
	    p->clientfd[i] = -1;       

    /* Initially, listenfd is only member of select read set */
    p->maxfd = listenfd;           
    FD_ZERO(&p->read_set);
    FD_SET(listenfd, &p->read_set);
}
/* $end init_pool */

/* $begin add_client */
void add_client(int connfd, pool *p) {
    int i;
    p->nready--;
    for (i = 0; i < FD_SETSIZE; i++)  /* Find an available slot */
	    if (p->clientfd[i] < 0) { 
	        /* Add connected descriptor to the pool */
	        p->clientfd[i] = connfd;               
	        Rio_readinitb(&p->clientrio[i], connfd);

	        /* Add the descriptor to descriptor set */
	        FD_SET(connfd, &p->read_set);

	    /* Update max descriptor and pool highwater mark */
	        if (connfd > p->maxfd)
		        p->maxfd = connfd;
	        if (i > p->maxi)      
		        p->maxi = i;      
	        break;
	    }
    if (i == FD_SETSIZE) /* Couldn't find an empty slot */
	    app_error("add_client error: Too many clients");
}
/* $end add_client */

/* $begin check_clients */
void check_clients(pool *p) {
    int i, connfd, n, temp_ID, temp_num;
    char buf[MAXLINE];
    char temp[MAXLINE];
    item *temp_item;
    rio_t rio;

    for (i = 0; (i <= p->maxi) && (p->nready > 0); i++) {
	    connfd = p->clientfd[i];
	    rio = p->clientrio[i];
	/* If the descriptor is ready, echo a text line from it */
        if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))) { 
            p->nready--;
            if ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
                printf("Server received %d bytes on fd %d\n", n, connfd);
                //printf("%s", buf);
                if(strncmp(buf, "show", 4) == 0) {
                    strcpy(temp, "");
                    show_item(root, temp);
                    Rio_writen(connfd, temp, MAXLINE);
                }
                else if(strncmp(buf, "buy", 3) == 0) {
                    sscanf(buf, "%s %d %d", temp, &temp_ID, &temp_num);
                    temp_item = find_item(root, temp_ID);
                    if(temp_item == NULL) {
                        Rio_writen(connfd, "[buy] error: Wrong ID\n", MAXLINE);
                    }
                    else if(temp_item->left_stock >= temp_num) {
                        temp_item->left_stock -= temp_num;
                        Rio_writen(connfd, "[buy] success\n", MAXLINE);
                    }
                    else {
                        Rio_writen(connfd, "Not enough left stocks\n", MAXLINE);
                    }
                }
                else if(strncmp(buf, "sell", 4) == 0) {
                    sscanf(buf, "%s %d %d", temp, &temp_ID, &temp_num);
                    temp_item = find_item(root, temp_ID);
                    if(temp_item == NULL) {
                        Rio_writen(connfd, "[sell] error: Wrong ID\n", MAXLINE);
                    }
                    else{
                        temp_item->left_stock += temp_num;
                        Rio_writen(connfd, "[sell] success\n", MAXLINE);
                    }
                }
                else if(strncmp(buf, "exit", 4) == 0) {
                    Close(connfd);
                    FD_CLR(connfd, &p->read_set);
                    p->clientfd[i] = -1;
                    connfd_num--;
                }
            }

            /* EOF detected, remove descriptor from pool */
            else { 
                Close(connfd);
                FD_CLR(connfd, &p->read_set);
                p->clientfd[i] = -1;
                connfd_num--;
            }
        }
    }
}
/* $end check_clients */

item *init_item(int ID, int left_stock, int price) {
    item *node = (item *)malloc(sizeof(item));

    node->ID = ID;
    node->left_stock = left_stock;
    node->price = price;
    node->left = NULL;
    node->right = NULL;

    return node;
}

item *add_item(item *node, item *newitem) {
    if(node == NULL) {
        node = newitem;
        return node;
    }
    else {
        if(newitem->ID < node->ID) {
            node->left = add_item(node->left, newitem);
        }
        else {
            node->right = add_item(node->right, newitem);
        }
    }
    return node;
}

void show_item(item *node, char *temp) {
    char temp2[MAXLINE];
    if(node != NULL) {
        sprintf(temp2, "%d %d %d\n", node->ID, node->left_stock, node->price);
        strcat(temp, temp2);
        show_item(node->left, temp);
        show_item(node->right, temp);
    }
}

item *find_item(item *node, int ID) {
    if(node->ID == ID) {
        return node;
    }
    else if(node->ID < ID) {
        return find_item(node->right, ID);
    }
    else {
        return find_item(node->left, ID);
    }
    return NULL;
}

void read_stock_data(void) {
    int temp_ID, temp_left_stock, temp_price;
    item *temp;
    FILE *fd;

    fd = Fopen("stock.txt", "r");
    while(fscanf(fd, "%d %d %d", &temp_ID, &temp_left_stock, &temp_price) != EOF) {
        item_num++;
        temp = init_item(temp_ID, temp_left_stock, temp_price);
        root = add_item(root, temp);
    }
    Fclose(fd);
}

void write_stock_data(void) {
    FILE *fd;

    fd = Fopen("stock.txt", "w");
    write_item_num = 0;
    write_item(root, fd);
    Fclose(fd);
}

void write_item(item *node, FILE *fd) {
    if(node != NULL) {
        if(item_num == ++write_item_num) {
            fprintf(fd, "%d %d %d", node->ID, node->left_stock, node->price);
        }
        else {
            fprintf(fd, "%d %d %d\n", node->ID, node->left_stock, node->price);
        }
        write_item(node->left, fd);
        write_item(node->right, fd);
    }
}

void sigint_handler(int signo){
	write_stock_data();
	exit(1);
}
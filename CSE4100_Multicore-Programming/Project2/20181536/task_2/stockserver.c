/* 
 * echoservert_pre.c - A prethreaded concurrent echo server
 */
/* $begin echoservertpremain */
#include "csapp.h"
#include "sbuf.h"
#define NTHREADS  20
#define SBUFSIZE  128

typedef struct item {
    int ID;
    int left_stock;
    int price;
    int readcnt;
    sem_t mutex;
    sem_t w;
    struct item *left;
    struct item *right;
}item;
item *root = NULL;
int item_num = 0, write_item_num = 0;

void echo_cnt(int connfd);
void *thread(void *vargp);

item *init_item(int ID, int left_stock, int price);
item *add_item(item *node, item *newitem);
void show_item(item *node, char *temp);
void buy_item(int connfd, item *node, int num);
void sell_item(int connfd, item *node, int num);
item *find_item(item *node, int ID);
void read_stock_data(void);
void write_stock_data(void);
void write_item(item *node, FILE *fd);
void stock(int connfd);
void sigint_handler(int signo);

sbuf_t sbuf; /* Shared buffer of connected descriptors */

int main(int argc, char **argv) 
{
    int i, listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid; 

    Signal(SIGINT, sigint_handler);

    if (argc != 2) {
	    fprintf(stderr, "usage: %s <port>\n", argv[0]);
	    exit(0);
    }
    listenfd = Open_listenfd(argv[1]);
    read_stock_data();

    sbuf_init(&sbuf, SBUFSIZE);
    for (i = 0; i < NTHREADS; i++)  /* Create worker threads */
	    Pthread_create(&tid, NULL, thread, NULL);

    while (1) { 
        clientlen = sizeof(struct sockaddr_storage);
	    connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
	    sbuf_insert(&sbuf, connfd); /* Insert connfd in buffer */
    }
}

void *thread(void *vargp) 
{  
    Pthread_detach(pthread_self()); 
    while (1) { 
	    int connfd = sbuf_remove(&sbuf); /* Remove connfd from buffer */
	    stock(connfd);                     /* Service client */   
	    Close(connfd);
    }
}
/* $end echoservertpremain */

item *init_item(int ID, int left_stock, int price) {
    item *node = (item *)malloc(sizeof(item));

    node->ID = ID;
    node->left_stock = left_stock;
    node->price = price;
    node->readcnt = 0;
    Sem_init(&node->mutex, 0 ,1);
    Sem_init(&node->w, 0 ,1);
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
        P(&(node->mutex));
        node->readcnt++;
        if(node->readcnt == 1) {
            P(&(node->w));
        }
        V(&(node->mutex));

        sprintf(temp2, "%d %d %d\n", node->ID, node->left_stock, node->price);
        strcat(temp, temp2);

        P(&(node->mutex));
        node->readcnt--;
        if(node->readcnt == 0) {
            V(&(node->w));
        }
        V(&(node->mutex));

        show_item(node->left, temp);
        show_item(node->right, temp);
    }
}

void buy_item(int connfd, item *node, int num) {
    P(&(node->mutex));
    node->readcnt++;
    if(node->readcnt == 1) {
        P(&(node->w));
    }
    V(&(node->mutex));

    if(node->left_stock >= num) {
        P(&(node->mutex));
        node->readcnt--;
        if(node->readcnt == 0) {
            V(&(node->w));
        }
        V(&(node->mutex));

        P(&(node->w));
        node->left_stock -= num;
        V(&(node->w));
        Rio_writen(connfd, "[buy] success\n", MAXLINE);
    }
    else {
        Rio_writen(connfd, "Not enough left stocks\n", MAXLINE);

        P(&(node->mutex));
        node->readcnt--;
        if(node->readcnt == 0) {
            V(&(node->w));
        }
        V(&(node->mutex));
    }
}

void sell_item(int connfd, item *node, int num) {
    P(&(node->w));
    node->left_stock += num;
    V(&(node->w));
    Rio_writen(connfd, "[sell] success\n", MAXLINE);
}

item *find_item(item *node, int ID) {
    P(&(node->mutex));
    node->readcnt++;
    if(node->readcnt == 1) {
        P(&(node->w));
    }
    V(&(node->mutex));

    if(node->ID == ID) {
        P(&(node->mutex));
        node->readcnt--;
        if(node->readcnt == 0) {
            V(&(node->w));
        }
        V(&(node->mutex));

        return node;
    }
    else if(node->ID < ID) {
        P(&(node->mutex));
        node->readcnt--;
        if(node->readcnt == 0) {
            V(&(node->w));
        }
        V(&(node->mutex));

        return find_item(node->right, ID);
    }
    else {
        P(&(node->mutex));
        node->readcnt--;
        if(node->readcnt == 0) {
            V(&(node->w));
        }
        V(&(node->mutex));

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

void stock(int connfd) {
    int n, temp_ID, temp_num;
    char buf[MAXLINE];
    char temp[MAXLINE];
    item *temp_item;
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        printf("server received %d bytes on fd %d\n", n, connfd);
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
            else {
                buy_item(connfd, temp_item, temp_num);
            }
        }
        else if(strncmp(buf, "sell", 4) == 0) {
            sscanf(buf, "%s %d %d", temp, &temp_ID, &temp_num);
            temp_item = find_item(root, temp_ID);
            if(temp_item == NULL) {
                Rio_writen(connfd, "[sell] error: Wrong ID\n", MAXLINE);
            }
            else {
                sell_item(connfd, temp_item, temp_num);
            }
        }
        else if(strncmp(buf, "exit", 4) == 0) {
            return;
        }
    }
}

void sigint_handler(int signo){
	write_stock_data();
	exit(1);
}
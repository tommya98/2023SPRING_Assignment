#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
  int next_node;
  int distance;
  int neigbor;
  int cost;
}node;

node table[100][100];
node basic_table[100][100];
int dij_table[100][3];
FILE *fp_topology, *fp_messages, *fp_changes, *fp_output;
int node_num;
char input[1024];

void init_table(void);
void make_table(void);
void update_table(int i);
void read_topology(void);
void write_table(void);
void wrtie_message(void);
int read_change(void);

int main(int argc, char* argv[]) {
  if(argc != 4) {
    printf("usage: linkstate topologyfile messagesfile changesfile\n");
    return 0;
  }

  fp_topology = fopen(argv[1], "r");
  if(fp_topology == NULL) {
    printf("Error: open input file.\n");
    return 0;
  }
  fp_messages = fopen(argv[2], "r");
  if(fp_messages == NULL) {
    printf("Error: open input file.\n");
    fclose(fp_topology);
    return 0;
  }
  fp_changes = fopen(argv[3], "r");
  if(fp_changes == NULL) {
    printf("Error: open input file.\n");
    fclose(fp_topology);
    fclose(fp_messages);
    return 0;
  }
  fp_output = fopen("output_ls.txt", "w");
  if(fp_output == NULL) {
    printf("Error: open output file.\n");
    fclose(fp_topology);
    fclose(fp_messages);
    fclose(fp_changes);
    return 0;
  }

  read_topology();
  while(1) {
    write_table();
    wrtie_message();
    if(read_change() == 0) {
      break;
    }
  }

  fclose(fp_topology);
  fclose(fp_messages);
  fclose(fp_changes);
  fclose(fp_output);
  printf("Complete. Output file written to output_ls.txt.\n");

  return 0;
}

void init_table(void) {
  for(int i = 0; i < node_num; i++) {
    for(int j = 0; j < node_num; j++) {
      basic_table[i][j].neigbor = 0;
      if(i == j) {
        basic_table[i][j].next_node = i;
        basic_table[i][j].distance = 0;
        basic_table[i][j].cost = 0;
      }
      else {
        basic_table[i][j].next_node = -999;
        basic_table[i][j].distance = -999;
        basic_table[i][j].cost = -999;
      }
    }
  }
}

void make_table(void) {
  int next, min_dist, min_node;

  for(int i = 0; i < node_num; i++) {
    for(int j = 0; j < node_num; j++) {
      table[i][j].next_node = basic_table[i][j].next_node;
      table[i][j].distance = basic_table[i][j].distance;
      table[i][j].neigbor = basic_table[i][j].neigbor;
      table[i][j].cost = basic_table[i][j].cost;
    }
  }

  for(int i = 0; i < node_num; i++) {
    for(int j = 0; j < node_num; j++) {
      dij_table[j][0] = 999;
      dij_table[j][1] = 999;
      dij_table[j][2] = 999;
    }

    next = i;
    dij_table[i][0] = 0;
    dij_table[i][1] = 0;
    dij_table[i][2] = 1;

    for(int j = 0; j < node_num; j++) {
      min_dist = 999;
      min_node = 999;
      for(int k = 0; k < node_num; k++) {
        if(dij_table[k][2] != 1 && table[next][k].cost != -999 && table[next][k].neigbor == 1) {
          if(dij_table[next][0] + table[next][k].distance == dij_table[k][0] && next < k) {
            dij_table[k][0] = dij_table[next][0] + table[next][k].distance;
            dij_table[k][1] = next;
          }
          else if(dij_table[next][0] + table[next][k].distance < dij_table[k][0]) {
            dij_table[k][0] = dij_table[next][0] + table[next][k].distance;
            dij_table[k][1] = next;
          }
        }
        if(dij_table[k][2] != 1 && dij_table[k][0] <= min_dist) {
          if(dij_table[k][0] == min_dist && k < min_node) {
            min_dist = dij_table[k][0];
            min_node = k;
          }
          else if(dij_table[k][0] < min_dist){
            min_dist = dij_table[k][0];
            min_node = k;
          }
        }
      }
      if(j == node_num - 1 || min_node == 999) {
        break;
      }
      dij_table[min_node][2] = 1;
      next = min_node;
    }
    update_table(i);
  }

  return;
}

void update_table(int node) {
  int dis = 0;
  int parent = -1;
  int next;

  for(int i = 0; i < node_num; i++) {
    if(i == node) {
      continue;
    }
    if(dij_table[i][0] == 999) {
      table[node][i].distance = -999;
      continue;
    }

    dis = dij_table[i][0];
    next = i;
    while(1){
      parent = dij_table[next][1];
      if (parent == node) {
        break;
      }
      next = parent;
    }
    
    table[node][i].next_node = next;
    table[node][i].distance = dis;
  }
}

void read_topology(void) {
  int x, y, dis;

  fscanf(fp_topology, "%d\n", &node_num);
  init_table();

  while(fgets(input, 1024, fp_topology)) {
    sscanf(input, "%d %d %d", &x, &y, &dis);
    basic_table[x][y].next_node = y;
    basic_table[x][y].distance = dis;
    basic_table[x][y].neigbor = 1;
    basic_table[x][y].cost = dis;
    basic_table[y][x].next_node = x;
    basic_table[y][x].distance = dis;
    basic_table[y][x].neigbor = 1;
    basic_table[y][x].cost = dis;
  }
  make_table();

  return;
}

void write_table(void) {
  for(int i = 0; i < node_num; i++) {
    for(int j = 0; j < node_num; j++) {
      if(table[i][j].distance != -999) {
        fprintf(fp_output, "%d %d %d\n", j, table[i][j].next_node ,table[i][j].distance);
      }
    }
    fprintf(fp_output, "\n");
  }
  return;
}

void wrtie_message(void) {
  int x, y;
  char message[1024];

  fseek(fp_messages, 0, SEEK_SET);
  while(fgets(input, 1024, fp_messages)) {
    sscanf(input, "%d %d %[^\n]s", &x, &y, message);

    if(table[x][y].distance == -999) {
      fprintf(fp_output, "from %d to %d cost infinite hops unreachable message %s\n", x, y, message);
    }
    else {
      fprintf(fp_output, "from %d to %d cost %d hops %d ", x, y, table[x][y].distance, x);
      while(y != table[x][y].next_node) {
        fprintf(fp_output, "%d ", table[x][y].next_node);
        x = table[x][y].next_node;
      }
      fprintf(fp_output, "message %s\n", message);
    }
  }
  fprintf(fp_output, "\n");

  return;
}

int read_change(void) {
  int x, y, dis;
  
  if(fgets(input, 1024, fp_changes) == NULL) {
    return 0;
  }
  sscanf(input, "%d %d %d", &x, &y, &dis);

  if(dis == -999) {
    basic_table[x][y].next_node = -999;
    basic_table[x][y].distance = -999;
    basic_table[x][y].neigbor = 0;
    basic_table[x][y].cost = -999;
    basic_table[y][x].next_node = -999;
    basic_table[y][x].distance = -999;
    basic_table[y][x].neigbor = 0;
    basic_table[y][x].cost = -999;
  }
  else {
    basic_table[x][y].next_node = y;
    basic_table[x][y].distance = dis;
    basic_table[x][y].neigbor = 1;
    basic_table[x][y].cost = dis;
    basic_table[y][x].next_node = x;
    basic_table[y][x].distance = dis;
    basic_table[y][x].neigbor = 1;
    basic_table[y][x].cost = dis;
  }
  make_table();

  return 1;
}
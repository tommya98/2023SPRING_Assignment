#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
  int next_node;
  int distance;
}node;

node table[100][100];
node basic_table[100][100];
FILE *fp_topology, *fp_messages, *fp_changes, *fp_output;
int node_num;
char input[1024];

void init_table(void);
void make_table(void);
void read_topology(void);
void write_table(void);
void wrtie_message(void);
int read_change(void);

int main(int argc, char* argv[]) {
  if(argc != 4) {
    printf("usage: distvec topologyfile messagesfile changesfile\n");
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
  fp_output = fopen("output_dv.txt", "w");
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
  printf("Complete. Output file written to output_dv.txt.\n");

  return 0;
}

void init_table(void) {
  for(int i = 0; i < node_num; i++) {
    for(int j = 0; j < node_num; j++) {
      if(i == j) {
        basic_table[i][j].next_node = i;
        basic_table[i][j].distance = 0;
      }
      else {
        basic_table[i][j].next_node = -999;
        basic_table[i][j].distance = -999;
      }
    }
  }
}

void make_table(void) {
  for(int i = 0; i < node_num; i++) {
    for(int j = 0; j < node_num; j++) {
      table[i][j].next_node = basic_table[i][j].next_node;
      table[i][j].distance = basic_table[i][j].distance;
    }
  }

  while(1) {
    int update = 0;
    for(int i = 0; i < node_num; i++) {
      for(int j = 0; j < node_num; j++) {
        for(int k = 0; k < node_num; k++) {
          if(j != table[i][j].next_node || i == j) {
            continue;
          }
          if(table[i][k].distance == -999 || table[i][k].distance > table[i][j].distance + table[j][k].distance) {
            if(table[j][k].distance == -999) {
              continue;
            }
            table[i][k].next_node = j;
            table[i][k].distance = table[i][j].distance + table[j][k].distance;
            update = 1;
          }
        }
      }
    }
    if(update == 0) {
      break;
    }
  }

  return;
}

void read_topology(void) {
  int x, y, dis;

  fscanf(fp_topology, "%d\n", &node_num);
  init_table();

  while(fgets(input, 1024, fp_topology)) {
    sscanf(input, "%d %d %d", &x, &y, &dis);
    basic_table[x][y].next_node = y;
    basic_table[x][y].distance = dis;
    basic_table[y][x].next_node = x;
    basic_table[y][x].distance = dis;
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
    basic_table[y][x].next_node = -999;
    basic_table[y][x].distance = -999;
  }
  else {
    basic_table[x][y].next_node = y;
    basic_table[x][y].distance = dis;
    basic_table[y][x].next_node = x;
    basic_table[y][x].distance = dis;
  }
  make_table();

  return 1;
}
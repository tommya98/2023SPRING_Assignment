#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp1, *fp2;
unsigned int dataword_size, file_len, gen_len, code_len, padding, first = 1;

void init_data(char *data, char c, int flag);
void modulo_2_div(char *data, char *generator);
void write_file(char *data);

int main(int argc, char *argv[]) {

  if(argc != 5) {
    printf("usage: ./crc_encoder input_file output_file generator dataword_size\n");
    return 0;
  }

  fp1 = fopen(argv[1], "rb");
  if(fp1 == NULL) {
    printf("input file open error.\n");
    return 0;
  }

  fp2 = fopen(argv[2], "wb");
  if(fp2 == NULL) {
    fclose(fp1);
    printf("output file open error.\n");
    return 0;
  }

  fseek(fp1, 0, SEEK_END);
  file_len = ftell(fp1);
  fseek(fp1, 0, SEEK_SET);
  gen_len = strlen(argv[3]);
  dataword_size = atoi(argv[4]);
  code_len = dataword_size + gen_len - 1;

  char *data = (char *)malloc(code_len);
  char c;

  if(dataword_size == 4) {
    padding = (8 - ((code_len * file_len * 2) % 8)) % 8;
    while((c = fgetc(fp1)) != EOF) {
      init_data(data, c, 0);
      modulo_2_div(data, argv[3]);
      write_file(data);

      init_data(data, c, 1);
      modulo_2_div(data, argv[3]);
      write_file(data);
    }
  }
  else if(dataword_size == 8) {
    padding = (8 - ((code_len * file_len) % 8)) % 8;
    while((c = fgetc(fp1)) != EOF) {
      init_data(data, c, 2);
      modulo_2_div(data, argv[3]);
      write_file(data);
    }
  }
  else {
    printf("dataword size must be 4 or 8.\n");
    fclose(fp1);
    fclose(fp2);
    free(data);
    return 0;
  }

  free(data);
  fclose(fp1);
  fclose(fp2);
  return 0;
}

void init_data(char *data, char c, int flag) {
  for(int i = 0; i < code_len; i++) {
    data[i] = '0';
  }
  if(flag == 0) {
    if(c & 0b10000000) {
      data[0] = '1';
    }
    if(c & 0b01000000) {
      data[1] = '1';
    }
    if(c & 0b00100000) {
      data[2] = '1';
    }
    if(c & 0b00010000) {
      data[3] = '1';
    }
  }
  else if(flag == 1) {
    if(c & 0b00001000) {
      data[0] = '1';
    }
    if(c & 0b00000100) {
      data[1] = '1';
    }
    if(c & 0b00000010) {
      data[2] = '1';
    }
    if(c & 0b00000001) {
      data[3] = '1';
    }
  }
  else if(flag == 2) {
    if(c & 0b10000000) {
      data[0] = '1';
    }
    if(c & 0b01000000) {
      data[1] = '1';
    }
    if(c & 0b00100000) {
      data[2] = '1';
    }
    if(c & 0b00010000) {
      data[3] = '1';
    }
    if(c & 0b00001000) {
      data[4] = '1';
    }
    if(c & 0b00000100) {
      data[5] = '1';
    }
    if(c & 0b00000010) {
      data[6] = '1';
    }
    if(c & 0b00000001) {
      data[7] = '1';
    }    
  }
}

void modulo_2_div(char *data, char *generator) {
  char *data_copy;
  data_copy = (char *)malloc(code_len);
  memcpy(data_copy, data, code_len);

  for(int i = 0; i < dataword_size; i++) {
    if(data_copy[i] == '0') {
      continue;
    }
    for(int j = 0; j < gen_len; j++) {
      if(data_copy[i + j] == generator[j]) {
        data_copy[i + j] = '0';
      }
      else {
        data_copy[i + j] = '1';
      }
    }
  }

  for(int i = dataword_size; i < code_len; i++) {
    data[i] = data_copy[i];
  }

  free(data_copy);
}

void write_file(char *data) {
  static unsigned char c = 0;
  static int flag;

  if(first == 1) {
    fprintf(fp2, "%c", padding);
    flag = padding;
    first = 0;    
  }

  for(int i = 0; i < code_len; i++) {
    if(data[i] == '1') {
      c |= (1 << (7 - flag));
    }
    else {
      c &= ~(1 << (7 - flag));
    }

    flag++;
    if(flag == 8) {
      fprintf(fp2, "%c", c);
      flag = 0;
    }
  }
}
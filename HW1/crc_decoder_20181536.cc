#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp1, *fp2, *fp3;
unsigned int dataword_size, gen_len, code_len, file_len, padding, first = 1, stop = 0;
unsigned long long word = 0, error = 0;

void read_file(char *data);
void modulo_2_div(char *data, char *generator);
char make_c(char *data, char c, int flag);
void modify_temp(char *temp);

int main(int argc, char *argv[]) {
  if(argc != 6) {
    printf("usage: ./crc_decoder input_file output_file result_file generator dataword_size\n");
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

  fp3 = fopen(argv[3], "wb");
  if(fp3 == NULL) {
    fclose(fp1);
    fclose(fp2);
    printf("result file open error.\n");
    return 0;
  }
  
  fseek(fp1, 0, SEEK_END);
  file_len = ftell(fp1);
  fseek(fp1, 0, SEEK_SET);
  gen_len = strlen(argv[4]);
  dataword_size = atoi(argv[5]);
  code_len = dataword_size + gen_len - 1;
  padding = fgetc(fp1);

  char *data = (char *)malloc(code_len);
  char *temp = (char *)malloc(code_len);
  char *temp2 = (char *)malloc(code_len);
  char c;

  if(dataword_size == 4) {
    while(1) {
      read_file(data);
      if(stop == 1) {
        word--;
        break;
      }
      memcpy(temp, data, code_len);
      modulo_2_div(temp, argv[4]);
      memcpy(temp2, data, code_len);
      modify_temp(temp2);
      if((strncmp(temp, temp2, code_len) != 0)) {
        error++;
      }
      c = make_c(data, c, 0);

      read_file(data);
      memcpy(temp, data, code_len);
      modulo_2_div(temp, argv[4]);
      memcpy(temp2, data, code_len);
      modify_temp(temp2);
      if((strncmp(temp, temp2, code_len) != 0)) {
        error++;
      }
      c = make_c(data, c, 1);

      fprintf(fp2, "%c", c);
    }
  }
  else if(dataword_size == 8) {
    while(1) {
      read_file(data);
      if(stop == 1) {
        word--;
        break;
      }
      memcpy(temp, data, code_len);
      modulo_2_div(temp, argv[4]);
      memcpy(temp2, data, code_len);
      modify_temp(temp2);
      if(strncmp(temp, temp2, code_len) != 0) {
        error++;
      }
      c = make_c(data, c, 2);
      fprintf(fp2, "%c", c);
    }
  }
  else {
    printf("dataword size must be 4 or 8.\n");
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    free(data);
    free(temp);
    free(temp2);
    return 0;    
  }

  fprintf(fp3, "%lld %lld", word, error);

  fclose(fp1);
  fclose(fp2);
  fclose(fp3);
  free(data);
  free(temp);
  free(temp2);
  return 0;
}

void read_file(char *data) {
  static char c;
  static int flag;

  word++;
  if(first == 1) {
    flag = padding;
    first = 0;
    c = fgetc(fp1);
  }

  for(int i = 0; i < code_len; i++) {
    if(flag == 8) {
      if(ftell(fp1) == file_len) {
        stop = 1;
        return;
      }
      c = fgetc(fp1);
      flag = 0;
    }

    if((c & (1 << (7 - flag))) != 0) {
      data[i] = '1';
    }
    else {
      data[i] = '0';
    }
    flag++;
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

char make_c(char *data, char c, int flag) {
  if(flag == 0) {
    c &= 0b00001111;

    if(data[0] == '1') {
      c |= 0b10000000;
    }
    if(data[1] == '1') {
      c |= 0b01000000;
    }
    if(data[2] == '1') {
      c |= 0b00100000;
    }
    if(data[3] == '1') {
      c |= 0b00010000;
    }
  }
  else if(flag == 1) {
    c &= 0b11110000;

    if(data[0] == '1') {
      c |= 0b00001000;
    }
    if(data[1] == '1') {
      c |= 0b00000100;
    }
    if(data[2] == '1') {
      c |= 0b00000010;
    }
    if(data[3] == '1') {
      c |= 0b00000001;
    }
  }
  else if(flag == 2) {
    c &= 0b00000000;

    if(data[0] == '1') {
      c |= 0b10000000;
    }
    if(data[1] == '1') {
      c |= 0b01000000;
    }
    if(data[2] == '1') {
      c |= 0b00100000;
    }
    if(data[3] == '1') {
      c |= 0b00010000;
    }
    if(data[4] == '1') {
      c |= 0b00001000;
    }
    if(data[5] == '1') {
      c |= 0b00000100;
    }
    if(data[6] == '1') {
      c |= 0b00000010;
    }
    if(data[7] == '1') {
      c |= 0b00000001;
    }
  }

  return c;
}

void modify_temp(char *temp) {
  for(int i = dataword_size; i < code_len; i++) {
    temp[i] = '0';
  }
}

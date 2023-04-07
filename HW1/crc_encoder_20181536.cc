#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int modulo_2_div(int dividend, int divisor);
char* int_to_binary(int n);
int binary_to_int(char *s);
void write_file(int s);

FILE *fp1, *fp2;
int dataword_size;
unsigned int generator = 0, gen_len, file_len = 0, code_len;
char padding = 0, first = 1;

int main(int argc, char *argv[]) {
  if(argc != 5) { // 인자의 수 확인
    printf("usage: ./crc_encoder input_file output_file generator dataword_size\n");
    return 0;
  }

  fp1 = fopen(argv[1], "r"); // input_file open
  if(fp1 == NULL) {
    printf("input file open error.\n");
    return 0;
  }

  fp2 = fopen(argv[2], "w"); // output_file open
  if(fp2 == NULL) {
    fclose(fp1);
    printf("output file open error.\n");
    return 0;
  }

  while(1) { // 전체 문자 개수 file_len에 저장
    char c = fgetc(fp1);
      if(c == EOF) {
        break;
    }
    file_len++;
  }
  fseek(fp1, 0, SEEK_SET);

  gen_len = strlen(argv[3]); // generator 길이 gen_len에 저장 후 generator int로 저장
  for(int i = 0; i < gen_len; i++) {
    generator |= ((argv[3][i] - 48) << (gen_len - i - 1));
  }
  dataword_size = atoi(argv[4]);
  code_len = dataword_size + gen_len - 1;

  if(dataword_size == 4) { //dataword_size가 4일 때
    int data1, data2;
    padding = 8 - ((code_len * file_len * 2) % 8);
    while(1) {
      char c = fgetc(fp1);
      if(c == EOF) {
        break;
      }
      data1 = (c >> 4) << (gen_len - 1);
      data1 = modulo_2_div(data1, generator);
      write_file(data1);
      data2 = (c & 0b00001111) << (gen_len - 1);
      data2 = modulo_2_div(data2, generator);
      write_file(data2);
    }
  }
  else if(dataword_size == 8) { //dataword_size가 8일 때
    int data;
    padding = 8 - ((code_len * file_len) % 8);
    while(1) {
      char c = fgetc(fp1);
      if(c == EOF) {
        break;
      }
      data = c << (gen_len - 1);
      data = modulo_2_div(data, generator);
      write_file(data);
    }
  }
  else { //data_word 오류 확인
    printf("dataword size must be 4 or 8.\n");
    fclose(fp1);
    fclose(fp2);
    return 0;
  }

  fclose(fp1);
  fclose(fp2);

  return 0;
}

int modulo_2_div(int dividend, int divisor) { //dataword가 정수로 들어오면 codeword를 정수로 내보낸다.
  char *dividend_char = int_to_binary(dividend);
  char *divisor_char = int_to_binary(divisor);
  char *generator_char = (char*)malloc(gen_len);
  char *result_char = (char*)malloc(gen_len - 1);
  int size = 32, result;

  for(int i = 0; i < gen_len; i++) {
    generator_char[i] = divisor_char[32 - gen_len + i];
  }

  for(int i = 0; i <= 32 - gen_len; i++) {
    if(dividend_char[i] == '0') {
      continue;
    }
    for(int j = 0; j < gen_len; j++) {
      if(dividend_char[i + j] == generator_char[j]) {
        dividend_char[i + j] = '0';
      }
      else {
        dividend_char[i + j] = '1';
      }
    }
  }

  for(int i = 0; i < gen_len - 1; i++) {
    result_char[i] = dividend_char[33 - gen_len + i];
  }
  result = binary_to_int(result_char);

  free(dividend_char);
  free(divisor_char);
  free(generator_char);
  free(result_char);

  return dividend + result;
}

char* int_to_binary(int n) { //정수를 binary 문자열로 변환
  char* s = (char*)malloc(32);
  memset(s, '0', 32);

  for(int i = 31; i >= 0; i--) {
    if(n & (1 << i)) {
      s[31 - i] = '1';
    }
  }

  return s;
}

int binary_to_int(char *s) { //binary 문자열을 정수로 변환
  int len = strlen(s);
  int result = 0, two = 1;
  
  for(int i = len - 1; i >= 0; i--) {
    if(s[i] == '1') {
      result += two;
    }
    two *= 2;
  }
    
  return result;
}

void write_file(int n) { //codeword를 파일에 출력
  static unsigned char c = 0;
  static int flag = 0;
  int n_flag = 32 - code_len;
  char *n_char = int_to_binary(n);

  if(first == 1) {
    fprintf(fp2, "%c", padding);
    flag += padding;
    first = 0;
  }

  while(1) {
    if(n_char[n_flag] == '1') {
      c |= (1 << (7 - flag));
    }
    else {
      c &= ~(1 << (7 - flag));
    }

    flag++;
    n_flag++;

    if((n_flag == 32)) {
      if(flag == 8) {
        fprintf(fp2, "%c", c);
        flag = 0;
      }
      break;
    }
    if(flag == 8) {
      fprintf(fp2, "%c", c);
      flag = 0;
    }
  }

  free(n_char);
}
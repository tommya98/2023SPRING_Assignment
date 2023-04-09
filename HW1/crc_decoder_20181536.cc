#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_file(void);
int modulo_2_div(int dividend, int divisor);
char* int_to_binary(int n);
int binary_to_int(char *s);


FILE *fp1, *fp2, *fp3;
char padding = 0, first = 1;
int dataword_size, error = 0, word = -1;
unsigned int generator = 0, gen_len, code_len;

int main(int argc, char *argv[]) {
  if(argc != 6) { // 인자의 수 확인
    printf("usage: ./crc_decoder input_file output_file result_file generator dataword_size\n");
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

  fp3 = fopen(argv[3], "w"); // result_file open
  if(fp3 == NULL) {
    fclose(fp1);
    fclose(fp2);
    printf("result file open error.\n");
    return 0;
  } 

  gen_len = strlen(argv[4]); // generator 길이 gen_len에 저장 후 generator int로 저장
  for(int i = 0; i < gen_len; i++) {
    generator |= ((argv[4][i] - 48) << (gen_len - i - 1));
  }
  dataword_size = atoi(argv[5]);
  code_len = dataword_size + gen_len - 1;
  padding = fgetc(fp1);

  if(dataword_size == 4) {
    int data1, data2, temp1, temp2;
    char c = 0;
    while(1) {
      data1 = read_file();
      if(data1 == -1) {
        break;
      }
      data2 = read_file();
      
      temp1 = modulo_2_div(data1, generator);
      if(temp1 != data1) {
        error++;
      }
      temp2 = modulo_2_div(data2, generator);
      if(temp2 != data2) {
        error++;
      }
      c = ((data1 >> (gen_len - 1)) << dataword_size) + (data2 >> (gen_len - 1));
      fprintf(fp2, "%c", c);
    }
  }
  else if(dataword_size == 8) {
    int data, temp;
    char c = 0;
    while(1) {
      data = read_file();
      if(data == -1) {
        break;
      }

      temp = modulo_2_div(data,generator);
      if(temp != data) {
        error++;
      }
      c = data >> (gen_len - 1);
      fprintf(fp2, "%c", c);
    }
  }
  else {
    printf("dataword size must be 4 or 8.\n");
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    return 0;
  }

  fprintf(fp3, "%d %d", word, error);

  fclose(fp1);
  fclose(fp2);
  fclose(fp3);

  return 0;
}

int read_file(void) { //여기만 짜면 됨
  static unsigned char c = 0; 
  static int flag = 0;
  int data = 0; //codeword
  int d_flag = 0;

  word++;
  if(first == 1) {
    flag += padding;
    first = 0;
    c = fgetc(fp1);
  }
  
  while(1) {
    if(d_flag == code_len) {
      return data;
    }
    if(flag == 8) {
      c = fgetc(fp1);
      if(c == 255) {
        return -1;
      }
      flag = 0;
    }

    if((c & (1 << (7 - flag))) != 0) {
      data += (1 << (code_len - 1 - d_flag));
    }
    d_flag++;
    flag++;
  }
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

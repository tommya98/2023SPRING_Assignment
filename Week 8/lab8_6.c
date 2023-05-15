#include <stm32f10x.h>

void print_num(u32 n);

int main(void) {
	u32 i, j;
	
  RCC->APB2ENR = 0x0000001D;

  GPIOA->CRL = 0x33333333;
  GPIOA->CRH = 0x33330000;

  while(1) {
		for(i = 0; i < 9; i++) {
			print_num(i);
			for(j = 0; j < 3000000; j++);
		}
  }
}

void print_num(u32 n) {
  switch(n) {
    case 0:
      GPIOA->ODR = 0xFF03;
      break;
    case 1:
			GPIOA->ODR = 0xFF9F;
      break;
    case 2:
			GPIOA->ODR = 0xFF25;
      break;
    case 3:
			GPIOA->ODR = 0xFF0D;
      break;
    case 4:
			GPIOA->ODR = 0xFF99;
      break;
    case 5:
			GPIOA->ODR = 0xFF49;
      break;
    case 6:
			GPIOA->ODR = 0xFF41;
      break;
    case 7:
			GPIOA->ODR = 0xFF1B;
      break;
    case 8:
			GPIOA->ODR = 0xFF01;
      break;
    case 9:
			GPIOA->ODR = 0xFF09;
      break;
  }
}
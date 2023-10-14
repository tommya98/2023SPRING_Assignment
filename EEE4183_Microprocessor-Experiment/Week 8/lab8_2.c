#include <stm32f10x.h>

u32 i, row, col;

int main(void) {
  
  RCC->APB2ENR = 0x0000001D;
  GPIOC->CRL = 0x33333333;
  GPIOB->CRH = 0x33333333;

  row = 0x0000;
  col = 0x0001;

  while(1) {
    GPIOC->ODR = row;
    GPIOB->ODR = col << 8;

    for(i = 0; i < 300000; i++){;}

    col = col << 1;
    if(col == 0x0100) col = 0x0001;
  }
}
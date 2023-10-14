#include <stm32f10x.h>

int i;
char string[10];

int main(void) {
  RCC->APB2ENR = 0x00004004;
  GPIOA->CRH &= ~(0xFFu << 4);
  GPIOA->CRH |= (0x04B << 4);

  USART1->BRR = 0x0EA6;
  USART1->CR1 = 0x00000000;
  USART1->CR2 = 0x00000000;
  USART1->CR3 = 0x00000000;
  USART1->CR1 |= 0x00000004;

  USART1->CR1 |= 0x00002000;

  i = 0;
  while(1) {
    while(!(USART1->SR & 0x020));
    string[i++] = USART1->DR;
  }

  while(1) {}
  
} //RX

// #include <stm32f10x.h>

// int i;
// char string[10] = "20221234\n";

// int main(void) {
//   RCC->APB2ENR = 0x00004004;
//   GPIOA->CRH &= ~(0xFFu << 4);
//   GPIOA->CRH |= (0x04B << 4);

//   USART1->BRR = 0x0EA6;
//   USART1->CR1 = 0x00000000;
//   USART1->CR2 = 0x00000000;
//   USART1->CR3 = 0x00000000;
//   USART1->CR1 |= 0x00000008;

//   USART1->CR1 |= 0x00002000;

//   i = 0;
//   while(string[i] != '\n') {
//     while(!(USART1->SR & 0x080));
//     USART1->DR = string[i++];
//   }

//   while(1) {}
  
// } //TX
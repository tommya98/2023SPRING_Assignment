#include <stm32f10x.h>

int txcnt = 0, rxcnt = 0;
char rxstring[10], txstring[10] = "12342022\n";

#define NULL 0

#define USART_EVENPARITY_SETTING 0x400
#define USART_ODDPARITY_SETTING 0x600
#define USART_WORDLENGTH_SETTING 0x1000

#define USART_PE_INTERRUPT 0x100
#define USART_TXE_INTERRUPT 0x80
#define USART_TC_INTERRUPT 0x40
#define USART_RXNE_INTERRUPT 0x20
#define USART_IDEL_INTERRUPT 0x10

void USART1_Init(int baud, int setting, int interrupt) {
  RCC->APB2ENR |= 0x00004004;
  GPIOA->CRH &= ~(0xFFu << 4);
  GPIOA->CRH |= (0x04B << 4);

  double temp = (double)4500000 / baud;
  USART1->BRR = temp * (1 << 4);

  USART1->CR1 |= setting;
  USART1->CR1 |= interrupt;
  if(interrupt) {
    NVIC->ISER[1] |= (1 << 5);
  }

  USART1->CR1 |= 0x0000200C;
}

int main(void) {
  RCC->APB2ENR = 0x00000011;
  GPIOC->CRH = 0x00800000;
  EXTI->FTSR = 0x2000;
  EXTI->IMR = 0x2000;
  AFIO->EXTICR[3] = 0x20;
  NVIC->ISER[1] = (1 << 8);

  USART1_Init(9600, NULL, USART_RXNE_INTERRUPT);

  while(1) {}
} //end main

void USART1_IRQHandler(void) {
  if(USART1->SR & 0x80) {
    if(txstring[txcnt] == '\n')
      USART1->CR1 &= ~0x80;
    else
      USART1->DR = txstring[txcnt++];
  }
  if(USART1->SR & 0x20) {
    rxstring[rxcnt++] = USART1->DR;
	}
}

void EXTI15_10_IRQHandler(void) {
  USART1->CR1 |= 0x00000080;
  EXTI->PR = 0x2000;
}
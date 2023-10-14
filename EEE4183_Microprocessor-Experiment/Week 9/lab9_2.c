#include <stm32f10x.h>

int main(void) {
  
  RCC->APB2ENR |= 0x000005;
  GPIOA->CRL = 0x00300008;
  GPIOA->ODR = 0x01;

  RCC->APB1ENR |= 0x00000001;
  TIM2->CR1 = 0x04;
  TIM2->CR2 = 0x00;
  TIM2->PSC = 0x5FFF;
  TIM2->ARR = 0x1FFF;
  TIM2->DIER = 0x0041;

  TIM2->SMCR = 0x8074;

  NVIC->ISER[0] = (1 << 28);
  TIM2->CR1 |= 0x0001;

  while(1) {}
		
} //end main

void TIM2_IRQHandler(void) {
  if((TIM2->SR & 0x0040) != 0) {
    GPIOA->BSRR = 1 << 5;
    TIM2->SR &= ~(1 << 6); //clear TIF
  }
  if((TIM2->SR & 0x0001) != 0) {
    GPIOA->BSRR = 1 << 21;
    TIM2->SR &= ~(1 << 0); //clar UIF
  }
}
#include <stm32f10x.h>

int main(void) {

  RCC->APB2ENR = 0x00000005;
  GPIOA->CRL = 0x00300008;
  GPIOA->ODR = 0x01;

  RCC->APB1ENR |= 0x00000001;
  TIM2->CR1 = 0x80;
  TIM2->CR2 = 0x00;
  TIM2->PSC = 7199;
  TIM2->ARR = 9999;
  TIM2->DIER = 0x0003;

  TIM2->CCMR1 = 0x0068;
  TIM2->CCER = 0x0001;
  TIM2->CCR1 = 9000;

  NVIC->ISER[0] = (1 << 28);
  TIM2->CR1 |= 0x0001;

  while(1) {}
		
} //end main

void TIM2_IRQHandler(void) {
  if((TIM2->SR & 0x0002) != 0) {
		GPIOA->BSRR = 1 << 21;
    TIM2->SR &= ~(1 << 1); //clear UIF
  }
  if((TIM2->SR & 0x0001) != 0) {
		GPIOA->BSRR = 1 << 5;
    TIM2->SR &= ~(1 << 0); //clear UIF
  }
}

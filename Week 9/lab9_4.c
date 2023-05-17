#include <stm32f10x.h>

//TIM_timer
#define TIM11 1
#define TIM22 2
#define TIM33 3

//TIM_interrupt
#define TIM_UPDATE_INT 0x01
#define TIM_TRIGGER_INT 0x02

//TIM_flag
#define TIM_BASIC 1
#define TIM_RESET_MODE 2
#define TIM_GATED_MODE 3
#define TIM_EXTERNAL_CLOCK_MODE2 4

void my_TIM_configuration(int timer, int psc, int arr, int interrupt, int flag) {
  switch(timer) {
    case TIM11:
      RCC->APB2ENR |= 0x00000800;
      TIM1->PSC = psc;
      TIM1->ARR = arr;

      if(interrupt & TIM_UPDATE_INT) {
        TIM1->DIER |= 0x0001;
        NVIC->ISER[0] |= 0x02000000;
      }
      if(interrupt & TIM_TRIGGER_INT) {
        TIM1->DIER |= 0x0040;
        NVIC->ISER[0] |= 0x04000000;
      }

      switch(flag) {
        case TIM_BASIC: 
          TIM1->CR1 = 0x00;
          TIM1->CR2 = 0x00;
          TIM1->SMCR = 0x00;
          break;
        case TIM_RESET_MODE: 
          TIM1->CR1 = 0x04;
          TIM1->CR2 = 0x00;
          TIM1->SMCR = 0x8074;
          break;
        case TIM_GATED_MODE: 
          TIM1->CR1 = 0x04;
          TIM1->CR2 = 0x00;
          TIM1->SMCR = 0x0075;
          break;
        case TIM_EXTERNAL_CLOCK_MODE2: 
          TIM1->CR1 = 0x04;
          TIM1->CR2 = 0x00;
          TIM1->SMCR = 0x4077;
          break;
      }

      TIM1->CR1 |= 0x0001;
      break;


    case TIM22:
      RCC->APB1ENR |= 0x00000001;
      TIM2->PSC = psc;
      TIM2->ARR = arr;

      if(interrupt & TIM_UPDATE_INT) {
        TIM2->DIER |= 0x0001;
        NVIC->ISER[0] |= 0x10000000;
      }
      if(interrupt & TIM_TRIGGER_INT) {
        TIM2->DIER |= 0x0040;
        NVIC->ISER[0] |= 0x10000000;
      }

      switch(flag) {
        case TIM_BASIC: 
          TIM2->CR1 = 0x00;
          TIM2->CR2 = 0x00;
          TIM2->SMCR = 0x00;
          break;
        case TIM_RESET_MODE: 
          TIM2->CR1 = 0x04;
          TIM2->CR2 = 0x00;
          TIM2->SMCR = 0x8074;
          break;
        case TIM_GATED_MODE: 
          TIM2->CR1 = 0x04;
          TIM2->CR2 = 0x00;
          TIM2->SMCR = 0x0075;
          break;
        case TIM_EXTERNAL_CLOCK_MODE2: 
          TIM2->CR1 = 0x04;
          TIM2->CR2 = 0x00;
          TIM2->SMCR = 0x4077;
          break;
      }

      TIM2->CR1 |= 0x0001;
      break;


    case TIM33:
      RCC->APB1ENR |= 0x00000002;
      TIM3->PSC = psc;
      TIM3->ARR = arr;

      if(interrupt & TIM_UPDATE_INT) {
        TIM3->DIER |= 0x0001;
        NVIC->ISER[0] |= 0x20000000;
      }
      if(interrupt & TIM_TRIGGER_INT) {
        TIM3->DIER |= 0x0040;
        NVIC->ISER[0] |= 0x20000000;
      }

      switch(flag) {
        case TIM_BASIC: 
          TIM3->CR1 = 0x00;
          TIM3->CR2 = 0x00;
          TIM3->SMCR = 0x00;
          break;
        case TIM_RESET_MODE: 
          TIM3->CR1 = 0x04;
          TIM3->CR2 = 0x00;
          TIM3->SMCR = 0x8074;
          break;
        case TIM_GATED_MODE: 
          TIM3->CR1 = 0x04;
          TIM3->CR2 = 0x00;
          TIM3->SMCR = 0x0075;
          break;
        case TIM_EXTERNAL_CLOCK_MODE2: 
          TIM3->CR1 = 0x04;
          TIM3->CR2 = 0x00;
          TIM3->SMCR = 0x4077;
          break;
      }

      TIM3->CR1 |= 0x0001;
      break;
  }
}

int main(void) {
  RCC->APB2ENR = 0x00000005;
  GPIOA->CRL = 0x00300008;
  GPIOA->ODR = 0x01;

  my_TIM_configuration(TIM22, 0, 10, TIM_UPDATE_INT, TIM_EXTERNAL_CLOCK_MODE2);

  while(1) {}
		
} //end main

void TIM2_IRQHandler(void) {
  if((TIM2->SR & 0x0001) != 0) {
    if(GPIOA->IDR & 0x20) GPIOA->BSRR = 1 << 21;
    else GPIOA->BSRR = 1 << 5;
    TIM2->SR &= ~(1 << 0); //clear UIF
  }
}
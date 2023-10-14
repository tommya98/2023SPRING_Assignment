// lED(좌)  : PC0 - PC3
// LED(우)  : PC4 - PC7
// L_SENSOR : PA10
// R_SENSOR : PA11
// EMERG    : PA12

#include <stm32f10x.h>

void init_port(void);
void init_timer(void);
void left_opening_timer(void);
void left_closing_timer(void);
void right_opening_timer(void);
void right_closing_timer(void);

volatile int left_door = 0, right_door = 0, left_opening = 0, left_closing = 0, right_opening = 0, right_closing = 0;
volatile int emerg_left_opening_flag = 0, emerg_right_opening_flag = 0, emerg_opened_flag, emerg_closing_flag;

int main(void) {
  init_port();
  init_timer();

  while(1) {
    if(left_door == 0) {
			GPIOC->BSRR = 0x000F0000;
		}
		else if(left_door == 1) {
			GPIOC->BSRR = 0x00070008;
		}
		else if(left_door == 2) {
			GPIOC->BSRR = 0x0003000C;
		}
		else if(left_door == 3) {
			GPIOC->BSRR = 0x0001000E;
		}
		else if(left_door == 4) {
			GPIOC->BSRR = 0x0000000F;
		}
		
    if(right_door == 0) {
			GPIOC->BSRR = 0x00F00000;
		}
		else if(right_door == 1) {
			GPIOC->BSRR = 0x00E00010;
		}
		else if(right_door == 2) {
			GPIOC->BSRR = 0x00C00030;
		}
		else if(right_door == 3) {
			GPIOC->BSRR = 0x00800070;
		}
		else if(right_door == 4) {
			GPIOC->BSRR = 0x000000F0;
		}
		
		if(GPIOA->IDR & 0x00000400) {
			TIM4->CNT = 0x00000000;
		}
		if(GPIOA->IDR & 0x00000800) {
			TIM1->CNT = 0x00000000;
		}
  }
  return 0;
}

void init_port(void) {
  RCC->APB2ENR |= 0x00000015;
  GPIOC->CRL = 0x33333333;
  GPIOA->CRH = 0x00088800;
  GPIOA->ODR = 0x00000000;

  EXTI->RTSR = 0x00001C00;
  EXTI->IMR = 0x00001C00;
  AFIO->EXTICR[2] = 0x00000000;
  AFIO->EXTICR[3] = 0x00000000;
  NVIC->ISER[1] |= 0x00000100;

  return;
}

void init_timer(void){
  RCC->APB1ENR |= 0x00000007;
  RCC->APB2ENR |= 0x00000800;

  TIM2->CR1 = 0x00000004;
  TIM2->CR2 = 0x00000000;
  TIM2->PSC = 0x00001C1F;
  TIM2->ARR = 0x000003E7;
  TIM2->DIER = 0x00000001;

  TIM3->CR1 = 0x00000004;
  TIM3->CR2 = 0x00000000;
  TIM3->PSC = 0x00001C1F;
  TIM3->ARR = 0x000003E7;
  TIM3->DIER = 0x00000001;

  TIM4->CR1 = 0x00000004;
  TIM4->CR2 = 0x00000000;
  TIM4->PSC = 0x00001C1F;
  TIM4->ARR = 0x00004E1F;
  TIM4->DIER = 0x00000001;

  TIM1->CR1 = 0x00000004;
  TIM1->CR2 = 0x00000000;
  TIM1->PSC = 0x00001C1F;
  TIM1->ARR = 0x00004E1F;
  TIM1->DIER = 0x00000001;

  NVIC->ISER[0] = 0x72000000;

  return;
}

void left_opening_timer(void) {
	TIM2->CR1 &= ~0x00000001;
  left_opening = 1;
  left_closing = 0;
  TIM2->CNT = 0x00000000;
  TIM2->ARR = 0x000003E7; //0.1초
  TIM2->CR1 |= 0x00000001;
}

void left_closing_timer(void) {
	TIM2->CR1 &= ~0x00000001;
  left_opening = 0;
  left_closing = 1;
  TIM2->CNT = 0x00000000;
  TIM2->ARR = 0x000007CF; //0.2초
  TIM2->CR1 |= 0x00000001;
}

void right_opening_timer(void) {
	TIM3->CR1 &= ~0x00000001;
  right_opening = 1;
  right_closing = 0;
  TIM3->CNT = 0x00000000;
  TIM3->ARR = 0x000003E7;
  TIM3->CR1 |= 0x00000001;
}

void right_closing_timer(void){
	TIM3->CR1 &= ~0x00000001;
  right_opening = 0;
  right_closing = 1;
  TIM3->CNT = 0x00000000;
  TIM3->ARR = 0x000007CF;
  TIM3->CR1 |= 0x00000001;
}

void EXTI15_10_IRQHandler(void) {
  if(EXTI->PR & 0x00000400) { //L_SEONSOR
    if((emerg_left_opening_flag || emerg_right_opening_flag || emerg_opened_flag || emerg_closing_flag) == 0) {
      left_opening_timer();
    }
    EXTI->PR = 0x00000400;
  }

  if(EXTI->PR & 0x00000800) { //R_SENSOR
    if((emerg_left_opening_flag || emerg_right_opening_flag || emerg_opened_flag || emerg_closing_flag) == 0) {
      right_opening_timer();
    }
    EXTI->PR = 0x00000800;
  }

  if(EXTI->PR & 0x00001000) { //EMERG
    if(emerg_opened_flag == 1) {
      emerg_left_opening_flag = 0;
			emerg_right_opening_flag = 0;
      emerg_opened_flag = 0;
      emerg_closing_flag = 1;
      left_closing_timer();
      right_closing_timer();
    }
    else {
      emerg_left_opening_flag = 1;
			emerg_right_opening_flag = 1;
      emerg_opened_flag = 0;
      emerg_closing_flag = 0;
      left_opening_timer();
      right_opening_timer();
			TIM4->CR1 &= ~0x00000001;
			TIM4->SR &= ~0x00000001;
			TIM1->CR1 &= ~0x00000001;
			TIM1->SR &= ~0x00000001;
    }
    EXTI->PR = 0x00001000;
  }

  return;
}

void TIM2_IRQHandler(void) {
  if(left_opening) {
    left_door++;
    if(left_door >= 4) {
      left_door = 4;
      if(emerg_left_opening_flag) {
        emerg_left_opening_flag = 0;
        emerg_opened_flag = 1;
				left_opening = 0;
				TIM2->CR1 &= ~0x00000001;
      }
			else {
				left_opening = 0;
				TIM2->CR1 &= ~0x00000001;
				TIM4->CNT = 0x00000000;
				TIM4->CR1 |= 0x00000001;
			}
    }
  }
  else if(left_closing) {
    left_door--;
    if(left_door <= 0) {
      left_door = 0;
      if(emerg_closing_flag) {
        emerg_closing_flag = 0;
      }
      left_closing = 0;
      TIM2->CR1 &= ~0x00000001;
    }
  }
	TIM2->SR &= ~0x00000001;

  return;
}

void TIM3_IRQHandler(void) {
  if(right_opening) {
    right_door++;
    if(right_door >= 4) {
      right_door = 4;
      if(emerg_right_opening_flag) {
        emerg_right_opening_flag = 0;
        emerg_opened_flag = 1;
				right_opening = 0;
				TIM3->CR1 &= ~0x00000001;
      }
			else {
				right_opening = 0;
				TIM3->CR1 &= ~0x00000001;
				TIM1->CNT = 0x00000000;
				TIM1->CR1 |= 0x00000001;
			}
    }
  }
  else if(right_closing) {
    right_door--;
    if(right_door <= 0) {
      right_door = 0;
      if(emerg_closing_flag) {
        emerg_closing_flag = 0;
      }
      right_closing = 0;
      TIM3->CR1 &= ~0x00000001;
    }
  }
	TIM3->SR &= ~0x00000001;

  return;
}

void TIM4_IRQHandler(void) { //2초 for left
  left_closing_timer();

  TIM4->CR1 &= ~0x00000001;
  TIM4->SR &= ~0x00000001;

  return;
}

void TIM1_UP_IRQHandler(void) { //2초 for right
  right_closing_timer();

  TIM1->CR1 &= ~0x00000001;
  TIM1->SR &= ~0x00000001;

  return;
}
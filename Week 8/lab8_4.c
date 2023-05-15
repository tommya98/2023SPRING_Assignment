#include <stm32f10x.h>

#define GPIOAA 1
#define GPIOBB 2
#define GPIOCC 3
#define GPIODD 4
#define GPIOEE 5
#define PIN0  0x0001
#define PIN1  0x0002
#define PIN2  0x0004
#define PIN3  0x0008
#define PIN4  0x0010
#define PIN5  0x0020
#define PIN6  0x0040
#define PIN7  0x0080
#define PIN8  0x0100
#define PIN9  0x0200
#define PIN10 0x0400
#define PIN11 0x0800
#define PIN12 0x1000
#define PIN13 0x2000
#define PIN14 0x4000
#define PIN15 0x8000

void GPIO_PUSH_PULL_OUTPUT(u32 abcde, u32 start, u32 end) {
	u32 n;
  for(n = start; n <= end; n++) {
    switch(abcde) {
      case GPIOAA : 
        if(n == start) {
          RCC->APB2ENR |= ((u32)1 << 2);
          if(n < 8) {
            GPIOA->CRL = ((u32)3 << (4 * n));
          }
          else {
            GPIOA->CRH = ((u32)3 << (4 * (n - 8)));
          }
        }
        if(n < 8) {
          GPIOA->CRL |= ((u32)3 << (4 * n));
        }
        else {
          GPIOA->CRH |= ((u32)3 << (4 * (n - 8)));
        }
        break;

      case GPIOBB : 
        if(n == start) {
          RCC->APB2ENR |= ((u32)1 << 3);
          if(n < 8) {
            GPIOB->CRL = ((u32)3 << (4 * n));
          }
          else {
            GPIOB->CRH = ((u32)3 << (4 * (n - 8)));
          }
        }
        if(n < 8) {
          GPIOB->CRL |= ((u32)3 << (4 * n));
        }
        else {
          GPIOB->CRH |= ((u32)3 << (4 * (n - 8)));
        }
        break;

      case GPIOCC : 
        if(n == start) {
          RCC->APB2ENR |= ((u32)1 << 4);
          if(n < 8) {
            GPIOC->CRL = ((u32)3 << (4 * n));
          }
          else {
            GPIOC->CRH = ((u32)3 << (4 * (n - 8)));
          }
        }
        if(n < 8) {
          GPIOC->CRL |= ((u32)3 << (4 * n));
        }
        else {
          GPIOC->CRH |= ((u32)3 << (4 * (n - 8)));
        }
        break;

      case GPIODD : 
        if(n == start) {
          RCC->APB2ENR |= ((u32)1 << 5);
          if(n < 8) {
            GPIOD->CRL = ((u32)3 << (4 * n));
          }
          else {
            GPIOD->CRH = ((u32)3 << (4 * (n - 8)));
          }
        }
        if(n < 8) {
          GPIOD->CRL |= ((u32)3 << (4 * n));
        }
        else {
          GPIOD->CRH |= ((u32)3 << (4 * (n - 8)));
        }
        break;

      case GPIOEE : 
        if(n == start) {
          RCC->APB2ENR |= ((u32)1 << 6);
          if(n < 8) {
            GPIOE->CRL = ((u32)3 << (4 * n));
          }
          else {
            GPIOE->CRH = ((u32)3 << (4 * (n - 8)));
          }
        }
        if(n < 8) {
          GPIOE->CRL |= ((u32)3 << (4 * n));
        }
        else {
          GPIOE->CRH |= ((u32)3 << (4 * (n - 8)));
        }
        break;
    }
  }
}
void GPIO_PULL_UP_INPUT(u32 abcde, u32 start, u32 end) {
	u32 n;
  for(n = start; n <= end; n++) {
    switch(abcde) {
      case GPIOAA : 
        if(n == start) {
          RCC->APB2ENR |= ((u32)1 << 2);
          if(n < 8) {
            GPIOA->CRL = ((u32)8 << (4 * n));
          }
          else {
            GPIOA->CRH = ((u32)8 << (4 * (n - 8)));
          }
        }
        if(n < 8) {
          GPIOA->CRL |= ((u32)8 << (4 * n));
        }
        else {
          GPIOA->CRH |= ((u32)8 << (4 * (n - 8)));
        }
        GPIOA->ODR |= ((u32)1 << n);
        break;

      case GPIOBB : 
        if(n == start) {
          RCC->APB2ENR |= ((u32)1 << 3);
          if(n < 8) {
            GPIOB->CRL = ((u32)8 << (4 * n));
          }
          else {
            GPIOB->CRH = ((u32)8 << (4 * (n - 8)));
          }
        }
        if(n < 8) {
          GPIOB->CRL |= ((u32)8 << (4 * n));
        }
        else {
          GPIOB->CRH |= ((u32)8 << (4 * (n - 8)));
        }
        GPIOB->ODR |= ((u32)1 << n);
        break;

      case GPIOCC : 
        if(n == start) {
          RCC->APB2ENR |= ((u32)1 << 4);
          if(n < 8) {
            GPIOC->CRL = ((u32)8 << (4 * n));
          }
          else {
            GPIOC->CRH = ((u32)8 << (4 * (n - 8)));
          }
        }
        if(n < 8) {
          GPIOC->CRL |= ((u32)8 << (4 * n));
        }
        else {
          GPIOC->CRH |= ((u32)8 << (4 * (n - 8)));
        }
        GPIOC->ODR |= ((u32)1 << n);
        break;

      case GPIODD : 
        if(n == start) {
          RCC->APB2ENR |= ((u32)1 << 5);
          if(n < 8) {
            GPIOD->CRL = ((u32)8 << (4 * n));
          }
          else {
            GPIOD->CRH = ((u32)8 << (4 * (n - 8)));
          }
        }
        if(n < 8) {
          GPIOD->CRL |= ((u32)8 << (4 * n));
        }
        else {
          GPIOD->CRH |= ((u32)8 << (4 * (n - 8)));
        }
        GPIOD->ODR |= ((u32)1 << n);
        break;

      case GPIOEE : 
        if(n == start) {
          RCC->APB2ENR |= ((u32)1 << 6);
          if(n < 8) {
            GPIOE->CRL = ((u32)8 << (4 * n));
          }
          else {
            GPIOE->CRH = ((u32)8 << (4 * (n - 8)));
          }
        }
        if(n < 8) {
          GPIOE->CRL |= ((u32)8 << (4 * n));
        }
        else {
          GPIOE->CRH |= ((u32)8 << (4 * (n - 8)));
        }
        GPIOE->ODR |= ((u32)1 << n);
        break;
    }
  }
}
void GPIO_SET(u32 abcde, u32 n) {
  switch(abcde) {
    case GPIOAA : 
      GPIOA->BSRR = n;
      break;

    case GPIOBB : 
      GPIOB->BSRR = n;
      break;

    case GPIOCC : 
      GPIOC->BSRR = n;
      break;

    case GPIODD : 
      GPIOD->BSRR = n;
      break;

    case GPIOEE : 
      GPIOE->BSRR = n;
      break;
  }
}
void GPIO_RESET(u32 abcde, u32 n) {
  switch(abcde) {
    case GPIOAA : 
      GPIOA->BRR |= n;
      break;

    case GPIOBB : 
      GPIOB->BRR |= n;
      break;

    case GPIOCC : 
      GPIOC->BRR |= n;
      break;

    case GPIODD : 
      GPIOD->BRR |= n;
      break;

    case GPIOEE : 
      GPIOE->BRR |= n;
      break;
  }
}
void GPIO_WRITE_ODR(u32 abcde, u32 n) {
  switch(abcde) {
    case GPIOAA : 
      GPIOA->ODR = n;
      break;

    case GPIOBB : 
      GPIOB->ODR = n;
      break;

    case GPIOCC : 
      GPIOC->ODR = n;
      break;

    case GPIODD : 
      GPIOD->ODR = n;
      break;

    case GPIOEE : 
      GPIOE->ODR = n;
      break;
  }  
}
u32 GPIO_READ_IDR(u32 abcde) {
  switch(abcde) {
    case GPIOAA : 
      return GPIOA->IDR;
      break;

    case GPIOBB : 
      return GPIOB->IDR;
      break;

    case GPIOCC : 
      return GPIOC->IDR;
      break;

    case GPIODD : 
      return GPIOD->IDR;
      break;

    case GPIOEE : 
      return GPIOE->IDR;
      break;
  }
}

u32 i, key_index, key_row, key_col, col_scan;

int main(void) {

  //RCC->APB2ENR = 0x0000001D;

  GPIO_PUSH_PULL_OUTPUT(GPIOCC, 8, 11);
  GPIO_PULL_UP_INPUT(GPIOAA, 8, 11);
  //GPIOC->CRH = 0x00003333;
  //GPIOA->CRH = 0x00008888;
  //GPIOA->ODR = 0x0F00;

  GPIO_PUSH_PULL_OUTPUT(GPIOCC, 0, 7);
  GPIO_PUSH_PULL_OUTPUT(GPIOBB, 8, 15);
  //GPIOC->CRL = 0x33333333;
  //GPIOB->CRH = 0x33333333;

  GPIO_SET(GPIOCC, PIN1|PIN2|PIN3|PIN4|PIN5|PIN6|PIN7);
  //GPIOC->BSRR = 0x0FE;

  key_index = 0;
  key_row = 0x01;

  while(1) {
    GPIO_SET(GPIOCC, (~(key_row << 8) & 0x0F00) | (key_row << 24));
    //GPIOC->BSRR = (~(key_row << 8) & 0x0F00) | (key_row << 24);
    for(i = 0; i < 1000; i++) { ; }
    key_col = GPIO_READ_IDR(GPIOAA);
    //key_col = GPIOA->IDR;
    key_col = (key_col >> 8) & 0x0F;
    col_scan = 0x01;
    for (i = 0; i < 4; i++) {
      if((key_col & col_scan) == 0)
        GPIO_WRITE_ODR(GPIOBB, (key_index << 8));
        //GPIOB->ODR = key_index << 8;
      col_scan = col_scan << 1;
      key_index = key_index + 1;
    }
    key_row = key_row << 1;
    if (key_row == 0x10) {
      key_row = 0x01;
      key_index = 0;
    }
  }
}
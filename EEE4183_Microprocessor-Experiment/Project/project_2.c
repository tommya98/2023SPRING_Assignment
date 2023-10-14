// 보드 내부 LED   PA5
// USART TX       PA9(x)
// USART RX       PA10
// USART CTS      PA11(x)
// USART RTS      PA12
// DOTMATRIX ROW  PC8-PC12, PC13
// DOTMATRIX COL  PB8-PB15, PC0-PC7 거꾸로

#include <stm32f10x.h>

void init(void);
void get_data(void);
void make_dotmatrix_buffer(void);
void USART1_IRQHandler(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void TIM1_UP_IRQHandler(void);

static unsigned char font[14][8] = {
  {0x07, 0x05, 0x05, 0x05, 0x07, 0x00, 0x00, 0x00}, //0
  {0x06, 0x02, 0x02, 0x02, 0x07, 0x00, 0x00, 0x00}, //1
  {0x07, 0x01, 0x07, 0x04, 0x07, 0x00, 0x00, 0x00}, //2
  {0x07, 0x01, 0x07, 0x01, 0x07, 0x00, 0x00, 0x00}, //3
  {0x05, 0x05, 0x07, 0x01, 0x01, 0x00, 0x00, 0x00}, //4
  {0x07, 0x04, 0x07, 0x01, 0x07, 0x00, 0x00, 0x00}, //5
  {0x07, 0x04, 0x07, 0x05, 0x07, 0x00, 0x00, 0x00}, //6
  {0x07, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00}, //7
  {0x07, 0x05, 0x07, 0x05, 0x07, 0x00, 0x00, 0x00}, //8
  {0x07, 0x05, 0x07, 0x01, 0x07, 0x00, 0x00, 0x00}, //9
  {0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00}, //l
  {0x00, 0x00, 0x05, 0x02, 0x05, 0x00, 0x00, 0x00}, //x
  {0x03, 0x04, 0x04, 0x04, 0x03, 0x00, 0x00, 0x00}, //C
  {0x07, 0x04, 0x07, 0x04, 0x04, 0x00, 0x00, 0x00}  //F
};
static unsigned char rxdata[10] = {0xFF, 1, 12, 00, 00, 7, 65, 4, 23, 4};
static unsigned int dotmatrix_buffer[8] = {0, };
static unsigned char print_buffer[2][8];
static unsigned char current_mode = 1, hh = 12, mm = 0, ss = 0, illum1 = 0, illum2 = 0, illum3 = 0, temp1 = 0, temp2 = 0;
static unsigned char col_len = 30, col_index = 14, row = 0x01, row2 = 0, led_flag = 1, scroll_index = 0;
static unsigned int illum = 0;
static unsigned char rx_index = 0, usart_off = 1, usart_off_led = 0;

int main(void) {
  init();

  while(1) {
    get_data();
    make_dotmatrix_buffer();
  }

}

void init(void) {
  RCC->APB2ENR |= 0x0000001D;
  GPIOA->CRL = 0x44344444;
  GPIOA->CRH = 0x444B44B4;
  GPIOB->CRH = 0x33333333;
  GPIOC->CRL = 0x33333333;
  GPIOC->CRH = 0x44333333;
  AFIO->MAPR = 0x00004000;

  RCC->APB2ENR |= 0x00004000;
	USART1->CR1 = 0x00000000;
  USART1->CR2 = 0x00000000;
	USART1->CR3 = 0x00000000;
  USART1->CR3 = 0x00000100;
  USART1->BRR = 0x00000EA6;
	NVIC->ISER[1] |= 0x00000020;
	USART1->CR1 |= 0x00000004;
  USART1->CR1 |= 0x00002000;
  USART1->CR1 |= 0x00000020;

  RCC->APB1ENR |= 0x00000001;
  TIM2->CR1 = 0x00000080; //dot matrix scan
  TIM2->CR2 = 0x00000000;
  TIM2->PSC = 0x00000009;
  TIM2->ARR = 0x00001C1F;
  TIM2->CCMR1 = 0x00000068;
  TIM2->CCER = 0x00000001;
  TIM2->CCR1 = 0x00001A26;
  TIM2->DIER = 0x00000003;
  NVIC->ISER[0] |= 0x10000000;
  TIM2->CR1 |= 0x00000001;

  RCC->APB1ENR |= 0x00000002;
  TIM3->CR1 = 0x00000004; //점멸 기능
  TIM3->CR2 = 0x00000000;
  TIM3->PSC = 0x00001C1F;
  TIM3->ARR = 0x000009C3; //0.25초
  TIM3->DIER = 0x00000001;
  NVIC->ISER[0] |= 0x20000000;
	TIM3->CR1 |= 0x00000001;

  RCC->APB1ENR |= 0x00000004;
  TIM4->CR1 = 0x00000004; //스크롤링
  TIM4->CR2 = 0x00000000;
  TIM4->PSC = 0x00001C1F;
  TIM4->ARR = 0x00000BB7;
  TIM4->DIER = 0x00000001;
  NVIC->ISER[0] |= 0x40000000;
	TIM4->CR1 |= 0x00000001;

  RCC->APB2ENR |= 0x00000800;
  TIM1->CR1 = 0x00000004;
  TIM1->CR2 = 0x00000000;
  TIM1->PSC = 0x00001C1F;
  TIM1->ARR = 0x00001387; //0.5초
  TIM1->DIER = 0x00000001;
  NVIC->ISER[0] |= 0x02000000;
  TIM1->CR1 |= 0x00000001;

}

void get_data(void) {
  if(rxdata[0] == 0xFF) {
    if(current_mode != rxdata[1]) {
      current_mode = rxdata[1];
      scroll_index = 0;
    }
    hh = rxdata[2];
    mm = rxdata[3];
    ss = rxdata[4];
    illum1 = rxdata[5];
    illum2 = rxdata[6];
    illum3 = rxdata[7];
    temp1 = rxdata[8];
    temp2 = rxdata[9];
		illum = illum1 * 100 + illum2;
		illum = 700 + illum * 10;
		if(illum > 7000) {
			illum = 7000;
		}
    TIM2->CCR1 = (unsigned short)illum;
  }

  return;
}

void make_dotmatrix_buffer(void) {
  int i;
  NVIC->ICER[0] = 0x72000020;
  switch(current_mode) {
    case 1: //시계
    case 2:
    case 3:
      for(i = 0; i < 5; i++) {
        dotmatrix_buffer[i] = (unsigned int)(font[hh / 10][i] << 24);
        dotmatrix_buffer[i] |= (unsigned int)(font[hh % 10][i] << 20);
        if(i == 1 || i == 3) {
          dotmatrix_buffer[i] |= 0x00040000;
        }
        dotmatrix_buffer[i] |= (unsigned int)(font[mm / 10][i] << 14);
        dotmatrix_buffer[i] |= (unsigned int)(font[mm % 10][i] << 10);
        if(i == 1 || i == 3) {
          dotmatrix_buffer[i] |= 0x00000100;
        }
        dotmatrix_buffer[i] |= (unsigned int)(font[ss / 10][i] << 4);
        dotmatrix_buffer[i] |= (unsigned int)(font[ss % 10][i] << 0);
      }
      if(hh >= 10) {
        col_len = 28;
      }
      else {
        col_len = 24;
      }
      break;
    case 4: //조도
      for(i = 0; i < 5; i++) {
        dotmatrix_buffer[i] = (unsigned int)(font[illum1 / 10][i] << 26);
        dotmatrix_buffer[i] = (unsigned int)(font[illum1 % 10][i] << 22);
        dotmatrix_buffer[i] |= (unsigned int)(font[illum2 / 10][i] << 18);
        dotmatrix_buffer[i] |= (unsigned int)(font[illum2 % 10][i] << 14);
        if(i == 4) {
          dotmatrix_buffer[i] |= 0x00001000;
        }
        dotmatrix_buffer[i] |= (unsigned int)(font[illum3][i] << 8);
        dotmatrix_buffer[i] |= (unsigned int)(font[10][i] << 4);
        dotmatrix_buffer[i] |= (unsigned int)(font[11][i] << 0);
      }
      if(illum1 >= 10) {
        col_len = 30;
      }
      if(illum1 > 0) {
        col_len = 26;
      }
      else if(illum2 >= 10) {
        col_len = 22;
      }
      else {
        col_len = 18;
      }
      
      break;
    case 5: //섭씨
      for(i = 0; i < 5; i++) {
        dotmatrix_buffer[i] = (unsigned int)(font[temp1 / 100][i] << 22);
        dotmatrix_buffer[i] |= (unsigned int)(font[(temp1 / 10) % 10][i] << 18);
        dotmatrix_buffer[i] |= (unsigned int)(font[temp1 % 10][i] << 14);
        if(i == 4) {
          dotmatrix_buffer[i] |= 0x00001000;
        }
        dotmatrix_buffer[i] |= (unsigned int)(font[temp2][i] << 8);
        if(i == 0) {
          dotmatrix_buffer[i] |= 0x00000020;
        }
				else if(i == 1) {
					dotmatrix_buffer[i] |= 0x00000050;
				}
        else if(i == 2) {
          dotmatrix_buffer[i] |= 0x00000020;
        }
        dotmatrix_buffer[i] |= (unsigned int)(font[12][i] << 0);
      }
      if(temp1 >= 100) {
        col_len = 26;
      }
      else if(temp1 >= 10) {
        col_len = 22;
      }
      else {
        col_len = 18;
      }
      break;
    case 6: //화씨
      for(i = 0; i < 5; i++) {
        dotmatrix_buffer[i] = (unsigned int)(font[temp1 / 100][i] << 22);
        dotmatrix_buffer[i] |= (unsigned int)(font[(temp1 / 10) % 10][i] << 18);
        dotmatrix_buffer[i] |= (unsigned int)(font[temp1 % 10][i] << 14);
        if(i == 4) {
          dotmatrix_buffer[i] |= 0x00001000;
        }
        dotmatrix_buffer[i] |= (unsigned int)(font[temp2][i] << 8);
        if(i == 0) {
          dotmatrix_buffer[i] |= 0x00000020;
        }
				else if(i == 1) {
					dotmatrix_buffer[i] |= 0x00000050;
				}
        else if(i == 2) {
          dotmatrix_buffer[i] |= 0x00000020;
        }
        dotmatrix_buffer[i] |= (unsigned int)(font[13][i] << 0);
      }
      if(temp1 >= 100) {
        col_len = 26;
      }
      else if(temp1 >= 10) {
        col_len = 22;
      }
      else {
        col_len = 18;
      }
      break;
  }
  NVIC->ISER[0] = 0x72000020;

  return;
}

void USART1_IRQHandler(void) {
  unsigned char temp;
  
  if(USART1->SR & 0x20) {
    temp = (unsigned char)USART1->DR;
		GPIOA->BSRR = 0x10000000;
    if(rx_index == 0 && temp == 255) {
			rxdata[rx_index++] = temp;
    }
    else if(rx_index > 0) {
      rxdata[rx_index++] = temp;
      if(rx_index == 10) {
        rx_index = 0;      
      }
    }

    if(usart_off == 0) {
      TIM1->CNT = 0;
      GPIOA->BSRR = 0x20;
    }
    else {
      usart_off = 0;
      TIM1->CNT = 0;
      GPIOA->BSRR = 0x20;
      TIM2->CR1 |= 0x00000001;
    }
  }

  return;
}

void TIM2_IRQHandler(void) {
  if((TIM2->SR & 0x01) != 0) {
    GPIOB->ODR = (unsigned int)print_buffer[1][row2] << 8;
    GPIOC->ODR = (unsigned int)print_buffer[0][row2];

    if(row2 == 5 && led_flag == 0) {
      GPIOC->ODR |= (0xFF << 8);
    }
    else {
      GPIOC->ODR |= (unsigned int)(~row << 8);
    }

    row = (unsigned char)(row << 1);
    if(row == 0x40) {
      row = 0x01;
    }
    row2 += 1;
    if(row2 == 6) {
      row2 = 0;
    }

    TIM2->SR &= ~0x00000001;
  }
  else if((TIM2->SR & 0x02) != 0) {
    GPIOC->ODR |= (0xFF << 8);
    TIM2->SR &= ~0x00000002;
  }
  
  return;
}

void TIM3_IRQHandler(void) {
	if((TIM3->SR & 0x01) != 0) {
    if((current_mode == 1 || (hh == 12 && mm == 0 && ss == 0)) == 0) {
      led_flag = 1;
      TIM3->SR &= ~0x00000001;
      return;
    }
    if(led_flag == 1) {
      led_flag = 0;
    }
    else {
      led_flag = 1;
    }
    TIM3->SR &= ~0x00000001;
  }

  return;
}

void TIM4_IRQHandler(void) {
  int i;
  unsigned int temp_buffer;
  
  if((TIM4->SR & 0x01) != 0) {
    col_index = col_len - 16;

    for(i = 0; i < 5; i++) {
      temp_buffer = (dotmatrix_buffer[i] << scroll_index) >> col_index;
      print_buffer[0][i] = (temp_buffer >> 8) & 0xFF;
      print_buffer[1][i] = temp_buffer & 0xFF;
    }
    print_buffer[1][5] = (unsigned char)(1 << (6 - current_mode));
    scroll_index++;
    if(scroll_index > col_len) {
      scroll_index = 0;
    }

    TIM4->SR &= ~0x00000001;
  }

  return;
}

void TIM1_UP_IRQHandler(void) {
  if((TIM1->SR & 0x01) != 0) {
    if(usart_off == 0) {
      usart_off = 1;
      usart_off_led = 0;
    }
    else {
      if(usart_off_led == 0) {
        GPIOA->BSRR = 0x200000;
        usart_off_led = 1;
      }
      else {
        GPIOA->BSRR = 0x20;
        usart_off_led = 0;
      }

      GPIOC->ODR |= (0xFF << 8);
      TIM2->CR1 &= ~0x00000001;
    }
    TIM1->SR &= ~0x00000001;
  }
}

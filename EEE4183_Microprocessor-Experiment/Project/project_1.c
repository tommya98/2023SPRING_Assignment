// 조도센서 입력  PA6
// 온도센서 입력  PA4
// 보드 내부 LED PA5
// USART TX     PA9
// USART RX     PA10(x)
// USART CTS    PA11
// USART RTS    PA12(x)
// KEYMATRIX    PC0-PC7

#include <stm32f10x.h>

#define StartAddr ((u32)0x08008000)

void init(void);
void calc_adc(void);
void manage_key(void);
void make_txdata(void);
void flash_read(void);
void flash_write(void);
void ADC1_2_IRQHandler(void);
void USART1_IRQHandler(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void TIM1_UP_IRQHandler(void);

static unsigned char key_matrix[9][9] = {
  {255, 255, 255, 255, 255, 255, 255, 255, 255},
  {255, 0  , 1  , 255, 2  , 255, 255, 255, 3  },
  {255, 4  , 5  , 255, 6  , 255, 255, 255, 7  },
  {255, 255, 255, 255, 255, 255, 255, 255, 255},
  {255, 8  , 9  , 255, 10 , 255, 255, 255, 11 },
  {255, 255, 255, 255, 255, 255, 255, 255, 255},
  {255, 255, 255, 255, 255, 255, 255, 255, 255},
  {255, 255, 255, 255, 255, 255, 255, 255, 255},
  {255, 12 , 13 , 255, 14 , 255, 255, 255, 15 }
};
static unsigned char txdata[10];
//"FF 모드 시시 분분 초초 조조 조조 도 온 도" //FF 01 13 12 11 08 20 3 25 3 => 01모드로 13시 12분 11초 조도:820.3 온도: 25.3도
// 모드 01 시계 세팅 모드 - 24시간 모드
// 모드 02 24시간 모드
// 모드 03 12시간 모드
// 모드 04 조도 모드
// 모드 05 온도 섭씨 모드
// 모드 06 온도 화씨 모드
static unsigned int ADCConverted_Temp, ADCConverted_Illum;
static unsigned char ADC_flag = 0, key_row = 1, key_col, before_pressed_key = 255, current_pressed_key = 255, key_pressing = 0, index = 1;
static unsigned char default_mode = 1, current_mode = 1, hh = 12, mm = 0, ss = 0, temphh = 12, tempmm = 0, tempss = 0, illum1 = 0, illum2 = 0, illum3 = 0, temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0;
static unsigned char tx_index = 0, usart_off = 1, usart_off_led = 0, time_set = 0;

int main(void) {
  init();
  flash_read();

  while(1) {
    manage_key();
    make_txdata();
  }
}

void init(void) {
  RCC->APB2ENR |= 0x00000015;
  GPIOA->CRL = 0x40304444;
  GPIOA->CRH = 0x444B84B4;
  GPIOA->ODR |= 0x00000800;
  GPIOC->CRL = 0x33338888;
  GPIOC->ODR = 0x0000000F;
  AFIO->MAPR = 0x00004000;

  RCC->APB2ENR |= 0x00000200;
  ADC1->CR1 = 0x00000020;
  ADC1->CR2 = 0x001E0002;
  ADC1->SMPR2 = 0x00007E00;
  ADC1->SQR1 = 0x00000000;
  ADC1->SQR2 = 0x00000000;
  ADC1->SQR3 = 0x00000004;
  NVIC->ISER[0] |= 0x00040000;
  ADC1->CR2 |= 0x00000001;
  ADC1->CR2 |= 0x00400000;
  
  RCC->APB2ENR |= 0x00004000;
  USART1->CR1 = 0x00000000;
  USART1->CR2 = 0x00000000;
	USART1->CR3 = 0x00000000;
  USART1->CR3 = 0x00000600;
  USART1->BRR = 0x00000EA6;
	NVIC->ISER[1] |= 0x00000020;
  USART1->CR1 |= 0x00000008;
  USART1->CR1 |= 0x00002000;
  USART1->CR1 |= 0x00000080;

  RCC->APB1ENR |= 0x00000001;
  TIM2->CR1 = 0x00000004; //key pad scan
  TIM2->CR2 = 0x00000000;
  TIM2->PSC = 0x00001C1F;
  TIM2->ARR = 0x000000C7;
  TIM2->DIER = 0x00000001;
  NVIC->ISER[0] |= 0x10000000;
  TIM2->CR1 |= 0x00000001;

  RCC->APB1ENR |= 0x00000002;
  TIM3->CR1 = 0x00000004; //1초 증가
  TIM3->CR2 = 0x00000000;
  TIM3->PSC = 0x00001C1F;
  TIM3->ARR = 0x0000270F;
  TIM3->DIER = 0x00000001;
  NVIC->ISER[0] |= 0x20000000;
	TIM3->CR1 |= 0x00000001;

  RCC->APB1ENR |= 0x00000004;
  TIM4->CR1 = 0x00000004; //모드 1초 타이머
  TIM4->CR2 = 0x00000000;
  TIM4->PSC = 0x00001C1F;
  //TIM4->ARR = 0x0000270F; //1초
  TIM4->ARR = 0x0000C34F; //5초
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

  return;
}

void calc_adc(void) {
	int temp, illum;
	
  temp = (ADCConverted_Temp * 3600 / 4095 - 500);
  temp1 = (unsigned char)(temp / 10);
  temp2 = (unsigned char)(temp % 10);
  temp3 = (unsigned char)(temp * 18 / 100 + 32);
  temp4 = (unsigned char)((temp * 18 / 10) % 10);
	
	if(ADCConverted_Illum <= 3815) {
    illum = ((ADCConverted_Illum * 13010 / 4095) - 2020);
		if(illum < 0) {
			illum = 50;
		}
  }
  else {
    illum = (ADCConverted_Illum * 44659 / 100 - 1693640);
  }
  illum1 = (unsigned char)(illum / 1000);
  illum2 = (unsigned char)((illum - illum1 * 1000) / 10);
  illum3 = (unsigned char)(illum % 10);

	return;
}

void manage_key(void) {
  int flag = 0;
  if(current_mode == default_mode) {
    flag = 1;
  }
  if(before_pressed_key != current_pressed_key){
    if(before_pressed_key == 255) {
      TIM4->CNT = 0x00000000;
      switch(current_pressed_key) {
        case 0:
          if(current_mode == 2) {
            current_mode = 3;
          }
          else {
            current_mode = 2;
          }
          break;
        case 1:
          current_mode = 4;
          break;
        case 2:
          if(current_mode == 5) {
            current_mode = 6;
          }
          else {
            current_mode = 5;
          }
          break;
        case 3:
          if(current_mode == 1) {
            time_set = 1;
            default_mode = 2;
            current_mode = 2;
          }
          default_mode = current_mode;
          TIM4->CR1 &= ~0x00000001;
          break;
        case 4:
          break;
        case 5:
          index += 1;
          if(index == 4) {
            index = 1;
          }
          break;
        case 6:
          break;
        case 7:
          break;
        case 8:
          if(current_mode == 1) {
            TIM4->CR1 &= ~0x00000001;
            TIM4->CNT = 0;
            TIM4->CR1 |= 0x00000001;
          }
          if(index == 1) {
            hh += 1;
            if(hh == 24) {
              hh = 0;
            }
          }
          else if(index == 2) {
            mm += 1;
            if(mm == 60) {
              mm = 0;
            }
          }
          else if(index == 3) {
            ss += 1;
            if(ss == 60) {
              ss = 0;
            }
          }
          break;
        case 9:
          break;
        case 10:
          if(current_mode == 1) {
            TIM4->CR1 &= ~0x00000001;
            TIM4->CNT = 0;
            TIM4->CR1 |= 0x00000001;
          }
          if(index == 1) {
            if(hh == 0) {
              hh = 23;
            }
            else {
              hh -= 1;
            }
          }
          else if(index == 2) {
            mm -= 1;
            if(mm == 0) {
              mm = 59;
            }
          }
          else if(index == 3) {
            ss -= 1;
            if(ss == 0) {
              ss = 59;
            }
          }
          break;
        case 11:
          break;
        case 12:
          break;
        case 13:
          index -= 1;
          if(index == 0) {
            index = 3;
          }
          break;
        case 14:
          break;
        case 15:
          current_mode = 1;
          temphh = hh;
          tempmm = mm;
          tempss = ss;
          index = 1;
          TIM4->CR1 |= 0x00000001;
          break;
        case 255:
          break;
      }
    }
    before_pressed_key = current_pressed_key;

    flash_write();
  }
  if((current_mode != default_mode) && (flag == 1)) {
    TIM4->CR1 |= 0x00000001;
  }

	return;
}

void make_txdata(void) {
	txdata[0] = 0xFF;
	txdata[1] = current_mode; //mode
  if(hh > 12 && current_mode == 3) {
    txdata[2] = hh - 12;
  }
  else {
    txdata[2] = hh;
  }
	txdata[3] = mm; //분
	txdata[4] = ss; //초
	txdata[5] = illum1; //조도
	txdata[6] = illum2; //조도
	txdata[7] = illum3; //조도 소수점
  if(current_mode == 6) { //화씨
    txdata[8] = temp3; //온도
    txdata[9] = temp4; //온도 소수점
  }
  else { //섭씨
    txdata[8] = temp1; //온도
    txdata[9] = temp2; //온도 소수점
  }
	
	return;
}

void flash_read(void) {
  memcpy(&current_mode, (unsigned char*) StartAddr, 1);
  default_mode = current_mode;

  return;
}

void flash_write(void) {
  while((FLASH->SR & 0x01) != 0) {}

  if((FLASH->CR & 0x80) != 0) {
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;
  }

  FLASH->CR |= 0x00000002;
  FLASH->AR = StartAddr;
  FLASH->CR |= 0x00000040;

  while((FLASH->SR & 0x01) != 0) {}
  if((FLASH->SR & 0x20) != 0) {
    FLASH->SR |= 0x20;
  }
  FLASH->CR &= (unsigned int)~0x00000002;

  while((FLASH->SR & 0x01) != 0) {}
  if((FLASH->CR & 0x80) != 0) {
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;
  }

  FLASH->CR |= 0x00000001;
  while ((FLASH->SR & 0x01) != 0) {}
  *(__IO uint16_t*)(StartAddr) = (uint16_t)current_mode;
  while ((FLASH->SR & 0x01) != 0) {}

  if ((FLASH->SR & 0x20) != 0) {
    FLASH->SR |= 0x20;
  }
  FLASH->CR &= (unsigned int)~0x00000001;

  return;
}

void ADC1_2_IRQHandler(void) {
  if(ADC1->SR & 0x02) {
    if(ADC_flag == 0) {
			ADC1->SQR3 = 0x00000006;
      ADC_flag = 1;
      ADCConverted_Temp = ADC1->DR;
    }
    else {
			ADC1->SQR3 = 0x00000004;
      ADC_flag = 0;
      ADCConverted_Illum = ADC1->DR;
    }
  }
  return;
}

void USART1_IRQHandler(void) {
  if(USART1->SR & 0x200) {
    if(usart_off == 0) {
      TIM1->CNT = 0;
      GPIOA->BSRR = 0x20;
    }
    else {
      usart_off = 0;
      TIM1->CNT = 0;
      GPIOA->BSRR = 0x20;
    }
    USART1->SR &= ~0x00000200;
  }
  else if(USART1->SR & 0x80) {
    USART1->DR = txdata[tx_index++];
    if(tx_index == 10) {
      tx_index = 0;
    }
  }

  return;
}

void TIM2_IRQHandler(void) {
  unsigned int i, temp_key_col;

  if((TIM2->SR & 0x01) != 0) {
    GPIOC->BSRR = (unsigned int)((~(key_row << 4) & 0xF0) | (key_row << 20));
    for(i = 0; i < 10000; i++) {}

    temp_key_col = GPIOC->IDR;
    temp_key_col = (temp_key_col) & 0x0F;
    if(key_pressing == 0 || temp_key_col == 0x0F) {
      key_col = (unsigned char)(temp_key_col ^ 0x0F);
    }
    current_pressed_key = key_matrix[key_row][key_col];

    if(key_col != 0x00) {
      key_pressing = 1;
    }
    else {
      key_pressing = 0;
    }
    
    if(key_pressing == 0) {
      key_row = (unsigned char)(key_row << 1);
      if(key_row == 0x10) {
        key_row = 0x01;
      }
    }

    TIM2->SR &= ~0x00000001;
  }

  return;
}

void TIM3_IRQHandler(void) {
  if((TIM3->SR & 0x01) != 0) {
    if(current_mode != 1 && time_set == 1) {
      ss += 1;
      if(ss == 60) {
        ss = 0;
        mm += 1;
      }
      if(mm == 60) {
        mm = 0;
        hh += 1;
      }
      if(hh == 24) {
        hh = 0;
      }
    }
    else if(current_mode == 1 && time_set == 1) {
      tempss += 1;
      if(tempss == 60) {
        tempss = 0;
        tempmm += 1;
      }
      if(tempmm == 60) {
        tempmm = 0;
        temphh += 1;
      }
      if(temphh == 24) {
        temphh = 0;
      }
    }
    calc_adc();

    TIM3->SR &= ~0x00000001;
  }

  return;
}

void TIM4_IRQHandler(void) {
  if((TIM4->SR & 0x01) != 0) {
    if(default_mode == 1 || (current_mode == 1 && default_mode == 2) || (current_mode == 1 && default_mode == 3)) {
      index = 1;
      hh = temphh;
      mm = tempmm;
      ss = tempss;
    }
    current_mode = default_mode;
		flash_write();

    TIM4->CR1 &= ~0x00000001;
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
    }
    TIM1->SR &= ~0x00000001;
  }
}

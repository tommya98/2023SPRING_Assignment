#include <stm32f10x.h>

u16 ADCConverted;

int main(void) {
  //RCC->APB2ENR |= 0x00000005;
  //GPIOA->CRL = 0x44404444;

  RCC->APB2ENR |= 0x00000200;
  ADC1->CR1 = 0x00000000;
  ADC1->CR2 = 0x009E0002; //TSVREFE bit
  ADC1->SMPR1 = 0x001C0000; //sample time 17.1us
  ADC1->SQR1 = 0x00000000;
  ADC1->SQR2 = 0x00000000;
  ADC1->SQR3 = 0x00000010; //ADC1_IN16

  ADC1->CR2 |= 0x00000001;
  ADC1->CR2 |= (1 << 22);

  while(1) {
    while(!(ADC1->SR & 0x02));
    ADCConverted = ADC1->DR;
  }
}
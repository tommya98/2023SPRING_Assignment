#include <stm32f10x.h>
#define ADC1_DR_BASE 0x4001244c

u16 ADCConverted;

int main(void) {
  RCC->APB2ENR |= 0x00000005;
  GPIOC->CRL = 0x44404444;

  RCC->AHBENR |= 0x00000001;
  DMA1_Channel1->CCR = 0x00003520;
  DMA1_Channel1->CNDTR = 1;
  DMA1_Channel1->CPAR = ADC1_DR_BASE;
  DMA1_Channel1->CMAR = (u32)&ADCConverted;

  RCC->APB2ENR |= 0x00000200;
  ADC1->CR1 = 0x00000000;
  ADC1->CR2 = 0x001E0102;
  ADC1->SMPR2 = 0x00007000;
  ADC1->SQR1 = 0x00000000;
  ADC1->SQR2 = 0x00000000;
  ADC1->SQR3 = 0x00000004;

  DMA1_Channel1->CCR |= 0x00000001;
  ADC1->CR2 |= 0x00000001;
  ADC1->CR2 |= 0x00400000;

	while(1) {;}
}
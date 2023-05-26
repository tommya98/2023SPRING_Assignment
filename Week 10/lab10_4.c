#include "stm32f10x.h" // Device header

char str1[] = "HAPPY";
char str2[] = "MicroProcessor";

void TIM1_UP_IRQHandler(void) {
	NVIC_DisableIRQ(TIM2_IRQn);
    for(int i=0; str1[i]!='\0'; i++) {
        while(!(USART1->SR & USART_SR_TXE)); // Wait until transmit data register is empty
        USART1->DR = str1[i]; // Send character
    }
    TIM1->SR &= ~TIM_SR_UIF; // Reset the update interrupt flag
    NVIC_EnableIRQ(TIM2_IRQn);
}
void TIM2_IRQHandler(void) {
    NVIC_DisableIRQ(TIM1_UP_IRQn);
    for(int i=0; str2[i]!='\0'; i++) {
        while(!(USART1->SR & USART_SR_TXE)); // Wait until transmit data register is empty
        USART1->DR = str2[i]; // Send character
    }
    TIM2->SR &= ~TIM_SR_UIF; // Reset the update interrupt flag
    NVIC_EnableIRQ(TIM1_UP_IRQn);
}
void USART1_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Enable USART1 clock
    USART1->BRR = 0x1D4C; // Set baud rate to 9600 [assuming 72MHz clock]
    USART1->CR1 |= USART_CR1_UE | USART_CR1_TE; // Enable USART, set TE (Transmitter enabled)
}
void TIM1_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; // Enable TIM1 clock
    TIM1->PSC = 7199; // Set prescaler [assuming 72MHz clock, 7200-1 for 1ms]
    TIM1->ARR = 10 - 1; // Set auto reload register for 10ms
    TIM1->DIER |= TIM_DIER_UIE; // Enable update interrupt
    TIM1->CR1 |= TIM_CR1_CEN; // Start timer
    NVIC_EnableIRQ(TIM1_UP_IRQn); // Enable TIM1 interrupt
}
void TIM2_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable TIM2 clock
    TIM2->PSC = 7199; // Set prescaler [assuming 72MHz clock, 7200-1 for 1ms]
    TIM2->ARR = 13 - 1; // Set auto reload register for 13ms
    TIM2->DIER |= TIM_DIER_UIE; // Enable update interrupt
    TIM2->CR1 |= TIM_CR1_CEN; // Start timer
    NVIC_EnableIRQ(TIM2_IRQn); // Enable TIM2 interrupt
}
int main() {
    RCC->APB2ENR = 0x00004004;
    GPIOA->CRH &= ~(0xFFu << 4);
    GPIOA->CRH |= (0x04B << 4);
	
    USART1_Init();
    TIM1_Init();
    TIM2_Init();
	
    while(1) {}
}
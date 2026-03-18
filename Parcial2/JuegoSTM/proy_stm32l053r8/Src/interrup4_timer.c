#include <stdint.h>
#include "stm32l053xx.h"

void delay_ms(uint16_t n);
void USART2_read (uint8_t ch);
uint USART2_Putstring(uint8_t* stringptr);
void USART2_PutstringE(uint8_t* stringptr);
void delay_ms(uint16_t n);
uint8_t atm_fsm = 0x00;
uint8_t recieved_Vic = 0x00;
uint8_t recieved_var = 0x00;

int main(void)
{
	//ENABLE HSI 16MHz
	//HSI on
	RCC->CR |= (1<<0);
	//HSI16 as SYSCLK
	RCC->CFGR |= (1<<0);

	//GPIOA CLK ENABLE
	RCC->IOPENR |= (1<<0);
	//GPIOC CLK ENABLE
	RCC->IOPENR |= (1<<2);

	//ONBOARD GPIOA PORT CONFIG
	//PA5 as Output
	GPIOA->MODER &= ~(1<<11);
	//ONBOARD CPIOC PORT CONFIG
	//PC4 AS OUTPUT
	GPIOC->MODER &= ~(1<<9);

	//ONBOARD FPIOC PORT CONFIG
	GPIOC->MODER &= ~(1<<26);
	GPIOC->MODER &= ~(1<<27);

	//ENABLE EXTERNAL INTERRUPTS
	//CLOCK FOR SYSCFG ENGINE
	RCC->APB2ENR |= (1<<0);
	//SELECT PORTC FOR EXTI13
	SYSCFG->EXTICR[3] |= (1<<5);
	//UNMASK EXT1 LINE 13, exti13
	EXTI->IMR |= (1<<13);
	//SELECT TRIGGER TYPE
	EXTI->FTSR |= (1<<13);

	//USART2
	//MODULE INITIALIZATION
	USART2_init();
	//USART RX INTERRUPT ENABLE
	USART2->CR1 |= (1<<5);
	USART_init();

}

/*
 * blinky.c
 *
 *      Author: hardware
 */

#include <stdint.h>
#include "stm32l053xx.h"

void delay_ms(uint16_t n);

int main(void)
{
	RCC->IOPENR |= 1<<0; //Enciendo CLK GPIOA
	RCC->IOPENR |= 1<<2; //Enciendo CLK GPIOC
	GPIOA->MODER &= ~(1<<11); //Config PA5 como output

	GPIOA->ODR |= 1<<5; //SET 1 en PA5
	GPIOA->ODR &= ~(1<<5); //SET 0 en PA5

	GPIOC->MODER &= ~(1<<26); //PC13 como input
	GPIOC->MODER &= ~(1<<27); //PC13 como input

	//*gpioc_idr



    /* Loop forever */
	while(1)
	{
		GPIOA->ODR &= ~(1<<5); // apago led
		delay_ms(1000);
		GPIOA->ODR |= 1<<5; //enciendo led
		delay_ms(1000);
		/*
		if(GPIOC->IDR & (1<<13))//mientras PC13 este high
		{
			GPIOA->ODR &= ~(1<<5); // apago led

		}
		else
		{
			GPIOA->ODR |= 1<<5; //enciendo led
		}
		*/

	}
}

void delay_ms(uint16_t n)
{
	uint16_t i;
	for(; n>0; n--)
		for(i=0; i<140;i++);
}

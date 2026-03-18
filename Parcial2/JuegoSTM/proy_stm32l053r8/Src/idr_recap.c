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

	GPIOC->MODER &= ~(1<<26); //PC13 como input
	GPIOC->MODER &= ~(1<<27); //PC13 como input

	GPIOC->MODER &= ~(1<<25); //PC12 como input
	GPIOC->MODER &= ~(1<<24); //PC12 como input

	//*gpioc_idr



    /* Loop forever */
	while(1)
	{
		/*
		GPIOA->ODR &= ~(1<<5); // apago led
		delay_ms(1000);
		GPIOA->ODR |= 1<<5; //enciendo led
		delay_ms(1000);
		*/

		if(GPIOC->IDR & (1<<13))//mientras PC13 este high
		{
			//set val down timer 15 s
			command_set = 0x01;
		}
		else if(GPIOC->IDR & (1<<12))
		{
			//set val down timer 60 s
			command_set = 0x02;
		}
		else if(GPIOC->IDR & (1<<11))
		{
			//set val down timer 120 s
			command_set = 0x03;
		}
		else
		{
			//No action
		}

		if(command_set > 0)
		{
			//Start downcounter
			/*
			 * TODO
			 */
			while(1)
			{
				break;
			}

			command_set = 0;
		}
		else
		{
			//action
		}



	}
}

void delay_ms(uint16_t n)
{
	uint16_t i;
	for(; n>0; n--)
		for(i=0; i<140;i++);
}

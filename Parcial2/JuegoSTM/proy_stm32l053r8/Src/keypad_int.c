/*
 * keypad_interface.c
 *
 *      Author: hardware
 */


#include <stdint.h>
#include "stm32l053xx.h"

#define  ALL_COLS_ON  	0xABCD
#define  COL0		  	(1<<7)
#define  COL1 		  	(1<<8)
#define  COL2		  	(1<<9)
#define  COL3 		  	(1<<10)
#define  NO_KEY_PRESSED	0x01E0

uint8_t	col_select = 0x00;
uint8_t key_pressed = 50;

void delay_ms(uint16_t n);

int main(void)
{

	//ENABLE IOPENR CLKA
	//ENABLE IOPENR CLKC

	//SEGMENT PORT, MODER = 01 :: OUTMODE
	/*PA4 S_A
	 *PA5 S_B
	 *PA6 S_C
	 *PA7 S_D
	 *PA8 S_E
	 *PA9 S_F
	 *PA10 S_G
	 *PA11 S_DP
	 */

	//DISPLAY SEL PORT, MODER = 01 :: OUTMODE
	/* PC0 D_0
	 * PC1 D_1
	 * PC2 D_2
	 * PC3 D_3
	 */

	// Keypad
	// x4 INPUT PORTS GPIOB B[8:5]
		//CONFIG ODR
	// x4 OUTPUT PORTS GPIOC
		//CONFIG ODR

	RCC->IOPENR |= 1<<2; //Enciendo CLK GPIOC

	GPIOC->MODER &= ~(1<<24); //PC12 como input
	GPIOC->MODER &= ~(1<<25); //PC12 como input

	GPIOC->PUPDR |= 1<<24;
	GPIOC->PUPDR &= ~(1<<24);
	GPIOC->PUPDR |= 1<<25;

	while(1)
	{
		print_7_segment(0,key_pressed);

		switch(col_select)
		{
			case 0:
			{
				GPIOC->BSRR |= ALL_COLS_ON;
				GPIOC->BSRR |= COL0<<16;
				if(GPIOB->IDR & NO_KEY_PRESSED)
				{
					// no action
				}
				else
				{
					if( !(GPIOC->IDR & (1<<5)) )
					{
						//ROW0 was pressed
						//DIGIT 1 was selected
						key_pressed = 0x01;
					}
					else if(!(GPIOC->IDR & (1<<6)))
					{
						//ROW1 was pressed
						//DIGIT 4 was selected
						key_pressed = 0x04;
					}
					if( !(GPIOC->IDR & (1<<7)) )
					{
						//ROW0 was pressed
						//DIGIT 7 was selected
						key_pressed = 0x07;
					}
					else if(!(GPIOC->IDR & (1<<8)))
					{
						//ROW1 was pressed
						//DIGIT * was selected
						key_pressed = 0x14;
					}
					else
					{
						//
					}
				}
				break;
			}
			case 1:
			{
				GPIOC->BSRR |= ALL_COLS_ON;
				GPIOC->BSRR |= COL1<<16;
				if(GPIOB->IDR & NO_KEY_PRESSED)
				{
					// no action
				}
				else
				{
					if( !(GPIOC->IDR & (1<<5)) )
					{
						//ROW0 was pressed
						//DIGIT 2 was selected
						key_pressed = 0x02;
					}
					else if(!(GPIOC->IDR & (1<<6)))
					{
						//ROW1 was pressed
						//DIGIT 5 was selected
						key_pressed = 0x05;
					}
					if( !(GPIOC->IDR & (1<<7)) )
					{
						//ROW0 was pressed
						//DIGIT 8 was selected
						key_pressed = 0x08;
					}
					else if(!(GPIOC->IDR & (1<<8)))
					{
						//ROW1 was pressed
						//DIGIT 0 was selected
						key_pressed = 0x00;
					}
					else
					{
						//
					}
				}
				break;
			}
			case 2:
			{
				GPIOC->BSRR |= ALL_COLS_ON;
				GPIOC->BSRR |= COL2<<16;
				break;
			}
			case 3:
			{
				GPIOC->BSRR |= ALL_COLS_ON;
				GPIOC->BSRR |= COL3<<16;
				break;
			}
			default:
			{
				break;
			}
		}

		delay_ms(50);

	}
}

void delay_ms(uint16_t n)
{
	uint16_t i;
	for(; n>0; n--)
		for(i=0; i<140;i++);
}

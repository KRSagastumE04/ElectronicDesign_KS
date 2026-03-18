/*
 * 7_segment_4_digit.c
 *
 *      Author: hardware
 */

#include "7_segment_4_digit.h"
#include "stm32l053xx.h"

uint8_t display_refresh = 0;

void print_7_segments(uint8_t Data_on_3, uint8_t Data_on_2, uint8_t Data_on_1, uint8_t Data_on_0);
uint8_t decode(uint8_t val);

void print_7_segments(uint8_t Data_on_3, uint8_t Data_on_2, uint8_t Data_on_1, uint8_t Data_on_0)
{
		if(display_refresh == 0x00) //DISPLAY 0, PRINT DIGIT #0, BITWISE STRATEGY
		{
			GPIOC->ODR &= ~(1<<0|1<<1|1<<2|1<<3); //Turns off all displays BITWISE
			GPIOC->ODR |= 1<<0;					  //Turns on D_0 BITWISE
			GPIOA->ODR |= (((1<<S_A)|(1<<S_B)|(1<<S_C)|(1<<S_D)|(1<<S_E)|(1<<S_F)|(1<<S_G)|(1<<S_DP))); //Turn off all segments BITWISE
			GPIOA->ODR &= ~(((1<<S_A)|(1<<S_B)|(1<<S_C)|(1<<S_D)|(1<<S_E)|(1<<S_F))); //Turn on segments for number 0 BITWISE
		}
		else if(display_refresh == 0x01)//DISPLAY 1, PRINT DIGIT #1, MASK STRATEGY
		{
			GPIOC->ODR &= ~ALL_DISPLAYS; //Turns off all displays MASK
			GPIOC->ODR |= D_1;			 //Turns on D_1 MASK
			GPIOA->ODR |= CA_ALL_SEG; 	 //Turn off all segments MASK
			GPIOA->ODR &= ~CA_DIGIT_0;    //Turn on segments for number 1 MASK
		}
		else if(display_refresh == 0x02) //DISPLAY 2, PRINT DIGIT #0, BITWISE + BSSR
		{
			GPIOC->BSRR |= ((1<<0|1<<1|1<<2|1<<3))<<16; //Turns off all displays
			GPIOC->BSRR |= 1<<2;						//Turns on D_2
			GPIOA->BSRR |= ((1<<S_A)|(1<<S_B)|(1<<S_C)|(1<<S_D)|(1<<S_E)|(1<<S_F)|(1<<S_G)|(1<<S_DP));//Turn off all segments with mask and BSSR
			GPIOA->BSRR |= (((1<<S_A)|(1<<S_B)|(1<<S_C)|(1<<S_D)|(1<<S_E)|(1<<S_F)) <<16); //Turn on segments for number 0 with BSSR
		}
		else if(display_refresh == 0x03) //DISPLAY 3, PRINT DIGIT #1, MASK + BSSR
		{
			GPIOC->BSRR |= (ALL_DISPLAY <<16); //Turns off all displays
			GPIOC->BSRR |= decode(Data_on_3);  //Turns on D_3
			GPIOA->BSRR |= CA_ALL_SEG_OFF;	   //Turn off all segments with mask and BSSR
			GPIOA->BSRR |= (CA_DIGIT_1 << 16); //Turn on segments for number 1 with BSSR
		}

		display_refresh++; //ALT display_refresh = display_refresh + 1 ;

		if(display_refresh==0x04)
		{
			display_refresh=0;			//Resets counter
		}

		delay_ms(1000);					//Refreshing rate
	}
}

uint8_t decode(uint8_t val)
{
	if(val==0)
		return CK_DIGIT_0	;
	else if(val == 1)
		return CK_DIGIT_1;
}


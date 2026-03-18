/*
 * 7segment_4digits.c
 *
 *      Author: hardware
 */

#include <stdint.h>
#include "stm32l053xx.h"
#include "7_segment_4_digit.h"

void delay_ms(uint16_t n);

uint8_t display_refresh = 0x00;

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
	while(1)
	{
		//Display number 1010, using Common Anode CA 7 segment displays

		print_7_segments(0, 0, 0, 2); // D0 D1 D2 D3


}

void delay_ms(uint16_t n)
{
	uint16_t i;
	for(; n>0; n--)
		for(i=0; i<230;i++);
}

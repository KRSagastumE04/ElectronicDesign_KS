/*
 * FSM_example.c
 *      Author: hardware
 */


#include <stdint.h>
#include "stm32l053xx.h"

/*
 * =0 ;; L1-R L2-G
 * =1 ;; L1-R L2-Y
 * =2 ;; L1-G L2-R
 * =3 ;; L1-Y L2-R
 * =4 ;; ?
 */
uint8_t main_controller = 0x00;
uint8_t fsm_seven_segment = 0x00;
uint8_t fsm_keypad = 0x00;

void delay_ms(uint16_t n);

int main(void)
{
	while(1){

		delay_ms(1);
		main_controller++;

		if(main_controller==10); //FSM_Seven_segment
		{
			main_controller = 0;
			enable_fsm_keypad++;
			switch(fsm_seven_segment){
				case 0:{
					//=0 ;; L1-R L2-G
					fsm_seven_segment++;
				}
				case 1:{
					//=1 ;; L1-R L2-Y
					fsm_seven_segment++;
				}
				case 2:{
					//=2 ;; L1-G L2-R
					fsm_seven_segment++;
				}
				case 3:{
					//=3 ;; L1-Y L2-R
					fsm_seven_segment=0x00;
				}
				default:{
					//=0 ;; L1-R L2-G
					fsm_seven_segment=0x00;
				}
			}
		}

		if(enable_fsm_keypad==10)
		{
			enable_fsm_keypad=0x00;

			switch (fsm_keypad)
			{

			}

		}

	}
}

void delay_ms(uint16_t n)
{
	uint16_t i;
	for(; n>0; n--)
		for(i=0; i<150;i++);
}

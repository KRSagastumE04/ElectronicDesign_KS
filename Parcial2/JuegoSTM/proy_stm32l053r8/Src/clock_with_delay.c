/*
 * 7segment_4digits.c
 *
 *      Author: hardware
 */

#include <stdint.h>
#include "stm32l053xx.h"

void delay_ms(uint16_t n);

uint8_t display_refresh = 0x00;

struct clock_variables{

	uint8_t hora_unidad = 0x00;
	uint8_t hora_decima = 0x00;
	uint8_t minuto_unidad = 0x00;
	uint8_t minuto_decima = 0x00;
	uint8_t segundo_unidad = 0x00;
	uint8_t segundo_decima = 0x00;
};



uint8_t program_stage = 0x00;


int main(void)
{
	struct clock_variables mywatch; //Vars for clock based in delays
	struct clock_variables mywatch_UTC_minus_6; //Vars for clock based in delays
	struct clock_variables alarm1; //Vars for clock alarm

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

	alarm1.hora_decima = 0;
	alarm1.hora_unidad = 6;
	alarm1.minuto_decima = 1;
	alarm1.minuto_unidad = 5;
	alarm1.segundo_decima = 0;
	alarm1.segundo_decima = 0;

	while(1)
	{
		delay_ms(10);

		if(display_refresh == 0x00)//DISPLAY 0 [minutos unidad], PINTAR NÚMERO 3
			{
				GPIOA->BSRR |=CK_ALL_SEG_OFF<<16;//APAGO TODOS LOS SEGMENTOS, COLOCARLOS 0
				GPIOC->BSRR |=ALL_DISPLAY<<16;//APAGO TODOS LOS DISPLAY, COLOCARLOS 0
				GPIOC->BSRR |=D_0;//ENCIENDO EL D_0, COLOCARLO 1
				GPIOA->BSRR |=decodificador(mywatch.minuto_unidad);//ENCIENDO EL NÚMERO 3
			}
			else if(display_refresh == 0x01)//DISPLAY 1 [minutos decima], PINTAR NÚMERO 2
			{
				GPIOC->BSRR |=ALL_DISPLAY<<16;//APAGO TODOS LOS DISPLAY, COLOCARLOS 0
				GPIOC->BSRR |=D_1;//ENCIENDO EL D_1, COLOCARLO 1
				GPIOA->BSRR |=CK_ALL_SEG_OFF<<16;//APAGO TODOS LOS SEGMENTOS, COLOCARLOS 0
				GPIOA->BSRR |=decodicador(mywatch.minuto_decima);//ENCIENDO EL NÚMERO 3
			}
			else if(display_refresh == 0x02)//DISPLAY 2 [hora unidad], PINTAR NÚMERO 0
			{
				GPIOC->BSRR |=ALL_DISPLAY<<16;//APAGO TODOS LOS DISPLAY, COLOCARLOS 0
				GPIOC->BSRR |=D_2;//ENCIENDO EL D_2, COLOCARLO 1
				GPIOA->BSRR |=CK_ALL_SEG_OFF<<16;//APAGO TODOS LOS SEGMENTOS, COLOCARLOS 0
				GPIOA->BSRR |=decodificador(hora_unidad);//ENCIENDO EL NÚMERO 0
			}
			else if(display_refresh == 0x03)//DISPLAY 3 , PINTAR NÚMERO 2
			{
				GPIOC->BSRR |=ALL_DISPLAY<<16;//APAGO TODOS LOS DISPLAY, COLOCARLOS 0
				GPIOC->BSRR |=D_3;//ENCIENDO EL D_3, COLOCARLO 1
				GPIOA->BSRR |=CK_ALL_SEG_OFF<<16;//APAGO TODOS LOS SEGMENTOS, COLOCARLOS 0
				GPIOA->BSRR |=CommunCatod_DIGIT_2;//ENCIENDO EL NÚMERO 2
			}

		display_refresh++;

		if(display_refresh == 0x03)
		{
			display_refresh = 0;
		}
		else
		{
			display_refresh++;
		}

		program_stage++;

		if(program_stage==100) //@1s has elapsed
		{
			program_stage = 0x00;
			mywatch.segundo_unidad++;

			if(mywatch.segundo_unidad == 10)
			{
				mywatch.segundo_decima++;
				mywatch.segundo_unidad = 0x00;
			}

			if(segundos_decima == 6)
			{
				minutos_unidad++;
				segundos_decima = 0x00;
			}

			if(minutos_unidad == 10)
			{
				minutos_decima++;
				minutos_unidad = 0x00;
			}

			if(minutos_decima == 6)
			{
				horas_unidad++;
				minutos_decima = 0x00;
			}

			if(horas_unidad == 10)
			{
				horas_decima++;
				horas_unidad = 0x00;
			}

			if((horas_decima == 2)&(horas_unidad == 4))
			{
				horas_unidad = 0x00;
				horas_decima = 0x00;
			}

			if(mywatch.segundo_unidad == alarm1.segundo_unidad
			&  mywatch.segundo_decima == alarm1.segundo_decima
			&  mywatch.minuto_unidad == alarm1.minuto_unidad
			&  mywatch.minuto_decima == alarm1.minuto_decima)
			{
				//action
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

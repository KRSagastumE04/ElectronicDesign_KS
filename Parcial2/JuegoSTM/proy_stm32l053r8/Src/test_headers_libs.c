/*
 * test_headers_libs.c
 *
 *      Author: hardware
 */

#include <stdint.h>
#include "stm32l053xx.h"

#define RS 0x10				//PC4 Mask for R/S, register select
#define RW 0x20				//PC5 Mask for R/W, read-write
#define EN 0x40				//PC6 Mask fir E, enable

void LCD_PINS_init(void);
void LCD_init(void);
void LCD_command(uint8_t command);
void LCD_data(uint8_t data);
void delay_ms(uint16_t n);

int main(void)
{
	//Enable CLK of IOPorts
		//Enable PORTC for control signals RW/RS/E
	RCC->IOPENR |= 1<<2;
		//Enable PORTB for data bits
	RCC->IOPENR |= 1<<1;

	LCD_PINS_init();
	LCD_init();



	while(1)
	{
		LCD_data('v');
		LCD_data('i');
		LCD_data('c');
		LCD_data('t');
		LCD_data('o');
		LCD_data('r');
	}

}

void LCD_init(void)
{
	delay_ms(30);		//Hold until Hitachi controller boots
	LCD_command(0x30);
	delay_ms(10);
	LCD_command(0x30);
	delay_ms(1);
	LCD_command(0x30);

	LCD_command(0x38);	//Programs Hitachi controller to 8bit, 2-line, 5x7 font
	LCD_command(0x06);	//Move cursor right after each char is latched
	LCD_command(0x01);	//Clear screen and return cursor to home
	LCD_command(0x0F);	//Turn on display and set blinking cursor

}

void LCD_PINS_init(void)
{
	//CONFIG control pins on PC port
		//PC4 as R/S pin Output mode
	GPIOC->MODER &= ~(1<<9);
		//PC5 as R/W pin Output mode
	GPIOC->MODER &= ~(1<<11);
		//PC6 as EN pin Output mode
	GPIOC->MODER &= ~(1<<13);

	//CONFIG data pins on PB port
		//PB0 as D0 pin Output mode
	GPIOB->MODER &= ~(1<<1);
		//PB1 as D1 pin Output mode
	GPIOB->MODER &= ~(1<<3);
		//PB2 as D2 pin Output mode
	GPIOB->MODER &= ~(1<<5);
		//PB3 as D3 pin Output mode
	GPIOB->MODER &= ~(1<<7);
		//PB4 as D4 pin Output mode
	GPIOB->MODER &= ~(1<<9);
		//PB5 as D5 pin Output mode
	GPIOB->MODER &= ~(1<<11);
		//PB6 as D6 pin Output mode
	GPIOB->MODER &= ~(1<<13);
		//PB7 as D7 pin Output mode
	GPIOB->MODER &= ~(1<<15);
}

void LCD_command(uint8_t command)
{
	GPIOC->BSRR |= RS<<16;	//RS = 0
	GPIOC->BSRR |= RW<<16;	//RW = 0

	GPIOB->ODR  = command; //Writes command on data bus D[7:0]
	GPIOC->BSRR |= EN;		//Set Enable high
	delay_ms(0);			//Wait for data to be latched
	GPIOC->BSRR |= EN<<16;	//Clear Enable low
	delay_ms(2);			//Wait command to be process by controller
}

void LCD_data(uint8_t data)
{
	GPIOC->BSRR |= RS;		//RS = 1
	GPIOC->BSRR |= RW<<16;	//RW = 0
	GPIOB->ODR  = data; 	//Writes data on data bus D[7:0]
	GPIOC->BSRR |= EN;		//Set Enable high
	delay_ms(0);			//Wait for data to be latched
	GPIOC->BSRR |= EN<<16;	//Clear Enable low
}

void delay_ms(uint16_t n)
{
	uint16_t i;
	for(; n>0; n--)
		for(i=0; i<150;i++);
}

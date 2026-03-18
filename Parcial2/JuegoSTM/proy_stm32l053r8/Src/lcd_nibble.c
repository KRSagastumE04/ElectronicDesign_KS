/*
 * press_portc.c
 *
 */


//conexion PC4 PC5 PC6 PC7 --> R1 R2 R3 R4
//conexion PC0 PC1 PC2 PC3 --> C1 C2 C3 C4

#include <stdint.h>
#include "stm32l053xx.h"

void delay(void);
void delayMs(int n);
void keypad_init(void);
char keypad_getkey(void);
void LED_blink(int value);

void LCD_nibble_write(char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(char data);
void LCD_init(void);
void PORTS_init(void);

#define RS 0x40     /* PA6 mask for reg select */
#define EN 0x100     /* PA8 mask for enable */
#define OUT_PA 0x15000 //Establecemos como output PA6,PA7 y PA8
#define OUT_PB 0x0055 //Establecemos como output PB0-7


int main(void) {



    LCD_init();
    while(1) {

        	LCD_data('1');
        	delayMs(100);

        	LCD_data('2');
        	delayMs(100);

    }
}

/* turn on or off the LEDs according to the value */
void LED_blink(int value) {
    value %= 17;                    /* cap the max count at 16 */

    for (; value > 0; value--) {
        GPIOA->BSRR = 0x00000020;   /* turn on LED */
        delayMs(200);
        GPIOA->BSRR = 0x00200000;   /* turn off LED */
        delayMs(200);
    }
    delayMs(200);
}

/* initialize GPIOB/C then initialize LCD controller */
void LCD_init(void) {
    PORTS_init();

    delayMs(20);                /* LCD controller reset sequence */
    LCD_nibble_write(0x30, 0);
    delayMs(5);
    LCD_nibble_write(0x30, 0);
    delayMs(1);
    LCD_nibble_write(0x30, 0);
    delayMs(1);

    LCD_nibble_write(0x20, 0);  /* use 4-bit data mode */
    delayMs(1);
    LCD_command(0x28);          /* set 4-bit data, 2-line, 5x7 font */
    LCD_command(0x06);          /* move cursor right */
    LCD_command(0x01);          /* clear screen, move cursor to home */
    LCD_command(0x0F);          /* turn on display, cursor blinking */
}

void PORTS_init(void) {
    RCC->AHBENR |=  1 << 1;	/* activamos GPIOB clock */
    RCC->AHBENR |=  1 << 2;	/* activamos GPIOC clock */


    GPIOA->MODER |=  OUT_PA;    //output mode de los PA5-7
    GPIOA->MODER &=  ~EN;       //Apagamos el EN and R/W

    GPIOB->MODER |=  OUT_PB;    //output mode de PB0-7
}

void LCD_nibble_write(char data, unsigned char control) {
    /* populate data bits */
    GPIOB->BSRR = 0x00F00000;       /* clear data bits */
    GPIOB->BSRR = data & 0xF0;      /* set data bits */

    /* set R/S bit */
    if (control & RS)
        GPIOA->BSRR = RS;
    else
        GPIOA->BSRR = RS << 16;

    /* pulse E */
    GPIOA->BSRR = EN;
    delayMs(0);
    GPIOA->BSRR = EN << 16;
}

void LCD_command(unsigned char command) {
    LCD_nibble_write(command & 0xF0, 0);    /* upper nibble first */
    LCD_nibble_write(command << 4, 0);      /* then lower nibble */

    if (command < 4)
        delayMs(2);         /* command 1 and 2 needs up to 1.64ms */
    else
        delayMs(1);         /* all others 40 us */
}

void LCD_data(char data) {
    LCD_nibble_write(data & 0xF0, RS);      /* upper nibble first */
    LCD_nibble_write(data << 4, RS);        /* then lower nibble */

    delayMs(1);
}



/* make a small delay */
void delay(void) {
    int i;
    for (i = 0; i < 20; i++) ;
}

/* 16 MHz SYSCLK */
void delayMs(int n) {
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195; i++) ;
}





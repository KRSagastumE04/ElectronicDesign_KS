#include <stdint.h>
#include "stm32l053xx.h"

void delayMs(uint16_t n);

uint8_t seven_segs_sequence = 0x00;

struct time {   // Structure declaration
	uint8_t segundos_unidad;
	uint8_t segundos_decena;
}; // End the structure with a semicolon

struct time mihora;


int main(void) {

	 __disable_irq();                    //Disable global interrupt requests, good practice


	//Enable HSI 16MHz
		//HSI on
	RCC->CR |= (1<<0);
		//HSI16 as SYSCLK
	RCC->CFGR |= (1<<0);

    RCC->IOPENR |= 1<<0;	               //Enable CLK PORTA
    RCC->IOPENR |= 1<<1;	               //Enable CLK PORTB
    RCC->IOPENR |= 1<<2;                   //Enable CLK PORTC
    RCC->APB2ENR |= 1<<0;                  //Enable SYSCFG clock

    //GPIOA PIN 5 as output, on board LED
    GPIOA->MODER &=~(1<<11);

    //GPIOC PIN 13 a input, on board button
    GPIOC->MODER &= 1<<26;
    GPIOC->MODER &= 1<<27;

	//PB10 AS INPUT
	GPIOB->MODER &= ~(1<<20);
	GPIOB->MODER &= ~(1<<21);
	//PB10 PU ENABLE
	GPIOB->PUPDR |= (1<<20);

	//USART 2 PIN CONFIGURATION
		//ALTERNATIVE FUCTION PA2 AND PA3 (10-> MODE2 AND MODE3)
	GPIOA->MODER &=~(1<<4);
	GPIOA->MODER &=~(1<<6);
		//PA2 & PA3 Mapped as AF4
	GPIOA->AFR[0] |=1<<10;
	GPIOA->AFR[0] |=1<<14;

	//USART2 Config
		//USART2 CLOCK ENABLE ON RCC APB1ENR REGISTER
	RCC->APB1ENR |=1<<17;
		//USART2 BaudRate 9600@16Mhz HSI
	USART2->BRR=0x682;
		//ENABLE TX AND RX
	USART2->CR1 |=(1<<2)|(1<<3);
		//ENABLE PERIPHERIAL
	USART2->CR1 |=1<<0;
		////USART2 INTERRUPT CONFIG
	USART2->CR1 |= 1<<5;

	//EXTI13 CONFIG
    SYSCFG->EXTICR[3] |= 1<<5;          //Select PORTC for EXTI line13 EXTI13
    EXTI->IMR |= 1<<13;                 //Unmask EXTI13
    EXTI->FTSR |= 1<<13;                //Falling edge selected

	//EXTI10 CONFIG
	SYSCFG->EXTICR[2] |= (1<<8);		//Select PORTB for EXTI line10 EXTI10
	EXTI->IMR |= (1<<10); 				//Unmask EXTI10
	EXTI->FTSR |= (1<<10); 				//Falling edge selected (PU enable)

	//Systick configure @1s
    SysTick->LOAD = 16000000-1;     	//Reload down counter
    SysTick->VAL = 0;					//Initial value
    SysTick->CTRL = 7;              	//Enable Systick interrupt

    //Configure TIM2 @1s
    RCC->APB1ENR |= (1<<0);   			//Enable Peripheral
    TIM2->PSC = 16000 - 1;          	//Prescaler
    TIM2->ARR = 1000 - 1;           	//Automatic reload register
    TIM2->CR1 |= (1<<0);                //Enable TIM2
    TIM2->DIER |= (1<<0);               //Enable TIM2 interrupts

    //Configure TIM21 @.250s
    RCC->APB2ENR |= (1<<2);   			//Enable Peripheral
    TIM21->PSC = 16000 - 1;          	//Prescaler
    TIM21->ARR = 250 - 1;           	//Automatic reload register
    TIM21->CR1 |= (1<<0);               //Enable TIM2
    TIM21->DIER |= (1<<0);              //Enable TIM21 interrupts


	NVIC_EnableIRQ(USART2_IRQn);		//Enable specific interrupt (individual) source
    NVIC_EnableIRQ(EXTI4_15_IRQn);		//Enable specific interrupt (group) source to enter the NVIC
    NVIC_EnableIRQ(TIM2_IRQn);      	//Enable specific interrupt (individual) source
    NVIC_EnableIRQ(TIM21_IRQn);      	//Enable specific interrupt (individual) source

    __enable_irq();                     //Enable global interrupts

    while(1) {
    	//Doing something
    }
}

void TIM21_IRQHandler(void) //@0.250s
{
    TIM21->SR = 0;                   	//Clear interrupt flag
    switch (seven_segs_sequence)
	{
		case 0:
		{
			//apagar (MOSFET) el ultimo que es el que esta en el switch case 5
			//encender el primero
			//pintar en pantalla
			//que se pinta en pantalla? la cuanta que controla/gestiona el systick
			seven_segs_sequence++;
			break;
		}
		case 1:
		{
			//apagar el ultimo que es el que esta en el switch case 0
			//encender el primero
			//pintar en pantalla
			//que se pinta en pantalla? la cuanta que controla/gestiona el systick
			seven_segs_sequence++;
			break;
		}
		case 2:
		{
			//apagar el ultimo que es el que esta en el switch case 1
			//encender el primero
			//pintar en pantalla
			//que se pinta en pantalla? la cuanta que controla/gestiona el systick
		}


		case 2:
			{
				//apagar el ultimo que es el que esta en el switch case 1
				//encender el primero
				//pintar en pantalla
				//que se pinta en pantalla? la cuanta que controla/gestiona el systick
				seven_segs_sequence=0;
				break;
			}

		default:
		{

		}

	}
    //GPIOA->ODR ^= 1<<5;   				//Toggle PA5
}

void TIM2_IRQHandler(void)
{
    TIM2->SR = 0;                   	//Clear interrupt flag
    GPIOA->ODR ^= 1<<5;   			//Toggle PA5
}

void SysTick_Handler(void)
{
	//GPIOA->ODR ^= 1<<5;   				//Toggle PA5
    //Note SysTick interrupt doesn't need user-explicit action to clear an interrupt flag
	//Note interrupt was not required to passed via the NVIC NVIC_EnableIRQ due its a core M0+ peripheral
	mihora.segundos_decena = 0x10;
}

void USART2_IRQHandler(void) {
    uint8_t c;
    // Reading the RXIN bit automatically clears the flag. Compared to EXTI that user needs to clears it
    if (USART2->ISR & 1<<5) 			//Read data register not empty thus, clearing the interrupt flag
    {
        c = USART2->RDR;             		//Read char, transmit from Received Data Register
    }
    else
    {
    	//Should not happen, if the program entered the ISR, the bit 5 of USART2->ISR should not be empty
    }
}

void EXTI4_15_IRQHandler(void) 			//Important event(s) handler
{

	if(EXTI->PR == 0x400)
	{
		//Important action when EXTI10 was triggered
		EXTI->PR |= (1<<10);			//Clear pending register for EXTI10
	}
	else if (EXTI->PR & (1<<13))
	{
		//Important action when EXTI13 was triggered
		EXTI->PR |= (1<<13); 			//Clear pending register for EXTI13
	}
	else
	{
		//-_- ... event handler not programmed. WARNING/ERROR!
	}

	//GPIOA->ODR ^= (1<<5); 			//Default action
    EXTI->PR = 1<<13;          			//Clear flag event [user-explicit], otherwise NVIC engine throws another interrupt
}

/* 16 MHz SYSCLK */
void delayMs(uint16_t n) {
	uint16_t i;
    for (; n > 0; n--)
        for (i = 0; i < 3195; i++) ;
}

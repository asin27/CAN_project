/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "../trng/adc.h"
#include "timer.h"
#include "../led/led.h"
#include "../GLCD/GLCD.h"
#include <stdio.h>
#include "CAN.h"


/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

int blink_mask = 0xFF;

extern int check;
extern unsigned char AD_current;
extern uint8_t eKey[16];
extern uint8_t iv[16];
extern uint8_t sCode;
int ready = 0;

void keyGeneration(){
	int i;
	char s1[16];
	char s2[16];

	for(int i=0; i<16; i++){
		ADC_start_conversion();
		while(!ready);
		eKey[i] = AD_current;
		ready = 0;
	}
	for(int i=0; i<16; i++){
		ADC_start_conversion();
		while(!ready);
		iv[i] = AD_current;
		ready = 0;
	}
	ADC_start_conversion();
	while(!ready);
	sCode = AD_current;
	ready = 0;
	
	GUI_Text(10, 20, (unsigned char*)"New key, iv and code generated: ", White, Blue);

	for(i=0; i<16; i++){
		sprintf(s1,"%02x " , eKey[i]);
		sprintf(s2,"%02x ", iv[i]);
		GUI_Text(10 + i*16, 40, (unsigned char*)s1, White, Blue);
		GUI_Text(10 + i*16, 60, (unsigned char*)s2, White, Blue);
	}
}


void TIMER0_IRQHandler (void)
{
	keyGeneration();
	
	//do handshake here
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER1_IRQHandler (void)
{  	
	
	LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}


/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: to control led through EINT and joystick buttons
 * Note(s):
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/
                  
#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "trng/adc.h"
#include "CAN.h"
#include "GLCD/GLCD.h"
#include "TouchPanel/TouchPanel.h"
#include <security.h>

struct AES_ctx ctx;
uint8_t eKey[16] = "AAAAAAAAAAAAAAAA";
uint8_t iv[16] = "BBBBBBBBBBBBBBBB";
uint8_t sCode = 0xa;

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  	
	SystemInit();  												/* System Initialization (i.e., PLL)  */

	LCD_Initialization();
	LCD_Clear(Blue);
	ADC_init();
	
	ctx = AES_init(eKey, iv);
	
	NVIC_SetPriority(ADC_IRQn, 1);
	NVIC_SetPriority(TIMER0_IRQn, 2);

	init_timer(0, 0x2FAF080);
	enable_timer(0);
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
	
  while (1) {                           /* Loop forever                       */	
		__ASM("wfi");
  }

}

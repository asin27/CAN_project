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
#include "LPC17xx.H"      
#include "timer/timer.h"
#include "trng/adc.h"
#include "highcan.h"
#include "GLCD/GLCD.h"
#include "TouchPanel/TouchPanel.h"
#include <security/security.h>

struct AES_ctx ctx;
struct AES_ctx ack_ctx[4];
uint8_t eKey[16] = "AAAAAAAAAAAAAAAA";
uint8_t iv[16] = "BBBBBBBBBBBBBBBB";
uint8_t oldKey[16] = "AAAAAAAAAAAAAAAA";
uint8_t oldIv[16] = "BBBBBBBBBBBBBBBB";
uint8_t sCode = 0xa;
int good = 0, bad = 0;
uint8_t hK[32];
uint8_t hIv[32];
uint8_t finalMessage[96];

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  	
	SystemInit();  												/* System Initialization (i.e., PLL)  */
	
	for(int i=0; i<32; i++) NVIC_SetPriority(i, 5);

	LCD_Initialization();
	LCD_Clear(Blue);
	ADC_init();
	hCAN_init(1, CAN_5Kbps);
	hCAN_setID(0x4);
	
	ctx = AES_init(oldKey, oldIv);
	NVIC_SetPriority(CAN_IRQn, 0);
	NVIC_SetPriority(ADC_IRQn, 1);
	NVIC_SetPriority(TIMER0_IRQn, 2);

	init_timer(0, 10 * 0x2FAF080);
	enable_timer(0);
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
	
  while (1) {                           /* Loop forever                       */	
		__ASM("wfi");
  }

}

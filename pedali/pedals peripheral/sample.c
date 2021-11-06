/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: to control brake and gas pedals through EINT buttons
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
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "adc/adc.h"
#include "GLCD/GLCD.h"
#include "TouchPanel/TouchPanel.h"
#include "CAN/headers/highcan.h"
#include <security/security.h>

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/


uint8_t key_aes[16] = {'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'};
uint8_t iv[16] = {'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B'};
struct AES_ctx ctx;
struct AES_ctx ctx_dec;
struct AES_ctx newParam_dec;
struct AES_ctx ack;
unsigned char keyDgst[32] = {0};
unsigned char ivDgst[32] = {0};
unsigned char newKey[16] = {0};
unsigned char newIv[16] = {0};




 int main (void) {
  	
	SystemInit();  												/* System Initialization (i.e., PLL)  */
  LED_init();                           /* LED Initialization                 */
  
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	enable_RIT();													/* RIT enabled												*/
	ADC_init();
	hCAN_init(1, CAN_5Kbps);
	//hCAN_init(2, CAN_5Kbps);
	hCAN_setID(0x3);
	LCD_Initialization();
	LCD_Clear(bgnd);
	
	//init_timer(2, 0x2625A0);
	//enable_timer(2);
	ctx	= AES_init(key_aes, iv);
	ctx_dec = AES_init(key_aes, iv);
	newParam_dec = AES_init(key_aes, iv);
	ack = AES_init(key_aes, iv);
	TP_Init();
	for (int i = 0; i < 34; i++){
		NVIC_SetPriority(i, 5);
	}
	NVIC_SetPriority(CAN_IRQn, 4);
	 
	//digest(a, key_aes, f1);
	//digest(iv, key_aes, f2);
	
	
	GUI_Text(132, 5,(unsigned char *) "Off", White, Blue);
	GUI_Text(50, 292,(unsigned char *) "Gas", White, Blue);
	GUI_Text(155, 292, (unsigned char *)"Brake", writ, bgnd);
	
	BUTTON_init();												/* BUTTON Initialization              */		
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
		
	
	
  while (1) {                           /* Loop forever                       */	
		__ASM("wfi");
  }

}

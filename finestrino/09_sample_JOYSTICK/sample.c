/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: to control led through debounced buttons and Joystick
 *        	- key1 switches on the led at the left of the current led on, 
 *					- it implements a circular led effect,
 * 					- joystick select function returns to initial configuration (led11 on) .
 * Note(s): this version supports the LANDTIGER Emulator
 * Author: 	Paolo BERNARDI - PoliTO - last modified 15/12/2020
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
#include "joystick/joystick.h"
#include "GLCD/GLCD.h"
#include "security/security.h"
#include <highcan.h>

uint8_t fLevel[4] = {12,12,12,12};
unsigned char key[3][8];

uint8_t key_aes[16] = {'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'};
uint8_t iv[16] = {'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B'};

void init_key(){
	int i,j;
	for(i=0; i<3;i++)
		for(j=0;j<8;j++)
			key[i][j] = 0x41;
}

struct AES_ctx ctx_enc, ctx_dec, break_ctx;
struct AES_ctx ctx_dec_key;

uint8_t d[16] = {'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'};

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif
/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  int i;
	SystemInit();  												/* System Initialization (i.e., PLL)  */
  LED_init();                           /* LED Initialization                 */
  BUTTON_init();												/* BUTTON Initialization              */
	hCAN_init(1, CAN_5Kbps);
	hCAN_setID(0x1);
	joystick_init();											/* Joystick Initialization            */
	init_RIT(0x004C4B40);//init_RIT(0x004C4B40);			/* RIT Initialization 50 msec       	*/
	enable_RIT();							/* RIT enabled												*/
	LCD_Initialization();	
	init_timer(0, 0xbebc20);
	init_timer(1, 0xbebc20);
	LED_Out(15);
	init_key();
	ctx_enc = AES_init(key_aes, iv);
	ctx_dec = AES_init(key_aes, iv);
	break_ctx = AES_init(key_aes, iv);
	ctx_dec_key = AES_init(key_aes, iv);
	
	LCD_Clear(White);
	GUI_Text(55, 10, (uint8_t *)"CAs CAN project:", Black, White);
	GUI_Text(80, 30, (uint8_t *)"Finestrino 0", Black, White);
	LCD_DrawLine(0, 55, 240, 55, Black);
	LCD_DrawLine(0, 265, 240, 265, Black);
	LCD_DrawLine(40, 55, 40, 265, Black);
	LCD_DrawLine(200, 55, 200, 265, Black);
	for(i=0; i<13; i++)
		drawLineOfSquares(i, Grey);
		//drawSquare(7, i, Black);
		
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
		
  while (1) {                           /* Loop forever                       */	
		__ASM("wfi");
  }

}


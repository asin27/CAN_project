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
#include "adc/adc.h"
#include "GLCD/GLCD.h"
#include "TouchPanel/TouchPanel.h"
#include "drawing/draw.h"
#include "highcan.h"
#include "../security/security.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif
/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
 
unsigned char key[3][8] = {"AAAAAAAA", 
									"AAAAAAAA", 
									"AAAAAAAA"};

uint8_t keyAES[16] = "AAAAAAAAAAAAAAAA"; 
uint8_t ivAES[16] = "BBBBBBBBBBBBBBBB";
 
struct AES_ctx ctx;
struct AES_ctx dec_ctx;
struct AES_ctx break_dec_ctx;									
char msg[16] = {0};
uint8_t lock = 1;
 
int main (void) {
  
	
	
 	SystemInit();  												/* System Initialization (i.e., PLL)  */
  LED_init();                           /* LED Initialization                 */
  BUTTON_init();												/* BUTTON Initialization              */
	joystick_init();											/* Joystick Initialization            */
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	enable_RIT();													/* RIT enabled												*/
	ADC_init();
	hCAN_init(1, CAN_5Kbps);
	hCAN_setID(2);
	
	ctx = AES_init(keyAES, ivAES);
	dec_ctx = AES_init(keyAES, ivAES);
	break_dec_ctx = AES_init(keyAES, ivAES);
	
	
	//init_timer(1, 0x002625A0);
	init_timer(1, 0x00bebc20/2);
	init_timer(2, 0x00bebc20/2); 						/* 12,5 Mhz */
	LCD_Initialization();
	LCD_Clear(Blue);
	
	//LED_On(0);
	
	GUI_Text(45, 20,(uint8_t *) "CAs PoliTO 2020-21", Yellow, Blue);
	GUI_Text(24, 40,(uint8_t *) "LANDTIGER board - Lights", White, Blue);
	clear_box(45, 80, Red);
	GUI_Text(69, 80,(uint8_t *) "Hazard Lights", White, Blue);
	clear_box(45, 100, Red);
	GUI_Text(69, 100,(uint8_t *) "Left Turn Signal", White, Blue);
	clear_box(45, 120, Red);
	GUI_Text(69, 120,(uint8_t *) "Right Turn Signal", White, Blue);
	clear_box(45, 140, Red);
	GUI_Text(69, 140,(uint8_t *) "Low Beams", White, Blue);
	clear_box(45, 160, Red);
	GUI_Text(69, 160,(uint8_t *) "High Beams", White, Blue);
	clear_box(45, 180, Red);
	GUI_Text(69, 180,(uint8_t *) "Break Lights", White, Blue);
	
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
		
  while (1) {                           /* Loop forever                       */	
		__ASM("wfi");
  }

}

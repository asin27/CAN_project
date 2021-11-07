/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "../drawing/draw.h"
#include "../GLCD/GLCD.h"
#include "../security/security.h"
#include "../security/des.h"
#include "highcan.h"


/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

volatile uint8_t k1=0;
volatile uint8_t k2=0;
volatile uint8_t k3=0;
volatile uint8_t position=0;
volatile uint8_t hazardLights=0;
volatile uint8_t leftTurnSignal = 0;
volatile uint8_t rightTurnSignal = 0;

extern char msg[16];


									
extern struct AES_ctx ctx;
extern struct AES_ctx dec_ctx;
																		

extern uint8_t lock;
								
									
void RIT_IRQHandler (void)
{					
	static int select=0;
	static uint8_t left=0;
	static uint8_t right=0;
	static uint8_t lowBeamsState=0;
	static uint8_t highBeamsState=0;
	static uint8_t breakLights=0;
	

	
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		/* Joytick Select pressed */
		select++;
		switch(select){
			case 1:
				if(rightTurnSignal==1){
					clear_box(45, 120, Red);
					correctRed(lowBeamsState, highBeamsState, breakLights);
					correctRed2(leftTurnSignal, rightTurnSignal, hazardLights);
					rightTurnSignal=0;
					msg[2] = rightTurnSignal;
					LED_Off(0);
				}
				if(leftTurnSignal==1){
					clear_box(45, 100, Red);
					correctRed(lowBeamsState, highBeamsState, breakLights);
					correctRed2(leftTurnSignal, rightTurnSignal, hazardLights);
					leftTurnSignal=0;
					msg[1] = leftTurnSignal;
					LED_Off(3);
				}
				if(hazardLights == 0){
					clear_box(45, 80, Green);
					correctGreen(lowBeamsState, highBeamsState, breakLights);
					enable_timer(1);
					hazardLights = 1;
					msg[0] = hazardLights;
					lock = 0;
					enable_timer(2);
				}else {
					LED_Off(0);
					LED_Off(1);
					LED_Off(2);
					LED_Off(3);
					clear_box(45, 80, Red);
					correctRed(lowBeamsState, highBeamsState, breakLights);
					correctRed2(leftTurnSignal, rightTurnSignal, hazardLights);
					hazardLights = 0;
					msg[0] = hazardLights;
					disable_timer(2);
					reset_timer(2);
					disable_timer(0);
					reset_timer(0);
					disable_timer(1);
					reset_timer(1);
				}
				break;
			default:
				break;
		}
	}
	else{
			select=0;
			
			
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick left pressed */
		left++;
		switch(left){
			case 1:
				position = 3;
				if(hazardLights != 0) {
					clear_box(45, 80, Red);
					correctRed(lowBeamsState, highBeamsState, breakLights);
					correctRed2(leftTurnSignal, rightTurnSignal, hazardLights);
					hazardLights = 0;
					msg[0] = hazardLights;
					LED_Off(0);
					LED_Off(1);
					LED_Off(2);
					LED_Off(3);
				}
				if(rightTurnSignal==1){
					clear_box(45, 120, Red);
					correctRed(lowBeamsState, highBeamsState, breakLights);
					correctRed2(leftTurnSignal, rightTurnSignal, hazardLights);
					rightTurnSignal=0;
					msg[2] = rightTurnSignal;
					LED_Off(0);
				}
				if(leftTurnSignal == 0){
					clear_box(45, 100, Green);
					correctGreen(lowBeamsState, highBeamsState, breakLights);
					enable_timer(1);
					leftTurnSignal=1;
					msg[1] = leftTurnSignal;
					lock = 0;
					enable_timer(2);
				}else {
					LED_Off(3);
					clear_box(45, 100, Red);
					correctRed(lowBeamsState, highBeamsState, breakLights);
					correctRed2(leftTurnSignal, rightTurnSignal, hazardLights);
					leftTurnSignal=0;
					msg[1] =leftTurnSignal;
					disable_timer(2);
					reset_timer(2);
					disable_timer(0);
					reset_timer(0);
					disable_timer(1);
					reset_timer(1);
				}
				break;
			default:
				break;
		}
	}
	else{
			left=0;
	}
	
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick right pressed */
		right++;
		switch(right){
			case 1:
				position = 0;
				if(hazardLights != 0) {
					clear_box(45, 80, Red);
					correctRed(lowBeamsState, highBeamsState, breakLights);
					correctRed2(leftTurnSignal, rightTurnSignal, hazardLights);
					hazardLights = 0;
					msg[0] = hazardLights;
					LED_Off(0);
					LED_Off(1);
					LED_Off(2);
					LED_Off(3);
				}
				if(leftTurnSignal==1){
					clear_box(45, 100, Red);
					correctRed(lowBeamsState, highBeamsState, breakLights);
					correctRed2(leftTurnSignal, rightTurnSignal, hazardLights);
					leftTurnSignal=0;
					msg[1] = leftTurnSignal;
					LED_Off(3);
				}
				if(rightTurnSignal == 0){
					clear_box(45, 120, Green);
					correctGreen(lowBeamsState, highBeamsState, breakLights);
					enable_timer(1);
					rightTurnSignal=1;
					msg[2] = rightTurnSignal;
					lock = 0;
					enable_timer(2);
				}else {
					LED_Off(0);
					clear_box(45, 120, Red);
					correctRed(lowBeamsState, highBeamsState, breakLights);
					correctRed2(leftTurnSignal, rightTurnSignal, hazardLights);
					rightTurnSignal=0;
					msg[2] = rightTurnSignal;
					disable_timer(2);
					reset_timer(2);
					disable_timer(0);
					reset_timer(0);
					disable_timer(1);
					reset_timer(1);
				}
				break;
			default:
				break;
		}
	}
	else{
			right=0;
	}
	
	
	/* button management */
	if(k1!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* low beams KEY1 pressed */
			k1++;				
			switch(k1){
				case 2:				/* pay attention here: please see slides 19_ to understand value 2 */
				if (highBeamsState == 1) {
					clear_box(45, 160, Red);
					correctRed(lowBeamsState, highBeamsState, breakLights);
					highBeamsState = 0;
					msg[4] = highBeamsState;
					LED_Off(6);
				}	
				if (lowBeamsState == 0){
						LED_On(7);
						LED_On(5);
						lowBeamsState = 1;
						msg[3] = lowBeamsState;
						lock = 0;
						clear_box(45, 140, Green);
						correctGreen(lowBeamsState, highBeamsState, breakLights);
						correctGreen2(leftTurnSignal, rightTurnSignal, hazardLights);
					} else {
						LED_Off(7);
						LED_Off(5);
						lowBeamsState = 0;
						msg[3] = lowBeamsState;
						clear_box(45, 140, Red);
						correctRed(lowBeamsState, highBeamsState, breakLights);
						correctRed2(leftTurnSignal, rightTurnSignal, hazardLights);
					}
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			k1=0;	
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	
	if(k2!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* high beams KEY2 pressed */
			k2++;				
			switch(k2){
				case 2:				/* pay attention here: please see slides 19_ to understand value 2 */
				if (lowBeamsState == 1) {
					lowBeamsState = 0;
					msg[3] = lowBeamsState;
					LED_Off(7);
					clear_box(45, 140, Red);
					correctRed(lowBeamsState, highBeamsState, breakLights);
					correctRed2(leftTurnSignal, rightTurnSignal, hazardLights);
				}	
				if (highBeamsState == 0){
						LED_On(6);
						LED_On(5);
						highBeamsState = 1;
						msg[4] = highBeamsState;
						lock = 0;
						GUI_Text(0,0, (unsigned char *)msg, White, Blue);
						clear_box(45, 160, Green);
						correctGreen(lowBeamsState, highBeamsState, breakLights);
						correctGreen2(leftTurnSignal, rightTurnSignal, hazardLights);
					} else {
						LED_Off(6);
						LED_Off(5);
						highBeamsState = 0;
						msg[4] = highBeamsState;
						clear_box(45, 160, Red);
						correctRed(lowBeamsState, highBeamsState, breakLights);
						correctRed2(leftTurnSignal, rightTurnSignal, hazardLights);
					}
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			k2=0;
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	}
	
	/*if(k3!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* breaks light INT0 pressed *
			k3++;				
			switch(k3){
				case 2:				/* pay attention here: please see slides 19_ to understand value 2 *
					LED_On(4);
					breakLights=1;
					msg[5] = breakLights;
					lock = 0;
					clear_box(45, 180, Green);
					correctGreen(lowBeamsState, highBeamsState, breakLights);
					correctGreen2(leftTurnSignal, rightTurnSignal, hazardLights);
					k2++;
					break;
				default:
					break;
			}
		}
		else {	/* button released *
			if (k3 > 2){
				LED_Off(4);
				breakLights=0;
				msg[5] = breakLights;
				clear_box(45, 180, Red);
				correctRed(lowBeamsState, highBeamsState, breakLights);
				correctRed2(leftTurnSignal, rightTurnSignal, hazardLights);
				k3 = 0;
			}
			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection *
		}
	} */
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
	if (k1 == 0 && k2 == 0 && k3 == 0 && left == 0 && right == 0 && select == 0) lock = 0;
	if (k1 > 1 || k2 > 1 || k3 > 1 || left > 0 || right > 0 || select > 0 ){
			if (!lock) {
				//DES3((unsigned char*)msg, key, ENCRYPT);
				//AES_state(&ctx, (unsigned char*)msg, &lock);
				
				msg[15] = 0xa; // verifing code
				AES(&ctx,  (unsigned char *) msg, 16);
				
				int r = hCAN_sendMessage(1, (char *) msg, 16);
				if(r != hCAN_SUCCESS){
					if(r == hCAN_ERR_BUS_ERROR){
						GUI_Text(0, 0, (uint8_t*) "Disconnected!", White, Yellow);
					}
				}
				
				AES(&dec_ctx, (unsigned char *) msg, 16);
				lock = 1;
			}
	}
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

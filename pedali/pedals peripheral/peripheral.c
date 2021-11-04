/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           peripheral.c
** Last modified Date:  2021-05-22
** Last Version:        V1.00
** Descriptions:        functions to print on display info about steering brake and gas
** Correlated files:    peripheral.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/

#include "lpc17xx.h"
#include "peripheral.h"
#include "./GLCD/GLCD.h"
#include "highcan.h"
#include "../security/security.h"
#include "../led/led.h"

extern struct AES_ctx ctx;
uint16_t level;
unsigned char message[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int pressed = 0;

/******************************************************************************
** Function name:		steeringHandler
**
** Descriptions:		handler of the steering peripheral
**
** parameters:			current ADC_value
** Returned value:		None -> print on display
**
******************************************************************************/

void steeringHandler(unsigned short ADC_value){
		
}

void brakeHandler(unsigned short ADC_value) {

	uint16_t finalHeight = (ADC_value * 260) / 0xFFF;
	message[0] = finalHeight & 0xFF;
	message[1] = finalHeight & 0xFF00;
	AES(&ctx, message);
	LED_Off(0);
	while(hCAN_sendMessage(1, (char *) message, 16) != hCAN_SUCCESS);
	//draw rectangle
	fillRectangle(125, 280, 230, 280 - finalHeight, brake_color, bgnd);	
	
}

void gasHandler(unsigned short ADC_value){
	
	uint16_t finalHeight = (ADC_value * 260) / 0xFFF;
	message[2] = finalHeight & 0xFF;
	message[3] = finalHeight & 0xFF00;
	AES(&ctx, message);
	LED_Off(0);
	while(hCAN_sendMessage(1, (char *) message, 16)!=hCAN_SUCCESS);
	//draw rectangle
	fillRectangle(10, 280, 115, 280 - finalHeight, gas_color, bgnd);
	
}



/*****************************************************************************
**                            End Of File
******************************************************************************/

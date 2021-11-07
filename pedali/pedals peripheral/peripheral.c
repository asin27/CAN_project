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
#include "CAN/headers/highcan.h"
#include <security/security.h>
//#include "../led/led.h"

extern struct AES_ctx ctx;
extern struct AES_ctx ctx_dec;
uint16_t level;
unsigned char message[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10};
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

	uint16_t finalHeight = (ADC_value * 150) / 0xFFF;
	if(finalHeight > 150) finalHeight = 150;
	message[0] = finalHeight & 0xFF;
	message[1] = finalHeight & 0xFF00;
	
	//GUI_Text(0,0, (uint8_t *)message, Black, bgnd);
	AES(&ctx, message, 16);
	//LED_Off(0);
	while(hCAN_sendMessage(1, (char *) message, 16) != hCAN_SUCCESS);
	AES(&ctx_dec, message, 16);
	//draw rectangle
	finalHeight = (finalHeight * 260) / 150;
	fillRectangle(125, 280, 230, 280 - finalHeight, brake_color, bgnd);	
	
}

void gasHandler(unsigned short ADC_value){
	
	uint16_t finalHeight = (ADC_value * 150) / 0xFFF;
	if (finalHeight > 150) finalHeight = 150;
	message[2] = finalHeight & 0xFF;
	message[3] = finalHeight & 0xFF00;
	AES(&ctx, message, 16);
	//LED_Off(0);
	int r = hCAN_sendMessage(1, (char *) message, 16);
	while(r!=hCAN_SUCCESS){
		hCAN_sendMessage(1, (char *) message, 16);
	}
	AES(&ctx_dec, message, 16);
	//draw rectangle
	finalHeight = (finalHeight * 260) / 150;
	fillRectangle(10, 280, 115, 280 - finalHeight, gas_color, bgnd);
	
}



/*****************************************************************************
**                            End Of File
******************************************************************************/

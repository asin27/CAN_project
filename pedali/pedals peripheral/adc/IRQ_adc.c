/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "adc.h"
#include "../led/led.h"
#include "../GLCD/GLCD.h"
#include "../peripheral.h"
#include <stdio.h>

#define N 23

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

unsigned short AD_current;   
unsigned short AD_last = 0xFF;     /* Last converted value               */

// flags

int steering = 0; 
int isOn = 0;
int gas = 1;
int breaks = 0;

void ADC_IRQHandler(void) {
  	
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */  
	if(AD_current <= N) AD_current = 0;
	if(AD_current >= 0xFFF-N) AD_current = 0xFFF;
	//if (AD_current > AD_last + (2*N)) return; 
	void (*f)(unsigned short);
	
	if(steering != 0){
		// LEDs switched on and off according to the potentiometer - on right and on left depending on the turning wheel
		if(AD_current != AD_last){
			LED_Off(7 - ((AD_last*7)/0xFFF));	  // ad_last : AD_max = x : 7 		LED_Off((AD_last*7/0xFFF));	
			LED_On (7 - ((AD_current*7)/0xFFF));	// ad_current : AD_max = x : 7 		LED_On((AD_current*7/0xFFF));	
		} 
		AD_last = AD_current;
		return;
	}
	
	if(breaks != 0) f = brakeHandler;
	else if(gas != 0) f = gasHandler;
	
	if(isOn && (AD_current < AD_last - N || AD_current > AD_last + N /*|| AD_current == 0*/)){
   		f(AD_current);
	} 
	
	AD_last = AD_current;

	#ifdef DEBUG
	char string[200];
	sprintf(string, "%d %d %d - %06x", steering, breaks, 
		gas, AD_current);
	GUI_Text(10, 300, (uint8_t*) string, White, Blue);
	#endif
}

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

#define N 0

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

unsigned short AD_current;   
unsigned short AD_last = 0xFF;     /* Last converted value               */

// flags

int steering = 0; 
int brake = 0; 
int gas = 0;

void ADC_IRQHandler(void) {
  	
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */  
	
	if(steering != 0){
		
		// LEDs switched on and off according to the potentiometer - on right and on left depending on the turning wheel
		if(AD_current != AD_last){
			LED_Off(7 - ((AD_last*7)/0xFFF));	  // ad_last : AD_max = x : 7 		LED_Off((AD_last*7/0xFFF));	
			LED_On (7 - ((AD_current*7)/0xFFF));	// ad_current : AD_max = x : 7 		LED_On((AD_current*7/0xFFF));	
		} 
		
	} else if (brake != 0){
		
		if(AD_current < AD_last + N && AD_current > AD_last - N ){
			// function to manage the brake pedal
   		brakeHandler(AD_current);
		} 
	} else if (gas != 0){
	
		if(AD_current < AD_last + N && AD_current > AD_last - N){
			// function to manage the gas pedal
			gasHandler(AD_current);
		}
	}
	
	AD_last = AD_current;
  
	
}

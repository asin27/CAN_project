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
#include "../timer/timer.h"
#include "../GLCD/GLCD.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

extern unsigned char AD_current[16];   
unsigned short AD_last = 0xFF;     /* Last converted value               */
char converted[5];

static int i = 0;
void ADC_IRQHandler(void) {
	
  AD_current[i] = ((LPC_ADC->ADGDR>>4) & 0xFF);/* Read Conversion Result             */  
	i++;
	if (i < 16) ADC_start_conversion();
	
}

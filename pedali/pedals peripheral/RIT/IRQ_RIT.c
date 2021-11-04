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
#include "../GLCD/GLCD.h"
#include "../button_EXINT/button.h"
#include "../TouchPanel/TouchPanel.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

int down_INT0=0;
int down_KEY1=0;
int down_KEY2=0;

extern int steering;
extern int brake;
extern int gas;

void RIT_IRQHandler (void)
{						
	/* ADC management */
	ADC_start_conversion();
	
	/* button management */
	
	/* INT0 - sterzo*/
	if(down_INT0!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* INT0 pressed */
			down_INT0++;	
				
			switch(down_INT0){
				case 2:
					steering = 1;
					brake = 0;
					gas = 0;
					break;
				default:
					break;
			}
		} else {	/* button released */
			down_INT0=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
		
		/* KEY1 */
	if(down_KEY1!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */
			down_KEY1++;		
				
			switch(down_KEY1){
				case 2:
					steering = 0;
					brake = 0;
					gas = 1;
					break;
				default:
					break;
			}
		} else {	/* button released */
			down_KEY1=0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
		
		/* KEY2 */
	if(down_KEY2!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* KEY2 pressed */
			down_KEY2++;			

			switch(down_KEY2){
				case 2:
					steering = 0;
					brake = 1;
					gas = 0;
					break;
				default:
					break;
			}
		} else {	/* button released */
			down_KEY2=0;			
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	}	
			
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
}

/******************************************************************************
**                            End Of File
******************************************************************************/

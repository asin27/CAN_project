/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "timer.h"
#include "../led/led.h"
#include "../GLCD/GLCD.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

extern uint8_t fLevel[4];
extern int selected;

void TIMER0_IRQHandler (void)
{
	drawLineOfSquares(12 - fLevel[selected], Grey);
	if(fLevel[selected]<12){
		fLevel[selected]++;
		LED_Out(fLevel[selected]);
		
	}
	else{
		disable_timer(0);
		//send data through CAN
	}
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	drawLineOfSquares(12 - fLevel[selected], White);
	if(fLevel[selected]>0){
		fLevel[selected]--;
		LED_Out(fLevel[selected]);
		
	}
	else{
		disable_timer(1);
		//send data through CAN
	}
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}
/******************************************************************************
**                            End Of File
******************************************************************************/

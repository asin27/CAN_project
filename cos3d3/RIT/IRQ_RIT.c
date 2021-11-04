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
#include "../timer/timer.h"
#include "../GLCD/GLCD.h"
#include "../trng/adc.h"
#include <highcan.h>
#include "../security/security.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

extern struct AES_ctx keyctx;
uint8_t cnt = 0;
unsigned char message[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int pressed = 0;
extern unsigned char key[16];
unsigned char *k;


extern struct AES_ctx ctx_enc, ctx_dec, break_ctx;

void RIT_IRQHandler (void)
{					
	static int select=0;
	uint8_t i = 0;
	//static int position=0;	
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick Select up */
		pressed = 1;
		select++;
		switch(select){
				case 1:
					k = ADC_generate_random();
					for (i = 0; i < 16; i++){
						key[i] = k[i];
					}
					AES(&keyctx, (uint8_t *)key);
					while((hCAN_sendMessage(1, (char *) key, 16))!=hCAN_SUCCESS);
				break;
			}
	}
	
	else{
			if (pressed) {
			cnt = 0;
			select=0;
			}
	}
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

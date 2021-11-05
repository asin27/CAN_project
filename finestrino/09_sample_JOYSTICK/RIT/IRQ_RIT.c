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
#include "../GLCD/GLCD.h"
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

volatile int down=0;
extern uint8_t fLevel[4];
uint8_t cnt = 0;
unsigned char message[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int pressed = 0;
int selected = 0;

extern struct AES_ctx ctx_enc, ctx_dec, break_ctx;

void RIT_IRQHandler (void)
{					
	static int select=0;
	//static int position=0;	
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick Select up */
		pressed = 1;
		select++;
		if(cnt>1 && fLevel[selected] < 12)
			enable_timer(0);
		else{
			switch(select){
				case 1:
					if(!cnt){
						disable_timer(0);
						disable_timer(1);
						if(fLevel[selected]<12){
							drawLineOfSquares(12 - fLevel[selected], Grey);
							fLevel[selected]++;
							LED_Out(fLevel[selected]);
						}
					}
						break;
				case 10:
					drawLineOfSquares(12 - fLevel[selected], Grey);
					if(fLevel[selected]<12)
						fLevel[selected]++;
					LED_Out(fLevel[selected]);
					cnt++;
					select = 0;
					break;
			}
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){
		//select
		down++;
		disable_timer(0);  
		disable_timer(1);
		uint8_t f[2] = {0, 0};
		if(down==1){
			selected = (selected +1)%4;
			f[0] = selected  + 48;
			for(int i = 0; i<13; i++)
				drawLineOfSquares(i, White);
			for(int i=12 - fLevel[selected]; i<13; i++)
				drawLineOfSquares(i, Grey);
			
			GUI_Text(80, 30, (uint8_t *)"Finestrino ", Black, White);
			GUI_Text(170, 30, (uint8_t *) f, Black, White);
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick Select down */
		pressed = 1;
		select++;
		if(cnt>1 && fLevel >0)
			enable_timer(1);
		else{
			switch(select){
				case 1:
					if(!cnt){
						disable_timer(0);
						disable_timer(1);
						drawLineOfSquares(12 - fLevel[selected], White);
						if(fLevel[selected]>0){
							fLevel[selected]--;
							
						}
					}
						break;
				case 10:
					drawLineOfSquares(12 - fLevel[selected], White);
					if(fLevel[selected]>0)
						fLevel[selected]--;

					cnt++;
					select = 0;
					break;
			}
		}
	}
	else{
			//send data through CAN
			down = 0;
			if(pressed){
				message[0] = fLevel[selected];
				message[1] = selected;
				AES(&ctx_enc,message);
				LED_Off(0);
				while(hCAN_sendMessage(1, (char *) message, 16) != hCAN_SUCCESS);
				pressed = 0;
				for(int i = 0; i<8; i++)
					message[i] = 0;
			}
			cnt = 0;
			select=0;
	}
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

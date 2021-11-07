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
#include <security/security.h>
#include "CAN/headers/highcan.h"


/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern struct AES_ctx ctx_dec;
extern struct AES_ctx newParam_dec;
extern struct AES_ctx ctx;
extern struct AES_ctx ack;

extern uint8_t key_aes[16];
extern uint8_t iv[16];
#define canBus 1
void TIMER0_IRQHandler (void)
{
	hCAN_recMessage[canBus-1][hCAN_lenght[canBus-1]] = 0;
		
		
		if( hCAN_recID[canBus-1] == 0x4 ){
			//GUI_Text(10, 120, (uint8_t*) "livello finestrino: ", Black, Yellow);
			extern unsigned char keyDgst[32];
			extern unsigned char ivDgst[32];
			extern unsigned char newKey[16];
			extern unsigned char newIv[16];
			char res[32] = {0};
			int okKey = 0, okIv = 0; 
	
			for(int i=0;i<32;i++)
				keyDgst[i] = hCAN_recMessage[canBus-1][i];

			
			for(int i=32;i<48;i++)
				newKey[i-32] = hCAN_recMessage[canBus-1][i];
			
			for(int i=48;i<80;i++)
				ivDgst[i-48] = hCAN_recMessage[canBus-1][i];
			
			for(int i=80;i<96;i++)
				newIv[i-80] = hCAN_recMessage[canBus-1][i];
			
			if (!verify_digest(newKey, key_aes, keyDgst)){
					 GUI_Text(10, 120, (uint8_t*) "Errore verifica KEY", Black, Yellow);
			} else {
					AES(&newParam_dec, newKey, 16);
					okKey = res[0] = 1;
			}
			if (!verify_digest(newIv, key_aes, ivDgst)){
					 GUI_Text(20, 120, (uint8_t*) "Errore verifica IV", Black, Yellow);
			} else {
					AES(&newParam_dec, newIv, 16);
					okIv = res[1] = 1;
			}
			if (okKey && okIv){
				for (int i = 0; i < 16; i++){
						key_aes[i] = newKey[i];
						iv[i] = newIv[i];
				}
					ctx_dec = AES_init(key_aes, iv);
					ctx = AES_init(key_aes, iv);
				ack = AES_init(key_aes, iv);
					newParam_dec = AES_init(key_aes, iv);
					
					AES(&ack, (uint8_t *)res, 32);
					
					int r = hCAN_sendMessage(1, (char *) res, 32);
					while(r!=hCAN_SUCCESS){
						r = hCAN_sendMessage(1, (char *) res, 32);
					}
			} else {
					AES(&ack, (uint8_t *)res, 32);
					while(hCAN_sendMessage(1, (char *) res, 32)!=hCAN_SUCCESS);
			}
		
		
			for(int i=0; i<100; i++);
			
			
			//GUI_Text(10, 140, (uint8_t*) finestrino, Black, Yellow);
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
	
	LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER2_IRQHandler (void)
{	
	
	
	LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER3_IRQHandler (void)
{
	
	
	LPC_TIM3->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
**                            End Of File
******************************************************************************/

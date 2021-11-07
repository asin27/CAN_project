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

#include "lpc17xx.h"
#include "./highcan.h"
#include <security/security.h>
#include "../GLCD/GLCD.h"
#include "../drawing/draw.h"
#include <keep_alive/keep_alive.h>

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

extern uint8_t position;
extern uint8_t hazardLights;

#define N 10

/* k=1/f'*f/n  k=f/(f'*n) k=25MHz/(f'*45) */

//const int freqs[8]={4240,3779,3367,3175,2834,2525,2249,2120};
/* 
131Hz		k=4240 C3
147Hz		k=3779
165Hz		k=3367
175Hz		k=3175
196Hz		k=2834		
220Hz		k=2525
247Hz		k=2249
262Hz		k=2120 C4
*/

const int freqs[8]={2120,1890,1684,1592,1417,1263,1125,1062};
/*
262Hz	k=2120		c4
294Hz	k=1890		
330Hz	k=1684		
349Hz	k=1592		
392Hz	k=1417		
440Hz	k=1263		
494Hz	k=1125		
523Hz	k=1062		c5

*/

uint16_t SinTable[45] =                                       /* ÕýÏÒ±í                       */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

int seq4beats[3][N]={{0,8,0,8,0,8,0,8,0,8},
										 {0,8,3,8,6,8,5,8,7,8},
										 {0,0,0,8,8,0,0,0,8,8}
};
/* contains sequence of notes from c4 to C5 */
/* position of c4 = 0 to C5 - 7 in the freq const vect ADC */ 
/* position 8 is an artifact to include a pause of 1/4 beat */	
	
int play = 0;
/* current status of sound (on/off) */

int blink_mask = 0xFF;

extern int check;

void TIMER0_IRQHandler (void)
{
	static int ticks=0;
	/* DAC management */	
	LPC_DAC->DACR = (SinTable[ticks]/3)<<6;
	ticks++;
	if(ticks==45) ticks=0;
	
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
int song=0;
int note=0;
void TIMER1_IRQHandler (void)
{  	
	if(note==N){ 
		note=0;
		//song++;
		//if(song!=3){
		//	disable_timer(1);
		//	disable_timer(0);
		//}
		//else{
		//	song--;
		//}
	}
	else
		//play=seq4beats[song][note++];
	if (play == 0){
		play = 8;
	} else {
		play = 0;
	}
	
	if(play!=8){
		disable_timer(0);
		reset_timer(0);
		init_timer(0,freqs[play]);
		enable_timer(0);
	}
	else{
		disable_timer(0);
	}
	
	LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER2_IRQHandler (void)
{
	static uint8_t state = 0;
	if (state == 0){
		state = 1;
		if (hazardLights == 1){
			LED_On(0);
			LED_On(1);
			LED_On(2);
			LED_On(3);;
		} else {
			LED_On(position);
		}
	}
	else
	{
		state = 0;
		if (hazardLights == 1){
			LED_Off(0);
			LED_Off(1);
			LED_Off(2);
			LED_Off(3);
		} else {
			LED_Off(position);
		}
	}
		
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}



extern char msg[16];
extern struct AES_ctx break_dec_ctx;
extern struct AES_ctx dec_ctx;
extern struct AES_ctx ctx;
extern struct AES_ctx newParam_dec;
extern struct AES_ctx ack;

extern unsigned char keyDgst[32];
extern unsigned char ivDgst[32];
extern unsigned char newKey[16];
extern unsigned char newIv[16];
char res[32] = {0};


extern unsigned char keyAES[16];
extern uint8_t ivAES[16];

// Received messages 
void TIMER3_IRQHandler (void)
{
	int canBus = 1; // only bus 1 is used
	char b[16] = {0};
	int okKey = 0, okIv = 0;
	
	hCAN_recMessage[canBus-1][hCAN_lenght[canBus-1]] = 0;
		
		if( hCAN_lenght[canBus-1] == 32 ) return;
		
		// check for keep alive message
		if(checkMsg(hCAN_recMessage[canBus-1], hCAN_recID[canBus-1]))
			return;
		if( hCAN_recID[canBus-1] == 0x4 ){
			//GUI_Text(10, 120, (uint8_t*) "livello finestrino: ", Black, Yellow);
			
			for(int i=0;i<32;i++)
				keyDgst[i] = hCAN_recMessage[canBus-1][i];

			
			for(int i=32;i<48;i++)
				newKey[i-32] = hCAN_recMessage[canBus-1][i];
			
			for(int i=48;i<80;i++)
				ivDgst[i-48] = hCAN_recMessage[canBus-1][i];
			
			for(int i=80;i<96;i++)
				newIv[i-80] = hCAN_recMessage[canBus-1][i];
			
			if (!verify_digest(newKey, keyAES, keyDgst)){
					 GUI_Text(10, 120, (uint8_t*) "Errore verifica KEY", Black, Yellow);
			} else {
					AES(&newParam_dec, newKey, 16);
					okKey = res[0] = 1;
			}
			if (!verify_digest(newIv, keyAES, ivDgst)){
					 GUI_Text(20, 120, (uint8_t*) "Errore verifica IV", Black, Yellow);
			} else {
					AES(&newParam_dec, newIv, 16);
					okIv = res[1] = 1;
			}
			if (okKey && okIv){
				for(int i =0; i<16; i++){
						keyAES[i] = newKey[i];
						ivAES[i] = newIv[i];
					}
				ctx = AES_init(newKey, newIv);
				dec_ctx = AES_init(newKey, newIv);
				newParam_dec = AES_init(newKey, newIv);
				ack = AES_init(newKey, newIv);
				break_dec_ctx = AES_init(newKey, newIv);
				AES(&ack, (uint8_t *)res, 32);
				int r;
				r = hCAN_sendMessage(1, (char *) res, 32);
				while(r!=hCAN_SUCCESS)
					r=hCAN_sendMessage(1, (char *) res, 32);
			}
					
			for(int i=0; i<100; i++);
			
			
			//GUI_Text(10, 140, (uint8_t*) finestrino, Black, Yellow);
		}
		// otherwise other messages
		if( hCAN_recID[canBus-1] == 0x3 ){
			
			for(int i=0; i<16;i++)
				b[i] = hCAN_recMessage[canBus-1][i];
			
			//DES3((unsigned char*) finestrino, key, DECRYPT);
			AES(&break_dec_ctx, (unsigned char*) b, 16);
			
			if( b[15] != 0xa ){
				GUI_Text(0, 0, (uint8_t *) "Crypto Trubles", Black, Yellow);
			}
			else{
				if (b[0] > 0) {
					msg[5] = 1;
					clear_box(45, 180, Green);
				} else {
					msg[5] = 0;
					clear_box(45, 180, Red);
				}
		}
		}
	
  LPC_TIM3->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

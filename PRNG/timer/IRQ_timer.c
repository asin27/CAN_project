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
#include "../trng/adc.h"
#include "timer.h"
#include "../GLCD/GLCD.h"
#include <stdio.h>
#include <security/security.h>
#include "highcan.h"


/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

int blink_mask = 0xFF;

extern int check;
extern unsigned char AD_current;
extern struct AES_ctx ctx, decK;
extern struct AES_ctx ack_ctx[4]; //declared in main 
extern uint8_t oldKey[16];
extern uint8_t oldIv[16];
extern uint8_t eKey[16];
extern uint8_t iv[16];
extern uint8_t sCode;
extern uint8_t hK[32];
extern uint8_t hIv[32];
extern uint8_t finalMessage[96];
extern int good, bad;
int generated = 0;

int ready = 0;

void keyGeneration(){
	int i;
	char s1[16];
	char s2[16];

	for(int i=0; i<16; i++){
		ADC_start_conversion();
		while(!ready);
		eKey[i] = AD_current;
		ready = 0;
	}
	for(int i=0; i<16; i++){
		ADC_start_conversion();
		while(!ready);
		iv[i] = AD_current;
		ready = 0;
	}
	ADC_start_conversion();
	while(!ready);
	sCode = AD_current;
	ready = 0;
	
	GUI_Text(10, 20, (unsigned char*)"New key, iv and code generated: ", White, Blue);

	for(i=0; i<16; i++){
		sprintf(s1,"%02x " , eKey[i]);
		sprintf(s2,"%02x ", iv[i]);
		GUI_Text(10 + i*16, 40, (unsigned char*)s1, White, Blue);
		GUI_Text(10 + i*16, 60, (unsigned char*)s2, White, Blue);
	}
}






void prepareMessage(){
	for(int i=0; i<32; i++)
		finalMessage[i] = hK[i];
	for(int i=32; i<48; i++)
		finalMessage[i] = eKey[i-32];
	for(int i=48; i<80; i++)
		finalMessage[i] = hIv[i-48];
	for(int i=80; i<96; i++)
		finalMessage[i] = iv[i-80];
}


void TIMER0_IRQHandler (void)
{
	do{
	good = 0;
	bad = 0;
	keyGeneration();
	
	//prepare ack_ctx(s)
	for(int i = 0; i<4; i++)
		ack_ctx[i] = AES_init(eKey, iv);
		
	//auth then enc
	AES(&ctx, eKey, 16);
	AES(&ctx, iv, 16);
	digest(eKey, oldKey, hK);
	digest(iv, oldKey, hIv); 
	//do handshake here 
	//send digests then keys
	
	prepareMessage();
	generated = 1;
		
	while(hCAN_sendMessage(1, (char *) finalMessage, 96)!= hCAN_SUCCESS); //hK | k | hIv | iv

	while(good < 2 && bad ==0);
		
	generated = 0;
		
	}while(bad != 0);
	
	//updating keys
	AES(&decK, eKey, 16);
	AES(&decK, iv, 16);
	for(int i = 0; i<16; i++){
		oldKey[i] = eKey[i];
		oldIv[i] = iv[i];
	}
	decK = AES_init(oldKey, oldIv);
	ctx = AES_init(oldKey, oldIv);
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER1_IRQHandler (void)
{  	
	
	LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER3_IRQHandler (void)
{  	
	int canBus = 1; 
	hCAN_recMessage[canBus-1][hCAN_lenght[canBus-1]] = 0;
	if(hCAN_lenght[canBus-1] == 32 && generated){
		AES(&ack_ctx[hCAN_recID[canBus-1] == 0xa ? 3 : hCAN_recID[canBus-1] -1], (uint8_t *) hCAN_recMessage[canBus-1], 32);
		if(hCAN_recMessage[canBus-1][0] == 1 && hCAN_recMessage[canBus-1][1] == 1)
			good++;
		else 
			bad++;
	}
	LPC_TIM3->IR = 1;			/* clear interrupt flag */
  return;
}


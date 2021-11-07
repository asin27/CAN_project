/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "cos3d.h"
#include "math.h"
#include "highcan.h"
#include "security/security.h"
#include "RIT/RIT.h"
#include "trng/adc.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

/****************************************/

/****************************************/

uint8_t key[16] = {'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'};
uint8_t iv[16] = {'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B'};

struct AES_ctx ctx_dec[4];
struct AES_ctx keyctx;

unsigned char keyDgst[32];
unsigned char ivDgst[32];
unsigned char newKey[16];
unsigned char newIv[16];
struct AES_ctx newParam_dec;
struct AES_ctx ctx;
struct AES_ctx ack;

int main(void)
{
	SystemInit();  												/* System Initialization (i.e., PLL)  */
	for(int i=0; i<32; i++) NVIC_SetPriority(i, 5);
	init_RIT(0x004c4b40);
	enable_RIT();
	ADC_init();
	hCAN_init(1, CAN_5Kbps);
	hCAN_setID(0xa);
	LCD_Initialization();
	ctx_dec[0] = AES_init(key, iv);
	ctx_dec[1] = AES_init(key, iv);
	ctx_dec[2] = AES_init(key, iv);
	ctx_dec[3] = AES_init(key, iv);
	newParam_dec = AES_init(key, iv);
	ctx = AES_init(key, iv);
	ack = AES_init(key, iv);
  keyctx = AES_init(key, iv);
	
	
//	TP_Init();
//	TouchPanel_Calibrate();
	
	LCD_Clear(White);
	
	//init_timer(0, 0x1312D0 ); 			//20fps			
	init_timer(0, 0x2625a0); 					//10fps
	//init_timer(0, 0x196e6a); //15
	//init_timer(0, 0x0F4240 );				//25fps
	
	enable_timer(0);
	
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

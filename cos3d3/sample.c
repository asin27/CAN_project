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
#include "security.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

/****************************************/

/****************************************/

uint8_t key[16] = {'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'};
uint8_t iv[16] = {'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B'};

struct AES_ctx ctx_dec[2];
int main(void)
{
	SystemInit();  												/* System Initialization (i.e., PLL)  */
	hCAN_init(1, CAN_1Mbps);
	hCAN_setID(0xa);

	LCD_Initialization();
	ctx_dec[0] = AES_init(key, iv);
	ctx_dec[1] = AES_init(key, iv);
  
//	TP_Init();
//	TouchPanel_Calibrate();
	
	LCD_Clear(White);
	
	init_timer(0, 0x1312D0 ); 			//20fps			
	//init_timer(0, 0x4C4B40 );				//5fps
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

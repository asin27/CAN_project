#include "lpc17xx.h"
#include <CAN/headers/highcan.h>
#include "GLCD/GLCD.h"
#include <security/security.h>
#include "./timer/timer.h"

//static unsigned char keyDgst[32] = {0};
//static unsigned char ivDgst[32] = {0};
//static unsigned char newKey[16] = {0};
//static unsigned char newIv[16] = {0};



void IRQ_CAN(int canBus);



void CAN_IRQHandler (void)
{
	int32_t icr = LPC_CAN1->ICR; // clear the interrupt
	
//	if ( (icr & CAN_ICR_EPI) != 0 )
//		GUI_Text(0, 20*counter++, (uint8_t*) "EPI ERROR!", Black, Yellow);
	
	int canBus = 0;
	if( (LPC_CAN1->GSR & 1) == 1) IRQ_CAN(1);
	//else if( (LPC_CAN2->GSR & 1) == 1) canBus = 2;
	
	if(icr & CAN_ICR_BEI){
		hCAN_ActiveError = hCAN_ERR_BUS_ERROR;
	}
	if(icr & CAN_ICR_ALI){
		hCAN_ActiveError = hCAN_ERR_COLLISION;
	}
	
}

void IRQ_CAN(int canBus){
	
	
	
	if(hCAN_receiveMessage(canBus) == hCAN_SUCCESS && hCAN_recDone[canBus-1]){
		
		enable_timer(0);
		
		
	}
	
	
}

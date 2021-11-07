#include "lpc17xx.h"
#include "./highcan.h"
#include <security/security.h>
#include "../GLCD/GLCD.h"
#include "../drawing/draw.h"
#include <keep_alive/keep_alive.h>
#include "timer/timer.h"

extern uint8_t lock;

void IRQ_CAN_RECEIVE(int canBus);
void IRQ_CAN_BEI_HANDLER(LPC_CAN_TypeDef * can);
void IRQ_CAN_ALI_HANDLER(LPC_CAN_TypeDef * can);

void CAN_IRQHandler (void)
{
	int32_t icr = LPC_CAN1->ICR; // clear the interrupt
	LPC_CAN_TypeDef *can = LPC_CAN1;
	
	for(int i=1; i<=2; i++){
		if(i==1) can = LPC_CAN1;
		else if(i==2) can = LPC_CAN2;
		
		// Receive interrupt
		if( (icr & CAN_ICR_RI) != 0 ){			
			if( (can->GSR & 1) == 1) IRQ_CAN_RECEIVE(i);
		}
		
		// Bus Error Interrupt
		if( (icr & CAN_ICR_BEI) != 0)
			IRQ_CAN_BEI_HANDLER(can);
		
		// Arbitration lost interrupt
		if( (icr & CAN_ICR_ALI) != 0 )
			IRQ_CAN_ALI_HANDLER(can);
		
	}
}

void IRQ_CAN_RECEIVE(int canBus){
	
	if(hCAN_receiveMessage(canBus) == hCAN_SUCCESS && hCAN_recDone[canBus-1]){
		enable_timer(3);
	}
	
}

void IRQ_CAN_BEI_HANDLER(LPC_CAN_TypeDef* can){
	CAN_resetTXERR(can);
	CAN_abortTransmission(can);
	hCAN_ActiveError = hCAN_ERR_BUS_ERROR;
}

void IRQ_CAN_ALI_HANDLER(LPC_CAN_TypeDef* can){
	CAN_resetTXERR(can);
	CAN_abortTransmission(can);
	hCAN_ActiveError = hCAN_ERR_COLLISION;
}
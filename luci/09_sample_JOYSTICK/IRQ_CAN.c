#include "lpc17xx.h"
#include "./highcan.h"
#include <security/security.h>
#include "../GLCD/GLCD.h"
#include "../drawing/draw.h"
#include <keep_alive/keep_alive.h>

extern char msg[16];
extern struct AES_ctx break_dec_ctx;
extern struct AES_ctx dec_ctx;
extern struct AES_ctx ctx;

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
		
		int canBusReceive = 0;
		if( (can->GSR & 1) == 1) IRQ_CAN_RECEIVE(i);
		
		// Bus Error Interrupt
		if( (icr & CAN_ICR_BEI) != 0)
			IRQ_CAN_BEI_HANDLER(can);
		
		// Arbitration lost interrupt
		if( (icr & CAN_ICR_ALI) != 0 )
			IRQ_CAN_ALI_HANDLER(can);
		
	}
}

void IRQ_CAN_RECEIVE(int canBus){
	char b[16] = {0};
	
	if(hCAN_receiveMessage(canBus) == hCAN_SUCCESS && hCAN_recDone[canBus-1]){
		hCAN_recMessage[canBus-1][hCAN_lenght[canBus-1]] = 0;
		
		// check for keep alive message
		if(checkMsg(hCAN_recMessage[canBus-1], hCAN_recID[canBus-1]))
			return;
		
		// otherwise other messages
		if( hCAN_recID[canBus-1] == 0x3 ){
			
			for(int i=0;i<16;i++)
				b[i] = hCAN_recMessage[canBus-1][i];
			
			//DES3((unsigned char*) finestrino, key, DECRYPT);
			AES(&break_dec_ctx, (unsigned char*) b, 16);
			
			if( b[15] != 0xa ){
				GUI_Text(0, 0, (uint8_t *) "Crypto Trubles", Black, Yellow);
			}
			
			if (b[0] > 0) {
				msg[5] = 1;
				clear_box(45, 180, Green);
			} else {
				msg[5] = 0;
				clear_box(45, 180, Red);
			}
			
		}
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
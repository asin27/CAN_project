#include "lpc17xx.h"
#include "./highcan.h"
#include "../security/security.h"
#include "../GLCD/GLCD.h"
#include "../drawing/draw.h"

extern char msg[16];
extern struct AES_ctx break_dec_ctx;
extern struct AES_ctx dec_ctx;
extern struct AES_ctx ctx;

extern uint8_t lock;

void IRQ_CAN(int canBus);


void CAN_IRQHandler (void)
{
	int32_t icr = LPC_CAN1->ICR; // clear the interrupt
	
	
	int canBus = 0;
	if( (LPC_CAN1->GSR & 1) == 1) canBus = 1;
	//else if( (LPC_CAN2->GSR & 1) == 1) canBus = 2;
	
	if(canBus != 0)
		IRQ_CAN(canBus);
	// TODO: clear interrupt request
}

void IRQ_CAN(int canBus){
	char b[16] = {0};
	
	if(hCAN_receiveMessage(canBus) == hCAN_SUCCESS && hCAN_recDone){
		hCAN_recMessage[hCAN_lenght] = 0;
		
		if( hCAN_recID == 0x3 ){
			
			for(int i=0;i<16;i++)
				b[i] = hCAN_recMessage[i];
			
			//DES3((unsigned char*) finestrino, key, DECRYPT);
			AES(&break_dec_ctx, (unsigned char*) b);
			
			if (b[0] > 0) {
				msg[5] = 1;
				clear_box(45, 180, Green);
			} else {
				msg[5] = 0;
				clear_box(45, 180, Red);
			}
			AES(&ctx,  (unsigned char *) msg);
			//while(hCAN_sendMessage(1, (char *) msg, 16) != hCAN_SUCCESS);
			AES(&dec_ctx, (unsigned char *) msg);
			
		}
	}
	
	if(hCAN_arbitrationLost(canBus)){
	
	}
	
	if(hCAN_busError(canBus)){
		GUI_Text(0, 0, (uint8_t*)"ERROR!!!!", White, Blue);
	}
	
	
}

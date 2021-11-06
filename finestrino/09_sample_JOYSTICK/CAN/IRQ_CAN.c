#include "lpc17xx.h"
#include "highcan.h"
#include "GLCD/GLCD.h"
#include "security/security.h"

void IRQ_CAN(int canBus);

extern struct AES_ctx ctx_dec_key;
uint8_t new_key[16];

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
	
	if(hCAN_receiveMessage(canBus) == hCAN_SUCCESS && hCAN_recDone){
		hCAN_recMessage[hCAN_lenght] = 0;
		GUI_Text(10, 50, (uint8_t*) "criptato: ", Black, Yellow);
		GUI_Text(10, 70, (uint8_t*) hCAN_recMessage, Black, Yellow);
		
		
		if( hCAN_recID == 0xa ){
			GUI_Text(10, 120, (uint8_t*) "nuova chiave: ", Black, Yellow);
			
			for(int i=0;i<16;i++)
				new_key[i] = hCAN_recMessage[i];
			
			//DES3((unsigned char*) finestrino, key, DECRYPT);
			AES(&ctx_dec_key, (unsigned char*) new_key, 16);
			for(int i=0; i<100; i++);
			
			
			}
			
			GUI_Text(10, 140, (uint8_t*) new_key, Black, Yellow);
		}
		
	
	if(hCAN_arbitrationLost(canBus)){
		GUI_Text(0, 0, (uint8_t*) "ARBITRATION LOST! SOMEONE IS TRASMIITTING", Yellow, Red);
	};
	
}

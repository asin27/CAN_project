#include "lpc17xx.h"
#include "./highcan.h"
#include "../GLCD/GLCD.h"
#include <security.h>
#include "../trng/adc.h"

void IRQ_CAN(int canBus);
extern int good, bad; //declared static in IRQ_timer
extern struct AES_ctx ctx; //declared in main
extern struct AES_ctx ack_ctx[4]; //declared in main 
extern int generated; //declared in IRQ_timer
//extern unsigned char key[3][8];
//extern struct AES_ctx ctx_dec[2];

void CAN_IRQHandler (void)
{
	int32_t icr = LPC_CAN1->ICR; // clear the interrupt
	
//	if( (icr & CAN_ICR_EPI) != 0 )
//		GUI_Text(0, 20*counter++, (uint8_t*) "EPI ERROR!", Black, Yellow);
	
	int canBus = 0;
	if( (LPC_CAN1->GSR & 1) == 1) canBus = 1;
	//else if( (LPC_CAN2->GSR & 1) == 1) canBus = 2;
	
	if(canBus != 0)
		IRQ_CAN(canBus);
	
	
	//uint8_t c[2];
 	//c[0] = ADC_generate_random();
	//[1] = 0;
	//GUI_Text(10, 240, c, Black, Yellow);
}

void IRQ_CAN(int canBus){
	
	unsigned char finestrino[16] = {0};
	
	if(hCAN_receiveMessage(canBus) == hCAN_SUCCESS && hCAN_recDone){
		hCAN_recMessage[hCAN_lenght] = 0;
		if(hCAN_lenght == 32 && generated){
			AES(&ack_ctx[hCAN_recID == 0xa ? 3 : hCAN_recID -1], (uint8_t *) hCAN_recMessage, 32);
			if(hCAN_recMessage[0] == 1 && hCAN_recMessage[1] == 1)
				good++;
			else 
				bad++;
			/*switch(hCAN_recID){
				case 1: //finestrino
					break;
				case 2: //luci
					break;
				case 3: //pedali
					break;
				case 0xa: //dashboard
					break;
			}*/
		}
	}
	
	if(hCAN_arbitrationLost(canBus)){
		GUI_Text(0, 0, (uint8_t*) "ARBITRATION LOST! SOMEONE IS TRASMIITTING", Yellow, Red);
	};
	
}

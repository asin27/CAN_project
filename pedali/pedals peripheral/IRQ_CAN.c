#include "lpc17xx.h"
#include "CAN/headers/highcan.h"
#include "GLCD/GLCD.h"
#include <security/security.h>

//static unsigned char keyDgst[32] = {0};
//static unsigned char ivDgst[32] = {0};
//static unsigned char newKey[16] = {0};
//static unsigned char newIv[16] = {0};
extern struct AES_ctx ctx_dec;
extern struct AES_ctx newParam_dec;
extern struct AES_ctx ctx;
extern struct AES_ctx ack;

extern uint8_t key_aes[16];
extern uint8_t iv[16];


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
	
	extern unsigned char keyDgst[32];
	extern unsigned char ivDgst[32];
	extern unsigned char newKey[16];
	extern unsigned char newIv[16];
	char res[32] = {0};
	int okKey = 0, okIv = 0; 
	
	if(hCAN_receiveMessage(canBus) == hCAN_SUCCESS && hCAN_recDone[canBus-1]){
		hCAN_recMessage[canBus-1][hCAN_lenght[canBus-1]] = 0;
		
		
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
			
			if (!verify_digest(newKey, key_aes, keyDgst)){
					 GUI_Text(10, 120, (uint8_t*) "Errore verifica KEY", Black, Yellow);
			} else {
					AES(&newParam_dec, newKey, 16);
					okKey = res[0] = 1;
			}
			if (!verify_digest(newIv, key_aes, ivDgst)){
					 GUI_Text(20, 120, (uint8_t*) "Errore verifica IV", Black, Yellow);
			} else {
					AES(&newParam_dec, newIv, 16);
					okIv = res[1] = 1;
			}
			if (okKey && okIv){
				for (int i = 0; i < 16; i++){
						key_aes[i] = newKey[i];
						iv[i] = newIv[i];
				}
					ctx_dec = AES_init(key_aes, iv);
					ctx = AES_init(key_aes, iv);
				ack = AES_init(key_aes, iv);
					newParam_dec = AES_init(key_aes, iv);
					
					AES(&ack, (uint8_t *)res, 32);
					
					int r = hCAN_sendMessage(1, (char *) res, 32);
					while(r!=hCAN_SUCCESS){
						r = hCAN_sendMessage(1, (char *) res, 32);
					}
			} else {
					AES(&ack, (uint8_t *)res, 32);
					while(hCAN_sendMessage(1, (char *) res, 32)!=hCAN_SUCCESS);
			}
		
		
			for(int i=0; i<100; i++);
			
			
			//GUI_Text(10, 140, (uint8_t*) finestrino, Black, Yellow);
		}
		
		
		
	}
	
	
}

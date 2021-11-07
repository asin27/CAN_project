#include "lpc17xx.h"
#include "highcan.h"
#include "GLCD/GLCD.h"
#include "security/security.h"

void IRQ_CAN(int canBus);

extern struct AES_ctx ctx_dec_key;
uint8_t new_key[16];
extern uint8_t iv[16];

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
	extern struct AES_ctx ctx_enc;
	extern struct AES_ctx ctx_dec;
	extern struct AES_ctx newParam_dec;
	extern struct AES_ctx ack;
	
	extern unsigned char key_aes[16];
	
	extern unsigned char keyDgst[32];
	extern unsigned char ivDgst[32];
	extern unsigned char newKey[16];
	extern unsigned char newIv[16];
	char res[32] = {0};
	int okKey = 0, okIv = 0;
	
	if(hCAN_receiveMessage(canBus) == hCAN_SUCCESS && hCAN_recDone[canBus-1]){
		hCAN_recMessage[canBus-1][hCAN_lenght[canBus-1]] = 0;
		//GUI_Text(10, 50, (uint8_t*) "criptato: ", Black, Yellow);
		//GUI_Text(10, 70, (uint8_t*) hCAN_recMessage, Black, Yellow);
		
		
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
				for(int i=0; i<16;i++){
					key_aes[i] = newKey[i];
					iv[i] = newIv[i];
				}
					ctx_dec = AES_init(newKey, newIv);
					ctx_enc = AES_init(newKey, newIv);
					newParam_dec = AES_init(newKey, newIv);
					ack = AES_init(newKey, newIv);
					AES(&ack, (uint8_t *)res, 32);
					while(hCAN_sendMessage(1, (char *) res, 32)!=hCAN_SUCCESS);
			} else {
					while(hCAN_sendMessage(1, (char *) res, 32)!=hCAN_SUCCESS);
			}
		
			for(int i=0; i<100; i++);
			
			
			//GUI_Text(10, 140, (uint8_t*) finestrino, Black, Yellow);
		}
		
	}

}

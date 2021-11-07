#include "lpc17xx.h"
#include "highcan.h"
#include "../GLCD/GLCD.h"
#include "security/security.h"
#include "../trng/adc.h"
#include "../cos3d.h"
#include <stdio.h>

void IRQ_CAN(int canBus);

extern struct AES_ctx newParam_dec;
extern struct AES_ctx ctx;
extern struct AES_ctx ack;

extern uint8_t key[16];
extern uint8_t iv[16];

extern struct AES_ctx ctx_dec[4];

void CAN_IRQHandler (void)
{
	int32_t icr = LPC_CAN1->ICR; // clear the interrupt
	
//	if( (icr & CAN_ICR_EPI) != 0 )
//		GUI_Text(0, 20*counter++, (uint8_t*) "EPI ERROR!", Black, Yellow);
	
	int canBus = 0;
	if( (LPC_CAN1->GSR & 1) == 1) canBus = 1;
	else if( (LPC_CAN2->GSR & 1) == 1) canBus = 2;
	
	if(canBus != 0)
		IRQ_CAN(canBus);
	
	
	//uint8_t c[2];
 	//c[0] = ADC_generate_random();
	//[1] = 0;
	//GUI_Text(10, 240, c, Black, Yellow);
}

void IRQ_CAN(int canBus){
	char buf1[20];
	char buf2[20];
	unsigned char finestrino[16] = {0};
	
	extern unsigned char keyDgst[32];
	extern unsigned char ivDgst[32];
	extern unsigned char newKey[16];
	extern unsigned char newIv[16];
	char res[32] = {0};
	int okKey = 0, okIv = 0; 
	
	if(hCAN_receiveMessage(canBus) == hCAN_SUCCESS && hCAN_recDone[canBus -1]){
		hCAN_recMessage[canBus-1][hCAN_lenght[canBus-1]] = 0;
		//GUI_Text(10, 50, (uint8_t*) "criptato: ", Black, Yellow);
		//GUI_Text(10, 70, (uint8_t*) hCAN_recMessage, Black, Yellow);
		
		//finestrino ok
		if( hCAN_recID[canBus-1] == 0x1 ){
			//GUI_Text(10, 120, (uint8_t*) "livello finestrino: ", Black, Yellow);
			
			for(int i=0;i<16;i++)
				finestrino[i] = hCAN_recMessage[canBus-1][i];
			
			//DES3((unsigned char*) finestrino, key, DECRYPT);
			AES(&ctx_dec[hCAN_recID[canBus-1]-1], (unsigned char*) finestrino, 16);
			
			for(int i=0; i<100; i++);
			if(hCAN_recMessage[canBus-1][15] == 0xa) //checking special code
				set_finestrini(finestrino[1], (float) finestrino[0] /12);
			else
				GUI_Text(80, 280, (uint8_t*) "crypto troubles!!!", Black, Yellow);
			//GUI_Text(10, 140, (uint8_t*) finestrino, Black, Yellow);
		}
		
		//luci ok
		if( hCAN_recID[canBus-1] == 0x2 ){
			AES(&ctx_dec[hCAN_recID[canBus-1]-1], (unsigned char*) hCAN_recMessage, 16);
			for(int i=0; i<100; i++);
			//GUI_Text(10, 180, (uint8_t*) "luci: ", Black, Yellow);
			for(int i=0; i<6; i++){
				hCAN_recMessage[canBus-1][i] += '0';
			}
			hCAN_recMessage[canBus-1][6] = 0;
			
			//frecce
			if(hCAN_recMessage[canBus-1][15] == 0xa){ //checking special code
				if(hCAN_recMessage[canBus-1][0] != '0') 
					Set_freccia(HAZARD);
				else if(hCAN_recMessage[canBus-1][1] != '0')
					Set_freccia(FRECCIA_SX);
				else if(hCAN_recMessage[canBus-1][2] != '0')
					Set_freccia(FRECCIA_DX);
				else
					Set_freccia(NESSUNA);
				// luci
				if(hCAN_recMessage[canBus-1][4] != '0')
					Set_fari(ANABBAGLIANTI);
				else if(hCAN_recMessage[canBus-1][3] != '0')
					Set_fari(ABBAGLIANTI);
				else 
					Set_fari(SPENTI);
				
				//freno
				if(hCAN_recMessage[canBus-1][5] != '0')
					Set_freno(INSERITO);
				else 
					Set_freno(NON_INSERITO);
			}
			else 
				GUI_Text(80, 280, (uint8_t*) "crypto troubles!!!", Black, Yellow);
			//GUI_Text(10, 200, (uint8_t*) hCAN_recMessage, Black, Yellow);
		}
		//freno-acceleratore ok
		if( hCAN_recID[canBus-1] == 0x3 ){
			AES(&ctx_dec[hCAN_recID[canBus-1]-1], (unsigned char*) hCAN_recMessage, 16);
			for(int i=0; i<100; i++);
			if(hCAN_recMessage[canBus-1][15] != 0xa){
				GUI_Text(80, 280, (uint8_t*) "crypto troubles!!!", Black, Yellow);
			}
			else{
				sprintf(buf1, "acceleratore %3d", hCAN_recMessage[canBus-1][2]);
				sprintf(buf2, "freno %3d", hCAN_recMessage[canBus-1][0]);
				//GUI_Text(80, 280, (uint8_t*) buf1, Black, Yellow);
				//GUI_Text(80, 260, (uint8_t*) buf2, Black, Yellow);
				
				int newAcc = hCAN_recMessage[canBus-1][2] - hCAN_recMessage[canBus-1][0];
				if(newAcc < 0) newAcc = 0;
				sprintf(buf1, "%3d %3d %3d", hCAN_recMessage[canBus-1][2], hCAN_recMessage[canBus-1][0], newAcc);
				GUI_Text(80, 260, (uint8_t*) buf1, Black, Yellow);
				Set_acceleratore(newAcc);
			}
		}
	
		if( hCAN_recID[canBus-1] == 0x4 ){
			for(int i=0;i<32;i++)
				keyDgst[i] = hCAN_recMessage[canBus-1][i];
			
			for(int i=32;i<48;i++)
				newKey[i-32] = hCAN_recMessage[canBus-1][i];
			
			for(int i=48;i<80;i++)
				ivDgst[i-48] = hCAN_recMessage[canBus-1][i];
			
			for(int i=80;i<96;i++)
				newIv[i-80] = hCAN_recMessage[canBus-1][i];
			
			if (!verify_digest(newKey, key, keyDgst)){
					 GUI_Text(10, 120, (uint8_t*) "Errore verifica KEY", Black, Yellow);
			} else {
					AES(&newParam_dec, newKey, 16);
					okKey = res[0] = 1;
			}
			if (!verify_digest(newIv, key, ivDgst)){
					 GUI_Text(20, 120, (uint8_t*) "Errore verifica IV", Black, Yellow);
			} else {
					AES(&newParam_dec, newIv, 16);
					okIv = res[1] = 1;
			}
			if (okKey && okIv){
				for(int i = 0; i < 4; i++)
					ctx_dec[3] = AES_init(newKey, newIv);
					ctx = AES_init(newKey, newIv);
					newParam_dec = AES_init(newKey, newIv);
					ack = AES_init(newKey, newIv);
					AES(&ack, (uint8_t *)res, 32);
					while(hCAN_sendMessage(1, (char *) res, 32)!=hCAN_SUCCESS);
			} else {
					while(hCAN_sendMessage(1, (char *) res, 32)!=hCAN_SUCCESS);
			}
		}
	}
	
}

#include "keep_alive.h"
#include "highcan.h"
#include "../GLCD/GLCD.h"
#include "../timer/timer.h"

char alive_message[8] = "I'm alv";
int peripheralsIDs[N_PERIFERICHE] = {FINESTRINO, LUCI, PEDALI, KEYGEN, DASHBOARD};
char connectedPeripherals[N_PERIFERICHE] = {0};
 
void alive_timer_init(int timerID){
	init_timer(timerID, 0x186A0); // timer 1Mhz -> interval = 0.1s 
	enable_timer(timerID);
}


void sendHello(){
	int r = hCAN_sendMessage(2, alive_message, 8);
	
	while(r != hCAN_SUCCESS){
		r = hCAN_sendMessage(2, alive_message, 8);
	}
}

void receiveHello(int id){
	connectedPeripherals[ peripheralsIDs[id] ] = 1;
}

void voidHello(int id){
	connectedPeripherals[ peripheralsIDs[id] ] = 0;
}

void disconnected(){
	GUI_Text(0, 0, (uint8_t *) "DISCONNECTED!", White, Red);
}

int checkMsg(char *buff, int id){
	int keep_alive_msg = 1;
	for(int i=0; i<8 && keep_alive_msg; i++){
		if(buff[i] != alive_message[i])
			keep_alive_msg = 0;
	}
	if(keep_alive_msg)
		receiveHello(id);
	
	return keep_alive_msg;
}

void TIMER3_IRQHandler (void){
	static int phase = 0;
	int n_disconnected = 0;

	switch(phase){
		case 0:
			for(int i=0; i<N_PERIFERICHE; i++) peripheralsIDs[i] = 0;
			sendHello();
			phase = 1;
			break;
		case 1:
			for(int i=0; i<N_PERIFERICHE; i++) 
				if(peripheralsIDs[i] == 0) n_disconnected++;
			
			if(n_disconnected== N_PERIFERICHE)
					disconnected();
			
			phase = 1;
			break;
	}
}
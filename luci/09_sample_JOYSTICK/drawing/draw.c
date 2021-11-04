#include "draw.h"


void clear_box(int x, int y, uint16_t color){
	int i, j;
	for(j = y; j<y+16; j++){
		for(i = x; i<x+16;i++){
			LCD_SetPoint(i,j,color);
		}
	}
	return;
}


void correctRed (uint8_t lowBeamsState, uint8_t highBeamsState, uint8_t breakLights){
	if (lowBeamsState==0 && highBeamsState==0){
		LED_Off(7);
		LED_Off(5);
	}
	if (highBeamsState==0 && lowBeamsState == 0){
		LED_Off(6);
		LED_Off(5);
	}
	if (highBeamsState==0 && lowBeamsState == 1) LED_Off(6);
	if (highBeamsState==1 && lowBeamsState == 0) LED_Off(7);
	if (breakLights == 0) LED_Off(4);
	
}

void correctRed2 (uint8_t leftTurnSignal, uint8_t rightTurnSignal, uint8_t hazardLight){
	if (leftTurnSignal==0) LED_Off(3);
	if (rightTurnSignal==0) LED_Off(0);
	if (hazardLight == 0) {
		LED_Off(0);
		LED_Off(1);
		LED_Off(2);
		LED_Off(3);
	}
	
}

void correctGreen (uint8_t lowBeamsState, uint8_t highBeamsState, uint8_t breakLights){
	if (lowBeamsState==1){
		LED_On(7);
		LED_On(5);
	}
	if (highBeamsState==1){
		LED_On(6);
		LED_On(5);
	}
	if (breakLights == 1) LED_On(4);
	LED_Off(0);
	LED_Off(1);
	LED_Off(2);
	LED_Off(3);
}

void correctGreen2 (uint8_t leftTurnSignal, uint8_t rightTurnSignal, uint8_t hazardLight){
	if (leftTurnSignal == 1){
		LED_Off(0);
		LED_Off(1);
		LED_Off(2);
	}
	if (rightTurnSignal == 1){
		LED_Off(1);
		LED_Off(2);
		LED_Off(3);
	}
}

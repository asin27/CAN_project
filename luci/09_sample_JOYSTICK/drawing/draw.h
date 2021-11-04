#ifndef DRAW_H
#define DRAW_H

#include <stdint.h>
#include "../GLCD/GLCD.h"
#include "../led/led.h"

void clear_box(int x, int y, uint16_t color);
void correctRed (uint8_t lowBeamsState, uint8_t highBeamsState, uint8_t breakLights);
void correctGreen (uint8_t lowBeamsState, uint8_t highBeamsState, uint8_t breakLights);
void correctGreen2 (uint8_t leftTurnSignal, uint8_t rightTurnSignal, uint8_t hazardLight);
void correctRed2 (uint8_t leftTurnSignal, uint8_t rightTurnSignal, uint8_t hazardLight);


#endif

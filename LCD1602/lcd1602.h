#ifndef _lcd1602_h_
#define _lcd1602_h_


#include "reg52.h"

void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData);
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);
void LCMInit(void); //LCM≥ı ºªØ


#endif 


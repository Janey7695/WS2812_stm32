#ifndef _WS2812_H
#define _WS2812_H

#include "stm32f10x.h"



#define Bit_24 unsigned long int
#define Pos_x  unsigned char
#define Pos_y  unsigned char	
#define COUNT unsigned char	
	
typedef struct cav
{
	Bit_24 Canvas[64];
}CanvasStruct;



void Timer2_init(void);
void RGB_DrawFrame(Bit_24 *Frame);
void InitCanvas(CanvasStruct *ca_stu);
void Draw_Pixel(CanvasStruct *ca_stu,Pos_x x,Pos_y y,Bit_24 color);
void ClearCanvas(CanvasStruct *ca_stu);

#endif



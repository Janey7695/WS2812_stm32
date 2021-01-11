#include "stm32f10x.h"
#include "ws2812.h"
#include "delay.h"

//流星结点数据结构
typedef struct meteor
{
	Pos_x x;
	Pos_y y;
	Bit_24 color[4];
}Meteor;

//创建一条流星线
void createOneMeteor(Meteor *meteor,Pos_x X,Pos_y Y)
{
	meteor->x = X;
	meteor->y = Y;
	meteor->color[0]=0x006400;
	meteor->color[1]=0x00FF00;
	meteor->color[2]=0x008B22;
	meteor->color[3]=0x00420d;
}

//流星结点移动
void meteorStroke(Meteor *meteor,COUNT meteornum)
{
	while(meteornum--)
	{
		meteor[meteornum].y-=1;
		if(meteor[meteornum].y==0)
			meteor[meteornum].y=7;
	}
}

//将流星线画到画布里
void drawMeteorToCanvas(CanvasStruct *canv,Meteor *meteor,COUNT meteornum)
{
	ClearCanvas(canv);
	while(meteornum--)
	{
		Draw_Pixel(canv,meteor[meteornum].x,meteor[meteornum].y,meteor[meteornum].color[0]);
		if(meteor[meteornum].y+1>7)
			Draw_Pixel(canv,meteor[meteornum].x,0,meteor[meteornum].color[1]);
		else
			Draw_Pixel(canv,meteor[meteornum].x,meteor[meteornum].y+1,meteor[meteornum].color[1]);
		if(meteor[meteornum].y+2>7)
			Draw_Pixel(canv,meteor[meteornum].x,0,meteor[meteornum].color[2]);
		else
			Draw_Pixel(canv,meteor[meteornum].x,meteor[meteornum].y+2,meteor[meteornum].color[2]);
		if(meteor[meteornum].y+3>7)
			Draw_Pixel(canv,meteor[meteornum].x,0,meteor[meteornum].color[3]);
		else
			Draw_Pixel(canv,meteor[meteornum].x,meteor[meteornum].y+3,meteor[meteornum].color[3]);
		
	}
}



 int main(void)
 {	
	 CanvasStruct FrameOne;
	Meteor meteors[8];
	delay_init(72);	    //延时函数初始化	  
	Timer2_init();
	InitCanvas(&FrameOne);
	 
	 createOneMeteor(&meteors[0],0,7);
	 createOneMeteor(&meteors[1],1,2);
	 createOneMeteor(&meteors[2],2,5);
	 createOneMeteor(&meteors[3],3,7);
	 createOneMeteor(&meteors[4],4,4);
	 createOneMeteor(&meteors[5],5,1);
	 createOneMeteor(&meteors[6],6,6);
	 createOneMeteor(&meteors[7],7,1);
	 
	 drawMeteorToCanvas(&FrameOne,meteors,8);
	 RGB_DrawFrame(FrameOne.Canvas);
	delay_ms(100);
	 while(1)
	{
		meteorStroke(meteors,8);
		drawMeteorToCanvas(&FrameOne,meteors,8);
		RGB_DrawFrame(FrameOne.Canvas);
		delay_ms(100);
	}
 }




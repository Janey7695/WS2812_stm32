#include "ws2812.h"

/* Buffer that holds one complete DMA transmission
 * 
 * Ensure that this buffer is big enough to hold
 * all data bytes that need to be sent
 * 
 * The buffer size can be calculated as follows:
 * number of LEDs * 24 bytes + 42 bytes
 * 
 * This leaves us with a maximum string length of
 * (2^16 bytes per DMA stream - 42 bytes)/24 bytes per LED = 2728 LEDs
 */
//#define TIM3_CCR3_Address 0x4000043c 	// physical memory address of Timer 3 CCR1 register
//#define TIM3_CCR1_Address 0x40000434	// physical memory address of Timer 3 CCR1 register
#define TIM2_CCR1_Address 0x40000034
	
#define TIMING_ONE  50
#define TIMING_ZERO 25
 uint16_t LED_BYTE_Buffer[300];
 
 //坐标映射表
 static unsigned char theCoordinateMap[8][8]={
	{57,56,41,40,25,24,9,8},
	{58,55,42,39,26,23,10,7},
	{59,54,43,38,27,22,11,6},
	{60,53,44,37,28,21,12,5},
	{61,52,45,36,29,20,13,4},
	{62,51,46,35,30,19,14,3},
	{63,50,47,34,31,18,15,2},
	{64,49,48,33,32,17,16,1}
};
//---------------------------------------------------------------//



void Timer2_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* GPIOA Configuration: TIM2 Channel 1 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/* Compute the prescaler value */
	//PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 90-1; // 800kHz 
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
		
	/* configure DMA */
	/* DMA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* DMA1 Channel6 Config */
	DMA_DeInit(DMA1_Channel2);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM2_CCR1_Address;	// physical address of Timer 3 CCR1
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)LED_BYTE_Buffer;		// this is the buffer memory 
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						// data shifted from memory to peripheral
	DMA_InitStructure.DMA_BufferSize = 42;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// automatically increase buffer index
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							// stop DMA feed after buffer size is reached
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);

		/* TIM3 CC1 DMA Request enable */
	TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);
}

//往画布里写一个像素点
void Draw_Pixel(CanvasStruct *ca_stu,Pos_x x,Pos_y y,unsigned long int color)
{
	if(x>63||y>63)
	{
		//取值越界
	}
	else
	ca_stu->Canvas[theCoordinateMap[x][y]-1]=color;
}


//初始化画布
void InitCanvas(CanvasStruct *ca_stu)
{
	unsigned char i;
	for(i=0;i<63;i++)
	ca_stu->Canvas[i]=0;
}
//清除画布
void ClearCanvas(CanvasStruct *ca_stu)
{
	unsigned char i;
	for(i=0;i<63;i++)
	ca_stu->Canvas[i]=0;
}

//显示一帧图像
void RGB_DrawFrame(unsigned long int *Frame)
{
	uint8_t i;
	uint8_t len=64;
	uint16_t memaddr;
	uint16_t buffersize;
	buffersize = (64*24)+43;	// number of bytes needed is #LEDs * 24 bytes + 42 trailing bytes
	memaddr = 0;				// reset buffer memory index

	while (len)
	{	
		for(i=0; i<8; i++) // GREEN data
		{
			LED_BYTE_Buffer[memaddr] = ((Frame[64-len]>>(15-i)) & 0x001) ? TIMING_ONE:TIMING_ZERO;
			memaddr++;
		}
		for(i=0; i<8; i++) // RED
		{
				LED_BYTE_Buffer[memaddr] = ((Frame[64-len]>>(23-i)) & 0x001) ? TIMING_ONE:TIMING_ZERO;
				memaddr++;
		}
		for(i=0; i<8; i++) // BLUE
		{
				LED_BYTE_Buffer[memaddr] = ((Frame[64-len]>>(7-i)) & 0x001) ? TIMING_ONE:TIMING_ZERO;
				memaddr++;
		}
		len--;
	}
//===================================================================//	
//bug：最后一个周期波形不知道为什么全是高电平，故增加一个波形
  	LED_BYTE_Buffer[memaddr] = ((Frame[64-len]>>16) & 0x001) ? TIMING_ONE:TIMING_ZERO;
//===================================================================//	
	  memaddr++;	
		while(memaddr < buffersize)
		{
			LED_BYTE_Buffer[memaddr] = 0;
			memaddr++;
		}

		DMA_SetCurrDataCounter(DMA1_Channel2, buffersize); 	// load number of bytes to be transferred
		DMA_Cmd(DMA1_Channel2, ENABLE); 			// enable DMA channel 6
		TIM_Cmd(TIM2, ENABLE); 						// enable Timer 3
		while(!DMA_GetFlagStatus(DMA1_FLAG_TC2)) ; 	// wait until transfer complete
		TIM_Cmd(TIM2, DISABLE); 	// disable Timer 3
		DMA_Cmd(DMA1_Channel2, DISABLE); 			// disable DMA channel 6
		DMA_ClearFlag(DMA1_FLAG_TC2); 				// clear DMA1 Channel 6 transfer complete flag
}


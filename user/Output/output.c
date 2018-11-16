
#include "output.h"
#include "delay.h"

void GPIO_OutPut_Init(void)   
{	
//	RCC->APB2ENR|=1<<6;			//使能PORTCE时钟	
//	GPIOE->CRL&=0XFFFFFFFF;	//PE4,PE6 推挽输出 速度10MHz 
//	GPIOE->CRL|=0X01010000;
//	JTAG_Set(SWD_ENABLE);
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//PA15 禁用JTAG
			
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5 | GPIO_Pin_15;//PA5：P4,PA15：RFID_VCC
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 				//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//IO口速度为50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);									//初始化GPIOEA
	GPIO_ResetBits(GPIOA, GPIO_Pin_5|GPIO_Pin_15);				//IO输出0
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;							//PB7：P28_PUL1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				
 	GPIO_Init(GPIOB, &GPIO_InitStructure);									
	GPIO_ResetBits(GPIOB, GPIO_Pin_7);			
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3;//PC0：P1,PC1：P2,PC3：P3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				
 	GPIO_Init(GPIOC, &GPIO_InitStructure);									
	GPIO_ResetBits(GPIOC, GPIO_Pin_0|GPIO_Pin_1 | GPIO_Pin_3);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_7;//PE0：P27_DIR2,PE2：P27_PUL2,PE4：P28_DIR1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				
 	GPIO_Init(GPIOE, &GPIO_InitStructure);									
	GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_4);
	GPIO_ResetBits(GPIOE,GPIO_Pin_7);
}


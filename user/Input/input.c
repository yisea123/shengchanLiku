
#include "input.h"
//#include "led.h"
//#include "can.h"

_DEVICE_OBJECT Device_obj;	//结构体变量 工作站属性

void GPIO_InPut_Init(void)		//传感器端口初始化
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; //IO		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);				//初始化GPIOB
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13; //立库扩展板X轴前限位PC13
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOC, &GPIO_InitStructure);				//初始化GPIOC
		
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7|GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14|GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOE, &GPIO_InitStructure);			
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_15 | GPIO_Pin_11 | GPIO_Pin_6 | GPIO_Pin_9; //立库扩展板，PE:3X轴后限位，PE15:Z轴极限位，PE11:A面传感器,PE6:B面传感器
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOE, &GPIO_InitStructure);	
}


void Send_Sensor_Status(void)		//传感器状态变化上传
{

}

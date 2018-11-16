#include "Laser.h"


u8 Lrecflag;
u8 distance_data[2];//发送数据显示格式 CCC.M cm ; distance_data[0] = CCC; distance_data[1] = M;
u8 rec_num;
u8 rec_len;
u8 recdata[4];//接收缓存
char* Set_mode1 = "s5-1#";//被动接收模式
//char* Set_mode0 = "s5-0#";//主动发送模式
//char* Get_distance = "r6#";//获取测量结果，只在被动模式有效
u8 Set_mode[5] ={0x72,0x35,0x2d,0x31,0x23};
u8 Get_distance[3] ={0x72,0x36,0x23};
enum	UART2_MODE Uart2_Mode;
//int  Laser_Distance;					 //距离

//串口3初始化
//bound 波特率
//


/***********************************************
* 函数名称：void Laser_Init(void)
* 功能描述：激光模块初始化
* 输入参数：无
* 调用函数：无
* 返回结果：无
***********************************************/
void Laser_Init()
{
	char* Set_mode0 = "s5-1#";//被动接收模式
	while (0 != *Set_mode0)  //发送模式指令
	{  
			USART_SendData(USART2,*Set_mode0);	
			while( USART_GetFlagStatus(USART2,USART_FLAG_TC)!= SET);
			Set_mode0++;  
	}  
	

}
//采样5次，去高去低取平均
int SamplingAverage(int *Sampling)
{
	int Bubble[2]={0};
	int Sum=0;
	u8 i;
	u8 size;
	int Average;

	size = 5;
	Bubble[0] = Sampling[0];
	Bubble[1] = Sampling[1];
	for(i=0;i<size;i++)
	{
		if(Sampling[i]>Bubble[0])//取大值放入Bubble[0]
		{
			Bubble[0]=Sampling[i];
		}
		if(Sampling[i]<Bubble[1])//取小值放入Bubble[1]
		{
			Bubble[1]=Sampling[i];
		}
	}
	
	for(i=0;i<size;i++)//求总和
	{
		Sum+=Sampling[i];
	}
	//去掉最大值和最小值
	Sum-=Bubble[0];
	Sum-=Bubble[1];
	
	Average = Sum/(size-2);
	return Average;
}
void UART_SendChar(USART_TypeDef* USARTx, uint8_t Data)
{
    USART_SendData(USARTx, Data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
}

void UART_SendStr(USART_TypeDef* USARTx, char *str)  
{  
    while (0 != *str)  
    {  
       UART_SendChar(USARTx, *str);  
       str++;  
    }  
}

int Read_Laser()
{
		u8 i;
		distance_data[0] = 0;
		distance_data[1] = 0;
		int Laser_Distance = 0;


	
		/*for(i =0;i<3;i++)
	 {
	
		USART_SendData(USART2,Get_distance[i]);	
		while( USART_GetFlagStatus(USART2,USART_FLAG_TC)!= SET);
	 }
		
		delay_ms(100);*/
	 
		if(Lrecflag == 1)
		{
			switch(rec_len)
			{
				case 1:
				{
					distance_data[0] = 0x00;
					distance_data[1] = (recdata[0]-0x30)*10;
					
				
				}break;
				case 2:
				{
					distance_data[0] = recdata[0]-0x30;
					distance_data[1] = (recdata[1]-0x30)*10;
				
				}break;
				case 3:
				{
					distance_data[0] = (recdata[0]-0x30)*10+(recdata[1]-0x30);
					distance_data[1] = (recdata[2]-0x30)*10;
				
				}break;
				case 4:
				{
					distance_data[0] = (recdata[0]-0x30)*100+(recdata[1]-0x30)*10+(recdata[2]-0x30);
					distance_data[1] = (recdata[3]-0x30)*10;
				
				}break;
			
			}
			
			Laser_Distance = distance_data[0]*10+distance_data[1];
			return Laser_Distance;
			
		}
		else
		{
				return 0XFF;
		}

	
}



//void USART3_IRQHandler(void)
//{
//	u8 rec_buff;
//  USART_ClearITPendingBit(USART3,USART_IT_RXNE); 

//	rec_buff=USART_ReceiveData(USART3);	//接收数据暂存
//	
//	switch(rec_num)//初始值为0
//	{
//		case 0: //帧头1
//		{
//			 if(rec_buff==0x4C)//"L"
//			 {
//			 	rec_num=1;
//			 }
//       else 
//			 {
//			 	rec_num=0;//接收初始化重新接收
//			 }
//    }break;
//		case 1: //帧头2
//		{
//			 if(rec_buff==0x3D)//"="
//			 {
//			 	rec_num=2;
//				 
//			 }
//       else 
//			 {
//			 	rec_num=0;//接收初始化重新接收
//			 }
//    }break;

//		case 2: //数据0
//		{
//			 recdata[0] = rec_buff ;	//
//			 rec_num = 3;
//		}break;
//		case 3: //数据1
//		{
//			 if(rec_buff!=0x6d)//结束
//			 {
//					 recdata[1] = rec_buff;	//
//					 rec_num = 4;
//			 }
//			 else
//			 {
//				 rec_num = 0;//接收初始化重新接收
//				 Lrecflag = 1; //接收完成
//				 rec_len = 1;
//			 }
//				
//		}break;			
//		case 4://数据2
//		{
//			 if(rec_buff!=0x6d)//结束
//			 {
//					 recdata[2] = rec_buff;	//
//					 rec_num = 5;
//			 }
//			 else
//			 {
//				 rec_num = 0;//接收初始化重新接收
//				 Lrecflag = 1; //接收完成
//				 rec_len = 2;
//			 }
//		
//						 
//		}break;
//		case 5://数据3
//		{
//			 if(rec_buff!=0x6d)//结束
//			 {
//					 recdata[3] = rec_buff;	//
//					 rec_num = 6;
//			 }
//			 else
//			 {
//				 rec_num = 0;//接收初始化重新接收
//				 Lrecflag = 1; //接收完成
//				 rec_len = 3;
//			 }
//			 			 
//		}break;
//		case 6://校验
//		{
//			
//				if(rec_buff == 0x6d)//结束
//			 {
//					rec_num = 0;//接收初始化重新接收
//					Lrecflag = 1; //接收完成
//				  rec_len = 4;
//			 }
//			 else
//			 {
//				 rec_num = 0;//接收初始化重新接收
//				 Lrecflag = 0; //接收完成标志
//				 rec_len = 0;
//				 //Beep(50, 50, 1); //蜂鸣器响次数
//			 }
//			 
//		
//		}break;
//	} 
//}




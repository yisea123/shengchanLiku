#include "Laser.h"


u8 Lrecflag;
u8 distance_data[2];//����������ʾ��ʽ CCC.M cm ; distance_data[0] = CCC; distance_data[1] = M;
u8 rec_num;
u8 rec_len;
u8 recdata[4];//���ջ���
char* Set_mode1 = "s5-1#";//��������ģʽ
//char* Set_mode0 = "s5-0#";//��������ģʽ
//char* Get_distance = "r6#";//��ȡ���������ֻ�ڱ���ģʽ��Ч
u8 Set_mode[5] ={0x72,0x35,0x2d,0x31,0x23};
u8 Get_distance[3] ={0x72,0x36,0x23};
enum	UART2_MODE Uart2_Mode;
//int  Laser_Distance;					 //����

//����3��ʼ��
//bound ������
//


/***********************************************
* �������ƣ�void Laser_Init(void)
* ��������������ģ���ʼ��
* �����������
* ���ú�������
* ���ؽ������
***********************************************/
void Laser_Init()
{
	char* Set_mode0 = "s5-1#";//��������ģʽ
	while (0 != *Set_mode0)  //����ģʽָ��
	{  
			USART_SendData(USART2,*Set_mode0);	
			while( USART_GetFlagStatus(USART2,USART_FLAG_TC)!= SET);
			Set_mode0++;  
	}  
	

}
//����5�Σ�ȥ��ȥ��ȡƽ��
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
		if(Sampling[i]>Bubble[0])//ȡ��ֵ����Bubble[0]
		{
			Bubble[0]=Sampling[i];
		}
		if(Sampling[i]<Bubble[1])//ȡСֵ����Bubble[1]
		{
			Bubble[1]=Sampling[i];
		}
	}
	
	for(i=0;i<size;i++)//���ܺ�
	{
		Sum+=Sampling[i];
	}
	//ȥ�����ֵ����Сֵ
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

//	rec_buff=USART_ReceiveData(USART3);	//���������ݴ�
//	
//	switch(rec_num)//��ʼֵΪ0
//	{
//		case 0: //֡ͷ1
//		{
//			 if(rec_buff==0x4C)//"L"
//			 {
//			 	rec_num=1;
//			 }
//       else 
//			 {
//			 	rec_num=0;//���ճ�ʼ�����½���
//			 }
//    }break;
//		case 1: //֡ͷ2
//		{
//			 if(rec_buff==0x3D)//"="
//			 {
//			 	rec_num=2;
//				 
//			 }
//       else 
//			 {
//			 	rec_num=0;//���ճ�ʼ�����½���
//			 }
//    }break;

//		case 2: //����0
//		{
//			 recdata[0] = rec_buff ;	//
//			 rec_num = 3;
//		}break;
//		case 3: //����1
//		{
//			 if(rec_buff!=0x6d)//����
//			 {
//					 recdata[1] = rec_buff;	//
//					 rec_num = 4;
//			 }
//			 else
//			 {
//				 rec_num = 0;//���ճ�ʼ�����½���
//				 Lrecflag = 1; //�������
//				 rec_len = 1;
//			 }
//				
//		}break;			
//		case 4://����2
//		{
//			 if(rec_buff!=0x6d)//����
//			 {
//					 recdata[2] = rec_buff;	//
//					 rec_num = 5;
//			 }
//			 else
//			 {
//				 rec_num = 0;//���ճ�ʼ�����½���
//				 Lrecflag = 1; //�������
//				 rec_len = 2;
//			 }
//		
//						 
//		}break;
//		case 5://����3
//		{
//			 if(rec_buff!=0x6d)//����
//			 {
//					 recdata[3] = rec_buff;	//
//					 rec_num = 6;
//			 }
//			 else
//			 {
//				 rec_num = 0;//���ճ�ʼ�����½���
//				 Lrecflag = 1; //�������
//				 rec_len = 3;
//			 }
//			 			 
//		}break;
//		case 6://У��
//		{
//			
//				if(rec_buff == 0x6d)//����
//			 {
//					rec_num = 0;//���ճ�ʼ�����½���
//					Lrecflag = 1; //�������
//				  rec_len = 4;
//			 }
//			 else
//			 {
//				 rec_num = 0;//���ճ�ʼ�����½���
//				 Lrecflag = 0; //������ɱ�־
//				 rec_len = 0;
//				 //Beep(50, 50, 1); //�����������
//			 }
//			 
//		
//		}break;
//	} 
//}




 
#include "Sys_Init.h"	
int main(void)
{
	/*******�ϵ��ʼ��*******/
	Sys_Init();		

	/***************************��ѭ��****************************/
	while(1)
	{
//		Axis_Ultra_Limit();//XZ��������λ
//		Tray_Limit_Contr();//������λ
		
		if(Timer_1ms_flag == 1)//1ms��ѯ��ִ����Ҫ��ʱ�Ĺ���   ��������/ģ�����
		{
			//asdasd();
		//	Uart_Analysis();//���紮�ڽ���
			Timer_1ms_flag = 0;
			//XYZ_BackZero();//���Ḵλ�׶���ѯ
			Key_Deal();//����ɨ��
//			SURE_REC_UART2_Analyze();     //���������ڽ���
//			Get_Box(Action.Target_X,Action.Target_Z);//ȡ��׶���ѯ
//			Put_Box(Action.Target_X,Action.Target_Z);//����׶���ѯ
//			if(Uart3[Cache_Cnt].RecError ==1)//�������ݴ��󣬶����������ݣ����رմ���5����
//			{
//				Beep(50,50,3);
//				Uart3[Cache_Cnt].RecError = 0;				
//				memset(&Uart3[Cache_Cnt].Buffer,0,sizeof(Uart3[Cache_Cnt].Buffer));//�������
//				Uart_Send(0,Analysis_Error,Local_Ip,0x61,Analysis_Error,0);//���ر�����Ϣ	
//				USART_Cmd(USART3, DISABLE);//�رմ���
//				Enable_Uart3_Flg = 1;
//			}
//			if(Reset_Flag ==1)//������λ�������־λ
//			{
//				PAout(5) = 0;
//				Reset_Flag=0;
//			}
		}
	}
}


int fputc(int ch, FILE *f)
{
	USART_SendData(USART3, (unsigned char) ch);
	while (!(USART3->SR & USART_FLAG_TXE));
	return (ch);

}


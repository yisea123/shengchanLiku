#include "Auto_Locate.h"



u16 Distance_Detection(void);
u8 Auto_Position_Buff[8] ;//���ص�ǰ������Ϣ
u16 Calc_Z_Distance(u8 Z_Cnt);

void X_Uniform_Speed(void);//X��ˮƽ����ǰ��
u16 X_Cal_Uniform_Dis(int);
TYPEDEF_ARRARY X_Buf;
TYPEDEF_ARRARY Z_Buf[5];
TYPEDEF_UART2 Laser_Rec[LasBufLen];//���ܻ���
int X_Average_Distance = 0;//ˮƽƽ������
int Z_Average_Distance = 0;//��ֱƽ������
u8 X_Mutation_Judge(int Temp_Dis);//ͻ���쳣�ж�
u8 Laser_Length;
int Read_Distance(void);

u8 Laser_Rright=0;//���ܻ�������

u8 Current_X=0;
u8 Current_Y=0;
u8 Current_Z=0;
int test(void);
TYPEDEF_AUTOLOCATE Auto_Adjust =
{
	.Auto_Locate_Status_Change=1,
	.Over_Flag=0,
	.Start_Flag=0,
	.Auto_Locate_Stage=1,
	.X_Cnt=1,
	.Y_Cnt=0,
	.Z_Cnt=1,
};

void Auto_Locate()
{
	//Auto_Rest
	if(Auto_Adjust.Start_Flag == 1)
	{
		int temp_dis;
		int result;
		if(Auto_Adjust.Auto_Locate_Stage == 1)//��λ״̬��
		{
			if(Auto_Adjust.Auto_Locate_Status_Change == 1)
			{
				Auto_Adjust.Auto_Locate_Status_Change = 0;
				Auto_Rest();//��λ��ԭ��
			}
			if(XYZ_To_Zero.Over_Flag == 1)
			{
				Auto_Adjust.Auto_Locate_Status_Change = 1;
				Auto_Adjust.Auto_Locate_Stage++;//������һ���׶�
			}
		}

		else if(Auto_Adjust.Auto_Locate_Stage == 2)//XZ�˶�״̬��
		{			
			if(Auto_Adjust.Auto_Locate_Stage == 1)//��λ״̬��
			{
				X_Uniform_Speed();//X������ǰ��
			}
			temp_dis = Read_Distance();
			if(temp_dis==0xff)return;			
				result = X_Cal_Uniform_Dis(temp_dis);			
			if(result!=0)
			{
				X_Average_Distance = result;
				Auto_Adjust.Auto_Locate_Status_Change = 1;
				Auto_Adjust.Auto_Locate_Stage++;//������һ���׶�
				return;
			}
			
		}
		else if(Auto_Adjust.Auto_Locate_Stage == 3)
		{
			temp_dis = Read_Distance();
			if(temp_dis==0xff)return;
			
				result = X_Mutation_Judge(temp_dis);
			if(result==0xff)//�����ͻ��
			{
				Action_Stop();
			}
			else
			{
				return;
			}
		}


		else
		{
			Auto_Adjust.Auto_Locate_Stage = 1;
			Auto_Adjust.Start_Flag = 0;
			Auto_Adjust.X_Cnt = 1;
			Auto_Adjust.Z_Cnt = 1;
			Auto_Adjust.Y_Cnt = 0;
		}
		
		
	}
}

/*static u8 Traverse_XYZ(u8 Y_Cnt,u8 Z_Cnt ,u8 X_Cnt)
{
	u16 Z_Run;
	if (Auto_Adjust.X_Cnt > 5)//X������꣬Z�������һ��
	{
		++Auto_Adjust.Z_Cnt;
		Auto_Adjust.X_Cnt = 1;
		Auto_Adjust.Z_Cnt = Z_Cnt;
	}	
	
	Auto_Adjust.X_Cnt = X_Cnt;//����
	Auto_Adjust.Z_Cnt = Z_Cnt;//����
	Auto_Adjust.Y_Cnt = Y_Cnt;//����
	Z_Run = Calc_Z_Distance(Z_Cnt);//����Ŀ��Z�����
	
	Current_X=Auto_Adjust.X_Cnt;//����λ��
	Current_Y=Auto_Adjust.Y_Cnt;//
	Current_Z=Auto_Adjust.Z_Cnt;//
	
	if(Auto_Adjust.X_Cnt ==1 && Auto_Adjust.Z_Cnt == 10 && Auto_Adjust.Y_Cnt == 1)//��������
	{
		Auto_Adjust.X_Cnt = 1;
		Auto_Adjust.Z_Cnt = 1;
		Auto_Adjust.Y_Cnt = 0;
		return 1;
	}	
	Motor_MoveToPosition(X_Cnt*412,Z_Run);//�ƶ���Ŀ�����
	
	++Auto_Adjust.X_Cnt;//������һ��X��	
	if(Auto_Adjust.X_Cnt > 5 && Auto_Adjust.Z_Cnt == 9&&Auto_Adjust.Y_Cnt==0)
	{
		Auto_Adjust.X_Cnt = 1;
		Auto_Adjust.Z_Cnt = 1;
		Auto_Adjust.Y_Cnt++;
	}

	return 0;
}*/

int test()
{
	return 1;
}

u8 X_Mutation_Judge(int Temp_Dis)
{
	static u8 i =0;
	u8 equal_cnt = 0;
	X_Buf.Distance_Arrary[i] = Temp_Dis;
	i++;
	if(i>=16)
	{
		i = 0;
		for(int i =0;i<16;i++)
		{			
			if((X_Average_Distance -5)<X_Buf.Distance_Arrary[i]&&X_Buf.Distance_Arrary[i]<(X_Average_Distance +5))
			{
				equal_cnt++;
			}
		}
		if(equal_cnt>=8)
		{
			return 0;
		}
		else 
			return 0xff;//����ͻ��
	}
		else
		{
			return 0 ;
		}
}
u16 X_Cal_Uniform_Dis(int Temp_Dis)
{
	static u8 i =0;
	static u32 sum=0;
	u16 Aver_Dis;
	static u8 equal_cnt = 0;
	X_Buf.Distance_Arrary[i] = Temp_Dis;
	sum+=Temp_Dis;
	i++;
	if(i>=16)
	{
		i = 0;
		Aver_Dis = sum>>4;
		sum = 0;
		for(int j =0;j<16;j++)
		{
			
			if(((Aver_Dis -5)<X_Buf.Distance_Arrary[j])&&(X_Buf.Distance_Arrary[j]<(Aver_Dis +5)))
			{
				equal_cnt++;

			}
		}
			if(equal_cnt==16)
			{
				equal_cnt = 0;
				return Aver_Dis;
			}
			else 
			{
				equal_cnt = 0;
				i = 0;
				sum = 0;
				return 0;
			}
		
	}
	else
	{
		
		return 0;
	}
}

int Read_Distance(void)
{
	int temp_distance;
	if(Lrecflag)
	{
		Lrecflag = 0;
		switch (Laser_Rec[Laser_Rright-1].Data_Len)
		{
			case 1:
			{
				Laser_Rec[LasBufLen-1].distance_data[0] = Laser_Rec[Laser_Rright-1].Buf[0];;
				Laser_Rec[LasBufLen-1].distance_data[1] = 0x00;				
				Laser_Rec[LasBufLen-1].distance_data[2] = 0x00;
				temp_distance = Laser_Rec[LasBufLen-1].distance_data[0]-0x30;
			}break;
			case 2:
			{
				Laser_Rec[LasBufLen-1].distance_data[0] = Laser_Rec[Laser_Rright-1].Buf[0];
				Laser_Rec[LasBufLen-1].distance_data[1] = Laser_Rec[Laser_Rright-1].Buf[1];
				Laser_Rec[LasBufLen-1].distance_data[2] = 0x00;
				temp_distance = (Laser_Rec[LasBufLen-1].distance_data[0]-0x30)*10+(Laser_Rec[LasBufLen-1].distance_data[1]-0x30);
			}break;
			case 3:
			{
				Laser_Rec[LasBufLen-1].distance_data[0] = Laser_Rec[Laser_Rright-1].Buf[0];
				Laser_Rec[LasBufLen-1].distance_data[1] = Laser_Rec[Laser_Rright-1].Buf[1];
				Laser_Rec[LasBufLen-1].distance_data[2] = Laser_Rec[Laser_Rright-1].Buf[2];
				temp_distance =(Laser_Rec[LasBufLen-1].distance_data[0]-0x30)*100+(Laser_Rec[LasBufLen-1].distance_data[1]-0x30)*10+
				(Laser_Rec[LasBufLen-1].distance_data[2]-0x30);
			}break;
		}
		//temp_distance = Laser_Rec[LasBufLen-1].distance_data[0]*10+Laser_Rec[LasBufLen-1].distance_data[1];
		Laser_Rright--;
		return temp_distance;
	}
	else
	{
		return 0xff;
	}

}
void X_Uniform_Speed()//X��ˮƽ����ǰ��
{
	DIR_X = X_DIR_FRONT;
	Axis_X.Dir = X_DIR_FRONT;
	Axis_X.NowPulse = 0;
	Axis_X.Target_Pulse = 50000;
	Axis_X.InCtrl_Flag = 4;//��λģʽ
}




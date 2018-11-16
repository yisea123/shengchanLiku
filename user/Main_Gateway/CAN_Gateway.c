#include "CAN_Gateway.h"



//����TCU�豸��ʱ��Ӧ��ָ�������ת��λ���豸��100ms���޷����ص�ָ��
//3�γ�ʱ��100ms��ʱ
unsigned char Tcu_Commend_Gateway(unsigned commend,unsigned char deviceid,unsigned char *candata,unsigned char canlenh)
{
	unsigned char i;
	ReSendCanCommendTime_ms=1500;//100mS�ط�
	while(ReSendCanCommendTime_ms)//��ѯ�豸״̬�ط�3��
	{
		if(ReSendCanCommendTime_ms==1500||ReSendCanCommendTime_ms==1000||ReSendCanCommendTime_ms==500)
		{
			ReSendCanCommendTime_ms--;
			
			CANTXMEGSS.CANTXBuff[0]=0x80|canlenh;
			CANTXMEGSS.CANTXBuff[1]=deviceid;//Ŀ�ص�ַ
			CANTXMEGSS.CANTXBuff[2]=Device_Prop.DEVICE_ID;//Դ��ַ
			CANTXMEGSS.CANTXBuff[3]=0x10;//��ʾ��CCU/TCU����SCU/DCU/ACU��ָ��
			CANTXMEGSS.CANTXBuff[4]=commend<<3;//ָ��	
			for(i=0;i<canlenh;i++)
			{
				CANTXMEGSS.CANTXBuff[5+i]=candata[i];
			}
			SendMessage(CANTXMEGSS.CANTXBuff);			
		}
		SURE_REC_Analyze_Gateway();      	
		if(ReSendCanCommendTime_ms==1)
		{
			ReSendCanCommendTime_ms=0;
			return 1;//ʧ�ܷ���1
		}
	}	
	return 0;//�ɹ�
}


/***********************************************
* �������ƣ�void SURE_REC_Analyze(void)
* �����������ж��Ƿ�Ҫ�������ڽ��յ�������
* �����������
* ���ú�������
* ���ؽ������
***********************************************/

void SURE_REC_Analyze_Gateway(void)
{
  u8 i, j;
  u8 dlc;                                    //���ܱ��ĵĳ���
  u8 id[4];
  u8 RecData[8]={0};                         //8���ֽڵĽ�������
  
  if(canPoll())                              //�ȴ�����
  {				
    dlc=canRead(0, id, RecData);          	 //CAN���߶�ȡ����
    CANTXMEGSS.CANRXBuff[0]=0x40;
    CANTXMEGSS.CANRXBuff[1]=0xf7;
    CANTXMEGSS.CANRXBuff[2]=0xf7;            //Э���У�ֻ������f7��f7���Թ̶���ֵ
    CANTXMEGSS.CANRXBuff[3]=dlc+5;           //CAN��ϢͷΪ5���ֽڣ�����8�������ֽ�                    
    CANTXMEGSS.CANRXBuff[4]=0x80+dlc;        //��չ֡+���ݳ���     �ֽ�1
    CANTXMEGSS.CANRXBuff[5]=id[0];           //IDΪCAN��29Ϊ�˲�ͷ���ֽ�2��ʾĿ��ID���ֽ�3��ʾ����ID���ֽ�4��ʾ��Ϣ֡���ֽ�5��ʾ����
    CANTXMEGSS.CANRXBuff[6]=id[1];
    CANTXMEGSS.CANRXBuff[7]=id[2];
    CANTXMEGSS.CANRXBuff[8]=id[3];
		
		if(CANTXMEGSS.CANRXBuff[5]==Device_Prop.DEVICE_ID || CANTXMEGSS.CANRXBuff[5]==0x02)
		{
			for(i=0; i<dlc; i++)                   //8λ���ݣ����ڻ򲻰������ݵ�֡ȫΪ0
			{
				 CANTXMEGSS.CANRXBuff[9+i] = RecData[i];
			}
				
			CANTXMEGSS.CANRXBuff[9+dlc]=0;         //��ȡУ��֮ǰ��֤18λΪ0
			for(i=0;i<dlc+8;i++)	//CHECK
			{
				 CANTXMEGSS.CANRXBuff[9+dlc]+=CANTXMEGSS.CANRXBuff[i+1];    //CRCУ��	
			}
			REC_Analyze_Gateway();                         //�������ڽ��յ�������
		}
		else
		{
			for(j=0;j<8;j++)
      {
        CANTXMEGSS.CANRXBuff[i]=0;
      }
		}
  }
}

/***********************************************************************************
* �������ƣ�void REC_Analyze(void)
* ������������������1���յ�������
* �����������
* ���ú�������
* ���ؽ������
* ֡��ʼ��0x40��+״̬��(0xf7)+������CMD��0xf7��+ ���ݳ��ȣ�DLC ��+�������ݣ�data[0]-data[12]��+У���ֽڣ�CheckByte��
***********************************************************************************/
void REC_Analyze_Gateway(void)
{
	u8 i;
	u8 DLC;
	u8 check=0;
	DLC=CANTXMEGSS.CANRXBuff[3];

	for(i=0; i<DLC+3; i++)//У��
	{
		check+=CANTXMEGSS.CANRXBuff[i+1];		
	}

	if(CANTXMEGSS.CANRXBuff[DLC+4]==check)//У��ɹ�			
	{
	  if(CANTXMEGSS.CANRXBuff[0]==0x40) //֡��ʼ          
	  {
	    if(CANTXMEGSS.CANRXBuff[1]==0xF7)//���ձ��ĵ�״̬��(0xf7)
	    {				
	      if(CANTXMEGSS.CANRXBuff[2]==0xF7)//���ձ������� ������CMD
	      {
	        if(CANTXMEGSS.CANRXBuff[3] >= 5)//����ʶ����5���ֽ� ����Ϊ0-8�ֽ�	  
	        {
						if(CANTXMEGSS.CANRXBuff[4]&0x80)//FF=1Ϊ��չ֡��ʹ�õĶ�����չ֡
						{
							if((CANTXMEGSS.CANRXBuff[5]==Device_Prop.DEVICE_ID)||(CANTXMEGSS.CANRXBuff[5]==0x02))//�豸ID�ź͹㲥ID
							{
                if((CANTXMEGSS.CANRXBuff[7] & 0x10) == 0x10)//֡��Ϣ   �����ܵ�����ϵͳ��
								{
									switch(CANTXMEGSS.CANRXBuff[6]) //�ж����ĸ��豸CCU/TCU����������Ϣ
                  {
										case 0x01: 						//TCU���͹������ź�
                    {
											switch(CANTXMEGSS.CANRXBuff[7] & 0x03)		//֡����
											{
                        case 0x00:			//CCU Ӧ��֡
                        {
                          if(CANTXMEGSS.CANRXBuff[9] == 0x01)
                          {
                            ReSendCanCommendTime_ms = 0; //�����ط���־																																
                            Beep(30,30,1);
                          }
                        }break;
                        case 0x01:  	 	//��ʾ��CCU/TCU����ACU��ָ��֡
                        {
                          switch(CANTXMEGSS.CANRXBuff[8]>>3)
                          {
                            case 1:  //CCU��ѯ�豸״̬
                            {
                              
                            }break;
                          }
                        }break;
                          
                        case 0x02:	//CCU�㲥֡
                        {
                          switch(CANTXMEGSS.CANRXBuff[8]>>3)
                          {
                            case 1:
                            {	
                             
                            }
                            break;                     
                          }
                          break;                          
                        }
                        case 0x03://״̬֡ CCUȷ��������֡ 
                        {

                        }break;
                      }//֡����
                    }//TCU���������ź�
                  }//CAN����վ
							  }//֡��Ϣ
						  }//CANĿ��վ���߹㲥֡
	          }//��չ֡
	        }//����ʶ����5�ֽ�
	      }//���ձ�������
	    }//���ձ��ĵ�״̬��
	  }//֡��ʼ
  }
}





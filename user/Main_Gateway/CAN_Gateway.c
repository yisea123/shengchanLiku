#include "CAN_Gateway.h"



//发给TCU设备即时响应的指令，不包括转工位等设备在100ms内无法返回的指令
//3次超时，100ms超时
unsigned char Tcu_Commend_Gateway(unsigned commend,unsigned char deviceid,unsigned char *candata,unsigned char canlenh)
{
	unsigned char i;
	ReSendCanCommendTime_ms=1500;//100mS重发
	while(ReSendCanCommendTime_ms)//查询设备状态重发3次
	{
		if(ReSendCanCommendTime_ms==1500||ReSendCanCommendTime_ms==1000||ReSendCanCommendTime_ms==500)
		{
			ReSendCanCommendTime_ms--;
			
			CANTXMEGSS.CANTXBuff[0]=0x80|canlenh;
			CANTXMEGSS.CANTXBuff[1]=deviceid;//目地地址
			CANTXMEGSS.CANTXBuff[2]=Device_Prop.DEVICE_ID;//源地址
			CANTXMEGSS.CANTXBuff[3]=0x10;//表示是CCU/TCU发往SCU/DCU/ACU的指令
			CANTXMEGSS.CANTXBuff[4]=commend<<3;//指令	
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
			return 1;//失败反回1
		}
	}	
	return 0;//成功
}


/***********************************************
* 函数名称：void SURE_REC_Analyze(void)
* 功能描述：判断是否要解析串口接收到的数据
* 输入参数：无
* 调用函数：无
* 返回结果：无
***********************************************/

void SURE_REC_Analyze_Gateway(void)
{
  u8 i, j;
  u8 dlc;                                    //接受报文的长度
  u8 id[4];
  u8 RecData[8]={0};                         //8个字节的接收数据
  
  if(canPoll())                              //等待接收
  {				
    dlc=canRead(0, id, RecData);          	 //CAN总线读取数据
    CANTXMEGSS.CANRXBuff[0]=0x40;
    CANTXMEGSS.CANRXBuff[1]=0xf7;
    CANTXMEGSS.CANRXBuff[2]=0xf7;            //协议中，只是用了f7，f7所以固定给值
    CANTXMEGSS.CANRXBuff[3]=dlc+5;           //CAN信息头为5个字节，加上8个数据字节                    
    CANTXMEGSS.CANRXBuff[4]=0x80+dlc;        //扩展帧+数据长度     字节1
    CANTXMEGSS.CANRXBuff[5]=id[0];           //ID为CAN的29为滤波头，字节2表示目的ID，字节3表示主机ID，字节4表示信息帧，字节5表示命令
    CANTXMEGSS.CANRXBuff[6]=id[1];
    CANTXMEGSS.CANRXBuff[7]=id[2];
    CANTXMEGSS.CANRXBuff[8]=id[3];
		
		if(CANTXMEGSS.CANRXBuff[5]==Device_Prop.DEVICE_ID || CANTXMEGSS.CANRXBuff[5]==0x02)
		{
			for(i=0; i<dlc; i++)                   //8位数据，少于或不包含数据的帧全为0
			{
				 CANTXMEGSS.CANRXBuff[9+i] = RecData[i];
			}
				
			CANTXMEGSS.CANRXBuff[9+dlc]=0;         //读取校验之前保证18位为0
			for(i=0;i<dlc+8;i++)	//CHECK
			{
				 CANTXMEGSS.CANRXBuff[9+dlc]+=CANTXMEGSS.CANRXBuff[i+1];    //CRC校验	
			}
			REC_Analyze_Gateway();                         //解析串口接收到的数据
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
* 函数名称：void REC_Analyze(void)
* 功能描述：解析串口1接收到的数据
* 输入参数：无
* 调用函数：无
* 返回结果：无
* 帧起始（0x40）+状态字(0xf7)+命令字CMD（0xf7）+ 数据长度（DLC ）+接收数据（data[0]-data[12]）+校验字节（CheckByte）
***********************************************************************************/
void REC_Analyze_Gateway(void)
{
	u8 i;
	u8 DLC;
	u8 check=0;
	DLC=CANTXMEGSS.CANRXBuff[3];

	for(i=0; i<DLC+3; i++)//校验
	{
		check+=CANTXMEGSS.CANRXBuff[i+1];		
	}

	if(CANTXMEGSS.CANRXBuff[DLC+4]==check)//校验成功			
	{
	  if(CANTXMEGSS.CANRXBuff[0]==0x40) //帧起始          
	  {
	    if(CANTXMEGSS.CANRXBuff[1]==0xF7)//接收报文的状态字(0xf7)
	    {				
	      if(CANTXMEGSS.CANRXBuff[2]==0xF7)//接收报文数据 命令字CMD
	      {
	        if(CANTXMEGSS.CANRXBuff[3] >= 5)//报文识别码5个字节 数据为0-8字节	  
	        {
						if(CANTXMEGSS.CANRXBuff[4]&0x80)//FF=1为扩展帧，使用的都是扩展帧
						{
							if((CANTXMEGSS.CANRXBuff[5]==Device_Prop.DEVICE_ID)||(CANTXMEGSS.CANRXBuff[5]==0x02))//设备ID号和广播ID
							{
                if((CANTXMEGSS.CANRXBuff[7] & 0x10) == 0x10)//帧信息   气动管道传输系统用
								{
									switch(CANTXMEGSS.CANRXBuff[6]) //判断是哪个设备CCU/TCU发过来的信息
                  {
										case 0x01: 						//TCU发送过来的信号
                    {
											switch(CANTXMEGSS.CANRXBuff[7] & 0x03)		//帧类型
											{
                        case 0x00:			//CCU 应答帧
                        {
                          if(CANTXMEGSS.CANRXBuff[9] == 0x01)
                          {
                            ReSendCanCommendTime_ms = 0; //清零重发标志																																
                            Beep(30,30,1);
                          }
                        }break;
                        case 0x01:  	 	//表示是CCU/TCU发往ACU的指令帧
                        {
                          switch(CANTXMEGSS.CANRXBuff[8]>>3)
                          {
                            case 1:  //CCU查询设备状态
                            {
                              
                            }break;
                          }
                        }break;
                          
                        case 0x02:	//CCU广播帧
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
                        case 0x03://状态帧 CCU确定传感器帧 
                        {

                        }break;
                      }//帧类型
                    }//TCU发过来的信号
                  }//CAN发送站
							  }//帧信息
						  }//CAN目的站或者广播帧
	          }//扩展帧
	        }//报文识别码5字节
	      }//接收报文数据
	    }//接收报文的状态字
	  }//帧起始
  }
}





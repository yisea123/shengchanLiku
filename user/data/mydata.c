
#include "mydata.h"

//10进制数转16进制数 (BCD形式)
//单个BCD 10进制转16进制
u8 DEC2HEX_REG(u8 Data_DEC) //BCD形式
{
	u8 Temp;
	if(Data_DEC >= 10) //10以上 A B C D E F
	{
//		Temp = ('A' + (Data_DEC - 10));
		Temp = (0x0A + (Data_DEC - 10));
	}
	else
	{
		Temp = Data_DEC;
	}
	return Temp;
}

//32位数转BCD形式16进制
u32 DEC2HEX(u32 Data_DEC)
{
	u32 DEC_Temp = Data_DEC;
//	u8	DEC_REG; 
	u32 HEX_Temp;
	u8  DEC_HEX;
//	u8  HEX_REG[6];
	
	if((DEC_Temp > 0) && DEC_Temp<16)												//16^1
	{
		DEC_HEX = 1;
	}
	else if((DEC_Temp >= 16) && DEC_Temp<256)								//16^2
	{
		DEC_HEX = 2;
	}
	else if((DEC_Temp >= 256) && DEC_Temp<4096)							//16^3
	{
		DEC_HEX = 3;
	}
	else if((DEC_Temp >= 4096) && DEC_Temp<65536)						//16^4
	{
		DEC_HEX = 4;
	}
	else if((DEC_Temp >= 65536) && DEC_Temp<1048576)				//16^5
	{
		DEC_HEX = 5;
	}
	else if((DEC_Temp >= 1048576) && DEC_Temp<16777216)			//16^6
	{
		DEC_HEX = 6;
	}
	else if((DEC_Temp >= 16777216) && DEC_Temp<268435456)		//16^7
	{
		DEC_HEX = 7;
	}
//	else if((DEC_Temp >= 268435456) && DEC_Temp<4294967295)	//16^8
//	{
//		DEC_HEX = 8;
//	}
					
	switch(DEC_HEX)
	{
		case 1:
		{			
			HEX_Temp = DEC2HEX_REG(HEX_Temp%16);
		}
		break;
		case 2:
		{
//			HEX_Temp = HEX_Temp/16;
//			HEX_Temp = (HEX_Temp<<4)|(HEX_Temp%16);
			HEX_Temp = (DEC2HEX_REG(DEC_Temp/16)<<4)|(DEC2HEX_REG(DEC_Temp%16));					
		}
		break;
		case 3:
		{
//			HEX_Temp = HEX_Temp/256;
//			HEX_Temp = (HEX_Temp<<4)|(HEX_Temp%256/16);
//			HEX_Temp = (HEX_Temp<<4)|(HEX_Temp%256%16);
			HEX_Temp = (DEC2HEX_REG(DEC_Temp/256)<<8)|(DEC2HEX_REG(DEC_Temp%256/16)<<4)|(DEC2HEX_REG(DEC_Temp%256%16));
		}
		break;
		case 4:
		{
//			HEX_Temp = HEX_Temp/4096;
//			HEX_Temp = (HEX_Temp<<4)|(HEX_Temp%4096/256);
//			HEX_Temp = (HEX_Temp<<4)|(HEX_Temp%4096%256/16);
//			HEX_Temp = (HEX_Temp<<4)|(HEX_Temp%4096%256%16);
			HEX_Temp = (DEC2HEX_REG(DEC_Temp/4096)<<12)|(DEC2HEX_REG(DEC_Temp%4096/256)<<8)|(DEC2HEX_REG(DEC_Temp%4096%256/16)<<4)|(DEC2HEX_REG(DEC_Temp%4096%256%16));
		}
		break;
		case 5:
		{
//			HEX_Temp = HEX_Temp/65536;
//			HEX_Temp = (HEX_Temp<<1)|(HEX_Temp%65536/4096);
//			HEX_Temp = (HEX_Temp<<1)|(HEX_Temp%65536%4096/256);
//			HEX_Temp = (HEX_Temp<<1)|(HEX_Temp%65536%4096%256/16);
//			HEX_Temp = (HEX_Temp<<1)|(HEX_Temp%65536%4096%256%16);
//			HEX_Temp = (DEC2HEX_REG(DEC_Temp/65536)<<4)|(DEC2HEX_REG(DEC_Temp%65536/4096)<<3)|(DEC2HEX_REG(DEC_Temp%65536%4096/256)<<2)|(DEC2HEX_REG(DEC_Temp%65536%4096%256/16)<<1)|(DEC2HEX_REG(DEC_Temp%65536%4096%256%16));	

//		  HEX_Temp = DEC2HEX_REG(DEC_Temp/65536); 
//			HEX_Temp = (HEX_Temp<<4)|(DEC2HEX_REG(DEC_Temp%65536/4096));
//			HEX_Temp = (HEX_Temp<<4)|(DEC2HEX_REG(DEC_Temp%65536%4096/256));
//			HEX_Temp = (HEX_Temp<<4)|(DEC2HEX_REG(DEC_Temp%65536%4096%256/16));
//			HEX_Temp = (HEX_Temp<<4)|(DEC2HEX_REG(DEC_Temp%65536%4096%256%16));
			HEX_Temp = (DEC2HEX_REG(DEC_Temp/65536)<<16)|(DEC2HEX_REG(DEC_Temp%65536/4096)<<12)|(DEC2HEX_REG(DEC_Temp%65536%4096/256)<<8)|(DEC2HEX_REG(DEC_Temp%65536%4096%256/16)<<4)|(DEC2HEX_REG(DEC_Temp%65536%4096%256%16));	
		}
		break;
		case 6:
		{			
		  HEX_Temp = DEC2HEX_REG(DEC_Temp/1048576); 
			HEX_Temp = (HEX_Temp<<4)|(DEC2HEX_REG(DEC_Temp%1048576/65536));
			HEX_Temp = (HEX_Temp<<4)|(DEC2HEX_REG(DEC_Temp%1048576%65536/4096));
			HEX_Temp = (HEX_Temp<<4)|(DEC2HEX_REG(DEC_Temp%1048576%65536%4096/256));
			HEX_Temp = (HEX_Temp<<4)|(DEC2HEX_REG(DEC_Temp%1048576%65536%4096%256/16));
			HEX_Temp = (HEX_Temp<<4)|(DEC2HEX_REG(DEC_Temp%1048576%65536%4096%256%16));
		}
		break;
		case 7:
		{
			HEX_Temp = DEC2HEX_REG(DEC_Temp/268435456); 
			HEX_Temp = DEC2HEX_REG(DEC_Temp%268435456/1048576);
			HEX_Temp = (HEX_Temp<<4)|(DEC2HEX_REG(DEC_Temp%268435456%1048576/65536));
			HEX_Temp = (HEX_Temp<<4)|(DEC2HEX_REG(DEC_Temp%268435456%1048576%65536/4096));
			HEX_Temp = (HEX_Temp<<4)|(DEC2HEX_REG(DEC_Temp%268435456%1048576%65536%4096/256));
			HEX_Temp = (HEX_Temp<<4)|(DEC2HEX_REG(DEC_Temp%268435456%1048576%65536%4096%256/16));
			HEX_Temp = (HEX_Temp<<4)|(DEC2HEX_REG(DEC_Temp%268435456%1048576%65536%4096%256%16));
		}
		break;	
	}
	
	return HEX_Temp;
}

//u8 DEC2ARR(u32 data, u8 S[])
//{
//	u8 i;
////	u8 S[i];
//	DEC2HEX(data);
//	for(i=0; i<4; i++)
//	{
//		S[i] = (DEC2HEX(data)>>(0 + 8*(i+1)));
//	}
//	
//	return S[i];
//}

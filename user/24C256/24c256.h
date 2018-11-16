#ifndef __24C256_H
#define __24C256_H

#include "Sys_Init.h"
#include "stm32f10x.h"
#include "stm32f10x_it.h"
//#include "system_init.h"

#define SCL_H         GPIOB->BSRR = GPIO_Pin_8
#define SCL_L         GPIOB->BRR  = GPIO_Pin_8 
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_9
#define SDA_L         GPIOB->BRR  = GPIO_Pin_9

#define SCL_read      GPIOB->IDR  & GPIO_Pin_8
#define SDA_read      GPIOB->IDR  & GPIO_Pin_9

/**************************����ģ��1�ĵ�ַ*********************************/
#define ADD_IP			          6  									//�洢IP��ַ��EEPROM��ַ
#define IP_EEP_NUM   					2   								//2�ֽ�

#define ADD_WEIGHT_A			    ADD_IP+IP_EEP_NUM  	//�洢���ش��������ֱ�ߵ�б��
#define WEIGHT_A_EEP_NUM   		8   								//8�ֽ�

#define ADD_WEIGHT_B			    ADD_WEIGHT_A+WEIGHT_A_EEP_NUM  	//�洢���ش��������ֱ�ߵĽؾ�
#define WEIGHT_B_EEP_NUM   		8   								//8�ֽ�
/***********************************************************/

/**************************����ģ��2�ĵ�ַ********************************/
#define ADD_IP2			          30  									//�洢IP��ַ��EEPROM��ַ
#define IP_EEP_NUM2   					2   								//2�ֽ�

#define ADD_WEIGHT_A2			    ADD_IP2+IP_EEP_NUM2  	//�洢���ش��������ֱ�ߵ�б��
#define WEIGHT_A_EEP_NUM2   		8   								//8�ֽ�

#define ADD_WEIGHT_B2			    ADD_WEIGHT_A2+WEIGHT_A_EEP_NUM2  	//�洢���ش��������ֱ�ߵĽؾ�
#define WEIGHT_B_EEP_NUM2   		8  
/***********************************************************/
#define ADD_ICEBOXDATANUM				24								//�洢��������������ַ
#define ICEBOXDATANUM_EEP_NUM		2									//2�ֽ�

#define ADD_ERRNUM							26								//���ϴ���
#define ERRNUM_EEP_NUM					1									//1�ֽ�

#define ADD_IDCARDDATA          28                //ID����ŵ�ַ
#define IDCARD_EEP_NUM          40                //40�ֽ� 10�ſ���ÿ��4�ֽ�


#define ADD_ICEBOXDATA					100								//����������ʼ��ַ
#define ICEBOXDATA_EEP_NUM			4000							//������������

////�豸��Ϣ
//typedef struct
//{
//	unsigned char StationName;	//�豸����
//	unsigned char StationName2;	//�豸����	
//	unsigned char Pipe0ID; 		  //�ܿ�0�����豸��
//	unsigned char Pipe1ID; 		  //�ܿ�1�����豸��
//	unsigned char Pipe2ID; 		  //�ܿ�2�����豸��
//	unsigned char Pipe3ID; 		  //�ܿ�3�����豸��
//	unsigned char Pipe4ID; 		  //�ܿ�4�����豸��
//	unsigned char Pipe0Time;	  //�ܿ�0����ʱ��

//	unsigned char OnLine;	      //����״̬		  bit7��ʾ�豸��ϵͳ����У�Ӧ������
//															//              bit6��ʾ������·������0��ʾû������������
//															//						  bit0��ʾ�豸����
//															//              bit1��ʾ�ù���վ���ڷ��ͻ�ȴ�����ȡƿ����40��
//															//              bit2��ʾ�ù���վ���վ��������û�о������Ͳ��ܹ�����ƿ
//															//              bit3�ɹ��������У��ȴ�����
//	unsigned char Senser;
//}DEVICE;

//typedef union
//{
//	DEVICE ID[0x36];
//	unsigned char pd[DEVICE_EEP_NUM];
//}DEVICE_TypeDef ;

//extern DEVICE_TypeDef      device; 								        //�豸  ϵͳ��� ��ַ���豸��ID��һһ��Ӧ

////�����¼
//typedef struct
//{
//	unsigned char Month;			//�����¼-��
//	unsigned char Day; 				//�����¼-��  
//	unsigned char Hour; 			//�����¼-ʱ	 
//	unsigned char Min; 				//�����¼-��
//	unsigned char Sec; 				//�����¼-��  
//	unsigned char StarName; 	//�����¼-ʼ��վ����  
//	unsigned char EndName;		//�����¼-Ŀ��վ����
//	unsigned char Result;	  	//�����¼-������
//}RECORD;

//typedef union
//{
//	RECORD RecordNum[5];
//	unsigned char pd[40];
//}RECORD_TypeDef ;

//extern RECORD_TypeDef      Record; 								        //�豸  ϵͳ��� ��ַ���豸��ID��һһ��Ӧ









void IIC_Init(void);
extern unsigned char I2C_BufferWrite(u8* pBuffer, u16 length, u16 WriteAddress);
unsigned char I2C_ReadByte(u8* pBuffer, u16 length, u16 ReadAddress);
void I2C_PageWrite(u8* pBuffer, u16 length, u16 WriteAddress);

#endif

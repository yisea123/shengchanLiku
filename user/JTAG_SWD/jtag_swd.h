#ifndef __JTAGSWD_H
#define __JTAGSWD_H

#include "sys.h"

//JTAGģʽ���ö���

#define JTAG_ENABLE				 0X04		//�ر�SWD,����JTAG
#define JTAG_SWD_DISABLE   0X02		//�ر�JTAG,����SWD
#define SWD_ENABLE         0X01		//��ȫSWJ״̬����JNTRST
#define JTAG_SWD_ENABLE    0X00		//��ȫSWJ״̬����λ״̬ 

void JTAG_Set(u8 mode);	//JTAG\SWDģʽ����
void Free_JTAG(void);		//�ͷ�JTAG����,����ͨIO

#endif

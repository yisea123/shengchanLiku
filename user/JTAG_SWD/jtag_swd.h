#ifndef __JTAGSWD_H
#define __JTAGSWD_H

#include "sys.h"

//JTAG模式设置定义

#define JTAG_ENABLE				 0X04		//关闭SWD,启用JTAG
#define JTAG_SWD_DISABLE   0X02		//关闭JTAG,启用SWD
#define SWD_ENABLE         0X01		//完全SWJ状态：无JNTRST
#define JTAG_SWD_ENABLE    0X00		//完全SWJ状态：复位状态 

void JTAG_Set(u8 mode);	//JTAG\SWD模式设置
void Free_JTAG(void);		//释放JTAG引脚,做普通IO

#endif

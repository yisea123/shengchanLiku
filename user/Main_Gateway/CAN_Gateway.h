#ifndef __CAN_GATEWAY_H
#define __CAN_GATEWAY_H

#include "Sys_Init.h"
//#include "sys.h"	 
//#include "can.h"

void REC_Analyze_Gateway(void);
void SURE_REC_Analyze_Gateway(void);
unsigned char Tcu_Commend_Gateway(unsigned commend,unsigned char deviceid,unsigned char *candata,unsigned char canlenh);



#endif

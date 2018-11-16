#include "sys.h"
#include "network.h"
 
//系统中断分组设置	
void NVIC_Configuration(void)
{
//	NVIC_InitTypeDef NVIC_InitStructure;						//定义NVIC初始化结构体

  	/* Set the Vector Table base location at 0x08000000 */
  	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

}

//从SYSRESETREQ被置为有效，到复位发生器执行复位命令，往往会有一个延时。在此延时期间，
//处理器仍然可以响应中断请求。但我们的本意往往是要让此次执行到此为止，不要再做任何其它事情了。
//所以，最好在发出复位请求前，先把FAULTMASK置位。所以最好在将FAULTMASK置位才万无一失。
void System_SoftReset(void)
{
	__set_FAULTMASK(1);	//关闭所有中断	
	NVIC_SystemReset();	//复位
	Reset_Flag = 0;
}


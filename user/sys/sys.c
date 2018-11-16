#include "sys.h"
#include "network.h"
 
//ϵͳ�жϷ�������	
void NVIC_Configuration(void)
{
//	NVIC_InitTypeDef NVIC_InitStructure;						//����NVIC��ʼ���ṹ��

  	/* Set the Vector Table base location at 0x08000000 */
  	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

}

//��SYSRESETREQ����Ϊ��Ч������λ������ִ�и�λ�����������һ����ʱ���ڴ���ʱ�ڼ䣬
//��������Ȼ������Ӧ�ж����󡣵����ǵı���������Ҫ�ô˴�ִ�е���Ϊֹ����Ҫ�����κ����������ˡ�
//���ԣ�����ڷ�����λ����ǰ���Ȱ�FAULTMASK��λ����������ڽ�FAULTMASK��λ������һʧ��
void System_SoftReset(void)
{
	__set_FAULTMASK(1);	//�ر������ж�	
	NVIC_SystemReset();	//��λ
	Reset_Flag = 0;
}


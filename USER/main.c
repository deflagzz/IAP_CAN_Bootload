#include "my_include.h"   



int main(void)
{	 	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	delay_init();	    								//延时函数初始化
	IAP_CAN_Remap_Init(); 								//CAN引脚重映射配置

	IAP_BootLoad_Init();								//上电等待更新固件的延时
	
	while(1)
	{		
		IAP_BootLoad_UpData();							//更新固件
	}		
}



#include "my_include.h"   



int main(void)
{	 	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal);//CAN初始化环回模式,波特率500Kbps    
	delay_init();	    								//延时函数初始化
	
	IAP_BootLoad_Init();								//上电等待更新固件的延时
	
	while(1)
	{		
		IAP_BootLoad_UpData();							//更新固件
	}		
}



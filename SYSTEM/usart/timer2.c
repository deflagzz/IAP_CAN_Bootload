#include "timer2.h"

u8 timeover=5;//���ڽ������ݣ���ʱ�����õĶ�ʱ����10ms*5



void time2_init(u16 arr,u16 psc)//���ڽ������ݣ���ʱ�����õĶ�ʱ����10ms*5
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrct;
	NVIC_InitTypeDef NVIC_InitStrct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	
	TIM_TimeBaseInitStrct.TIM_Period=arr;
	TIM_TimeBaseInitStrct.TIM_Prescaler=psc;
	TIM_TimeBaseInitStrct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStrct.TIM_CounterMode=TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStrct);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	TIM_Cmd(TIM2,DISABLE);//ʧ��
	
	NVIC_InitStrct.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStrct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStrct.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStrct.NVIC_IRQChannelCmd=ENABLE;
	
	NVIC_Init(&NVIC_InitStrct);
	
	
	
	
	
}

void TIM2_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)  //���ڽ������ݣ���ʱ�����õĶ�ʱ����10ms*5
	{
		
		TIM_ClearFlag(TIM2,TIM_IT_Update);
		timeover--;
	}
}







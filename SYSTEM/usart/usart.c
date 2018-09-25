#include "usart.h"
	  



//////////////////////////////////////////////////////////////////*
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  			//*	
#if 1																														//*	
#pragma import(__use_no_semihosting)             								//*	
//��׼����Ҫ��֧�ֺ���                 													//*	
struct __FILE 																									//*	
{ 																															//*
	int handle; 																								  //*
																										            //*
}; 																															//*	
																														    //*
FILE __stdout;       																						//*				
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    												//*			
_sys_exit(int x) 																								//*
{ 																															//*		
	x = x; 																												//*					
} 																															//*			
//�ض���fputc���� 																							//*	
int fputc(int ch, FILE *f)																			//*		
{     																												  //*
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   				//*								
    USART1->DR = (u8) ch;      																	//*		
	return ch;																						        //*
}																													      //*
#endif 																													//*
//////////////////////////////////////////////////////////////////*


 
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������  
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
char USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

//u16 USART_RX_STA=0;       //����״̬���	 
//u8 cont=0;

u16 usart_recv_len = 0;

u8 usart_rx_flag = 0;

  


void usart_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=bound;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);
	
	//USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);       //ֻ�������ڿ����ж�
	USART_Cmd(USART1,ENABLE);
	
	
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
	
	
	
	
	
	
	
}





void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u16 tttemp=0;
	tttemp = tttemp;

	if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)
	{
		tttemp = USART1 -> SR;   //������������ж�
		tttemp = USART1 -> DR;		//
		
		usart_rx_flag = 1;
		
		/*��ȡһ֡�����ֽڳ���*/
		DMA_Cmd(DMA1_Channel5, DISABLE );  //�ر�USART1 RX DMA1 ��ָʾ��ͨ��  
		usart_recv_len=65535-DMA_GetCurrDataCounter(DMA1_Channel5);
		USART_RX_BUF[usart_recv_len] = '\0';  //����Ҫ��ʾ�ĳ���
		DMA_SetCurrDataCounter(DMA1_Channel5,65535);//DMAͨ����DMA����Ĵ�С
		DMA_Cmd(DMA1_Channel5, ENABLE);  //ʹ��USART1 RX DMA1 ��ָʾ��ͨ�� 
	}
	
		

	//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //���ڽ������ݣ���ʱ����10ms*5
//	{
//		
//		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
//		USART_RX_BUF[cont]=Res;
//		cont++;
//		
//		
//		
//		USART_ClearFlag(USART1,USART_FLAG_RXNE);
//		
//	
//	 
//	 } 
		
		
		
} 
#endif	









//void uart_init(u32 bound){
//  //GPIO�˿�����
//  GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	 
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
//  
//	//USART1_TX   GPIOA.9
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������  
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
//   
//  //USART1_RX	  GPIOA.10��ʼ��
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
//  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

//  //Usart1 NVIC ����
//  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
//	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
//  
//	
//   //USART ��ʼ������

//	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
//	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

//  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
//  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
//  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

//}












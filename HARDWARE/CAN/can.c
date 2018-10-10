#include "can.h"


//0x7D ��λ�����豸��Ϣָ��
//0x7B ��λ����Ӧָ��

//0x7F ��λ������ˢ�̼�ָ��
//0x7E ��λ����Ӧָ��

/*
1.���豸��Ϣ
	ID   Len  Data
			  CMD 
    7D   08   01 00 00 00 00 00 00 00     
  ��Ӧ
	ID   Len  Data  
			  CMD  SerialNum   CANID  DeviceType   RunMode
    7B   08   81   ** ** ** ** **     **           **       

1.����ˢ��
	ID   Len  Data
			  CMD  SerialNum	FirmWare Length  
    7F   08   01   ** ** ** **  ** ** **     
  ��Ӧ
	ID   Len  Data  
			  CMD  SerialNum
    7E   08   81   ** ** ** ** 00 00 00

3.��ʼˢ��
	ID   Len  Data
    7F   08   ** ** ** ** ** ** ** **  
  ����������1K������1K���ְ�ʵ�ʳ��ȷ��ͣ��豸�յ�1K����Ӧ��
  
  ��Ӧ
	ID   Len  Data
			  CMD SerialNum    Revice Length
    7E   08   82  ** ** ** **  ** ** **
  ���жϳ�������뷢��һ�¼������ͣ���һ�����·��ͣ�*/





u8 g_UT_CAN_ID = 0 ;


 
//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024;  tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
//������Ϊ:36M/((8+9+1)*4)=500Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��; 
//Remap_type 0:Ĭ������ 1:��ӳ��
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode,u8 Remap_type)
{ 
	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE 
	NVIC_InitTypeDef  		NVIC_InitStructure;
#endif

	if(!Remap_type)	//CAN�˿�Ĭ��PA11��PA12
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��PORTAʱ��	                   											 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
		GPIO_Init(GPIOA, &GPIO_InitStructure);			//��ʼ��IO

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//��������
		GPIO_Init(GPIOA, &GPIO_InitStructure);			//��ʼ��IO	
	}
	else			//CAN�˿���ӳ��ΪPB8��PB9
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��PORTAʱ��	                   											 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
		GPIO_Init(GPIOB, &GPIO_InitStructure);			//��ʼ��IO

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//��������
		GPIO_Init(GPIOB, &GPIO_InitStructure);			//��ʼ��IO	

		GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);//�˿���ӳ��	
	}

	

	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM=DISABLE;			//��ʱ�䴥��ͨ��ģʽ  
	CAN_InitStructure.CAN_ABOM=ENABLE;			//����Զ����߹���	 
	CAN_InitStructure.CAN_AWUM=DISABLE;			//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART=DISABLE;			//��ֹ�����Զ�����  ENABLE
	CAN_InitStructure.CAN_RFLM=DISABLE;		 	//���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP=DISABLE;			//���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode= mode;	        //ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; 
	//���ò�����
	CAN_InitStructure.CAN_SJW=tsjw;				//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 			//Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;				//Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;        //��Ƶϵ��(Fdiv)Ϊbrp+1	
	CAN_Init(CAN1, &CAN_InitStructure);        	//��ʼ��CAN1 

	CAN_FilterInitStructure.CAN_FilterNumber=0;	//������0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	//����λģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	//32λ�� 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;	//32λID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//���������0

	CAN_FilterInit(&CAN_FilterInitStructure);			//�˲�����ʼ��
	
#if CAN_RX0_INT_ENABLE 
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);				//FIFO0��Ϣ�Һ��ж�����.		    

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   


#if CAN_RX0_INT_ENABLE	//ʹ��RX0�ж�
//�жϷ�����			    
//0:�������
//1:���¹̼�
//2:ˢ��̼�
//3:
//4:
//5:
//6:
//7:


void USB_LP_CAN1_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;

    CAN_Receive(CAN1, 0, &RxMessage);
	
	IAP_BootLoad_CAN_RX(RxMessage);
	
}
#endif

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 Can_Send_Msg(u8 ID,u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId = ID;			// ��׼��ʶ��  
	TxMessage.ExtId = ID;			// ������չ��ʾ�� 
	TxMessage.IDE=CAN_Id_Standard; 	// ��׼֡
	TxMessage.RTR=CAN_RTR_Data;		// ����֡
	TxMessage.DLC=len;				// Ҫ���͵����ݳ���
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];			          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)!=CAN_TxStatus_Ok)&&(i<0XFFF))
		i++;	//�ȴ����ͽ���
	if(i>=0XFFF)
		return 1;
	return 0;	 
}
//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 Can_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//û�н��յ�����,ֱ���˳� 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	
    for(i=0;i<8;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}













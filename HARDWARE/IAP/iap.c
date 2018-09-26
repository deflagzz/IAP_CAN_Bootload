#include "iap.h"

//��ȡоƬΨһID
u32 g_CpuID[3];
u32 g_Lock_Code;	


stm32_iap g_iap;

iapfun jump2app; 
u16 iapbuf[1024];  

//appxaddr:Ӧ�ó������ʼ��ַ
//appbuf:Ӧ�ó���CODE.
//appsize:Ӧ�ó����С(�ֽ�).
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize)
{
	u16 t;
	u16 i=0;
	u16 temp;
	u32 fwaddr=appxaddr;//��ǰд��ĵ�ַ
	u8 *dfu=appbuf;
	for(t=0;t<appsize;t+=2)
	{						    
		temp=(u16)dfu[1]<<8;
		temp+=(u16)dfu[0];	  
		dfu+=2;//ƫ��2���ֽ�
		iapbuf[i++]=temp;	    
		if(i==1024)
		{
			i=0;
			STMFLASH_Write(fwaddr,iapbuf,1024);	
			fwaddr+=2048;//ƫ��2048  16=2*8.����Ҫ����2.
		}
	}
	if(i)STMFLASH_Write(fwaddr,iapbuf,i);//������һЩ�����ֽ�д��ȥ.  
}

//��ת��Ӧ�ó����
//appxaddr:�û�������ʼ��ַ.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)appxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��APP.
	}
}		 



void systeam_ReStart(void)
{
	u32 time = 65535;
	while(time--)
	{
		__NOP();
	}
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}
void GetLockCode(u32 *lock_ID)
{
	//??CPU??ID
	g_CpuID[0]=*(vu32*)(0x1ffff7e8);
	g_CpuID[1]=*(vu32*)(0x1ffff7ec);
	g_CpuID[2]=*(vu32*)(0x1ffff7f0);
	//????,????????
	*lock_ID=((g_CpuID[0]>>0)+(g_CpuID[1]>>1)+(g_CpuID[2]>>2)) / 2;
}
//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
static u8 IAP_Can_Send_Msg(u8 ID,u8* msg,u8 len)
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

//����CAN���߽��յ�һ֡����
u8 check_data(u8 *pBuf ,u16 pBufSize)	//1:�ɹ� 0:���� 
{
	u32 get_PC_crcVel=0;
	u8 *pbuff;
	
	pbuff = pBuf+pBufSize-4;
	g_iap.CRC_PC_vel = pbuff[0]<<24|pbuff[1]<<16|pbuff[2]<<8|pbuff[3];	//���յ�CRC32
	
	get_PC_crcVel 	 = CRC32Calculate(pBuf,pBufSize-4);	//MCU�˼����CRC32
	
	if(get_PC_crcVel == g_iap.CRC_PC_vel)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*1.DeviceType
	�豸���ͣ�01:����
			  02:�ŵ���-8 
			  03:�ŵ���-16
			  04:
			  05��
			  06:RFID �ر�
			  07:����������
2.RunMode
	����ģʽ��01:BootLoader 
			  02:APP*/
void Send_response(u8 Can_ID,u8 Byte1,u8 Byte6,u8 Byte7,u8 Byte8)	//������Ӧ
{
	u8 temp[8]={0};

	GetLockCode(&g_Lock_Code);			//��ȡоƬΨһID
		
	temp[0] = Byte1;					//������
	temp[1] = g_Lock_Code>>24&0xff;		//ΨһID
	temp[2] = g_Lock_Code>>16&0xff;	
	temp[3] = g_Lock_Code>>8&0xff;
	temp[4] = g_Lock_Code&0xff;
	temp[5] = Byte6;					//CAN_ID	//ˢ�̼�ģʽ:�������ݳ���3���ֽ�
	temp[6] = Byte7;					//�豸����
	temp[7] = Byte8;					//����ģʽ 01:BootLoader  02:APP

	IAP_Can_Send_Msg(Can_ID,temp,8);
}


void IAP_BootLoad_Init(void)	//�ϵ�ȴ����¹̼�����ʱ
{
	u8 led_num=0;
	CRC32TableCreate();				//CRCУ��ı�
	Send_response(MCU2PC_Rend_device_info,0x81,0,0,1);	//������λ���豸��Ϣ
	led_num = iap_wait_updata_time/50;
	while(led_num--)
	{
		if(g_iap.flag_RX_ == 1)
		{
			break;
		}
		delay_ms(50);
	}

}


/*	g_iap.flag_Recive_Bin:
	11:���ճɹ�
	1:У��ʹ��� 
	2:�������ݲ���1K
	3:FLAHSH�ڳ������	*/
void IAP_BootLoad_UpData(void)
{
	if(g_iap.flag_RX_ == 1)			//1:���¹̼�
	{
		if(g_iap.RX_data_len)
		{	
			if(g_iap.APP_RX_len == g_iap.APP_Bin_len+IAP_CRC_byte_len)			//���Ե�Bin�ļ��������
			{
				g_iap.APP_RX_len = g_iap.APP_RX_len - IAP_CRC_byte_len;			//��ȥ����͵�4���ֽ�
				//���У����Ƿ���ȷ
				if(check_data(g_iap.RX_BUFF,g_iap.RX_data_len))//1:�ɹ�	
				{
					g_iap.flag_Recive_Bin = 11;	
				}
				else
				{
					g_iap.flag_Recive_Bin = 1;					//1:У��ʹ���
				}
			}
			else
			{
				if(g_iap.RX_data_len == iap_start_updata_LEN)	//���չ�1K����	
				{
					g_iap.APP_RX_len = g_iap.APP_RX_len - IAP_CRC_byte_len;	//��ȥ����͵�4���ֽ�
					//���У����Ƿ���ȷ
					if(check_data(g_iap.RX_BUFF,g_iap.RX_data_len))//1:�ɹ�	
					{
						g_iap.flag_Recive_Bin = 11;					
					}
					else
					{
						g_iap.flag_Recive_Bin = 1;			//1:У��ʹ��� 
					}
				}
				else											
				{
					if(!g_iap.TIME_jian)	
					{					
						g_iap.flag_Recive_Bin = 2;	 		//2:�������ݲ���1K			
					}
				}													
			}				
			if(g_iap.flag_Recive_Bin == 11)					//��������֡�ɹ� 
			{
				g_iap.Flash_write_address = FLASH_APP1_ADDR+g_iap.APP_RX_len-(g_iap.RX_data_len-IAP_CRC_byte_len);
				//����FLASH���� 
				iap_write_appbin(g_iap.Flash_write_address,g_iap.RX_BUFF,g_iap.RX_data_len-IAP_CRC_byte_len);
				g_iap.RX_data_len 	   = 0;					//�������ݻ���ָ��
				g_iap.flag_Recive_Bin  = 0;					//�����־
				if(g_iap.APP_RX_len == g_iap.APP_Bin_len)
				{
					g_iap.OK_Recive        = 1;					//�������Bin�ļ�
				}
				//������λ�����չ̼�����
				Send_response(MCU2PC_IAP,0x82,g_iap.APP_RX_len>>16&0xff,g_iap.APP_RX_len>>8&0xff,g_iap.APP_RX_len&0xff);				
			}
			else if(g_iap.flag_Recive_Bin == 1 || g_iap.flag_Recive_Bin == 2)	//��������֡�ɹ� 
			{
				//�������0x83,02:��������1024,������λ�����յ�ǰ֡�̼�����
				Send_response(MCU2PC_IAP,0x83,g_iap.flag_Recive_Bin,(g_iap.RX_data_len-IAP_CRC_byte_len)>>8&0xff,(g_iap.RX_data_len-IAP_CRC_byte_len)&0xff);
				//��λ
				systeam_ReStart();				
			}	
			//
			//�̼���ת
			//			
			if(g_iap.OK_Recive)
			{
				delay_ms(100);
				if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
				{	 						
					g_iap.flag_Recive_Bin = 12;	
					Send_response(MCU2PC_IAP,0x84,1,0,0);	//1:�ɹ� //2:ʧ��
					iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
					
				}
				else //��FLASHӦ�ó���,�޷�ִ��
				{
					g_iap.RX_data_len = 0;
					g_iap.flag_RX_    = 0;	//�������						
					g_iap.flag_Recive_Bin = 3;	
					Send_response(MCU2PC_IAP,0x84,2,0,0);	//1:�ɹ� //2:ʧ��				
					//��λ
					systeam_ReStart();					
				}				
			}		
		}
	}
	else
	{
		if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
		{	 
			g_iap.flag_Recive_Bin = 12;	
			iap_load_app(FLASH_APP1_ADDR);	//ִ��FLASH APP����	
		}
		else 									
		{
			g_iap.flag_Recive_Bin = 3;		//��FLASHӦ�ó���,�޷�ִ��
		}				
	}
	
	if(g_iap.TIME_jian)
	{
		g_iap.TIME_jian--;
	}
	delay_ms(10);

}

//BootLoad����CAN�ж���
void IAP_BootLoad_CAN_RX(CanRxMsg temp_CAN_Msg)
{
	int i=0;
	
	if(temp_CAN_Msg.StdId == PC2MCU_Rend_device_info)	//PC���豸ΨһID
	{
		if(temp_CAN_Msg.Data[0]==1&&temp_CAN_Msg.Data[1]==0&&temp_CAN_Msg.Data[2]==0&&temp_CAN_Msg.Data[3]==0&&
		   temp_CAN_Msg.Data[4]==0&&temp_CAN_Msg.Data[5]==0&&temp_CAN_Msg.Data[6]==0&&temp_CAN_Msg.Data[7]==0)
		{	
			Send_response(MCU2PC_Rend_device_info,0x81,0,0,1);
		}
	
	}
	if(temp_CAN_Msg.StdId == PC2MCU_IAP)
	{		
		if(g_iap.flag_RX_ == 0)		//0:
		{
			GetLockCode(&g_Lock_Code);					//��ȡоƬΨһID

			if(	temp_CAN_Msg.Data[0]==0x01&&	
				temp_CAN_Msg.Data[1]==(g_Lock_Code>>24&0xff)&& 
				temp_CAN_Msg.Data[2]==(g_Lock_Code>>16&0xff)&&
				temp_CAN_Msg.Data[3]==(g_Lock_Code>>8&0xff)&&	
				temp_CAN_Msg.Data[4]==(g_Lock_Code&0xff) )
			{
				g_iap.flag_RX_ = 1;	
				g_iap.APP_Bin_len = temp_CAN_Msg.Data[5]<<16|temp_CAN_Msg.Data[6]<<8|temp_CAN_Msg.Data[7];//��ȡAPP�ļ��Ĵ�С		
				Send_response(MCU2PC_IAP,0x81,0,0,0);	//������λ�����¹̼�׼������
				
			}
		}			
		else if(g_iap.flag_RX_ == 1) //1:���¹̼�
		{
			g_iap.TIME_jian = iap_time;
			for(i=0;i<temp_CAN_Msg.DLC;i++)
			{
				g_iap.RX_BUFF[g_iap.RX_data_len] = temp_CAN_Msg.Data[i];
				g_iap.RX_data_len++;	
				g_iap.APP_RX_len++;			

			}

					
		}			
	}
}


void IAP_CAN__Init_Transformation(void)
{
	u8 temp[8]={0};
	
	while(1)
	{
		//Ĭ�϶˿ڳ�ʼ��CAN
		CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal,0);		//Ĭ��,������500Kbps 
		if(IAP_Can_Send_Msg(1,temp,8))		//����
		{
			CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal,1);	//��ӳ��,������500Kbps 
			if(!IAP_Can_Send_Msg(1,temp,8))	
			{
				break;						//��ȷ����
			}
		}	
		else
		{
			break;							//��ȷ����
		}
	}

}




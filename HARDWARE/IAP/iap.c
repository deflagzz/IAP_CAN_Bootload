#include "iap.h"

//获取芯片唯一ID
u32 g_CpuID[3];
u32 g_Lock_Code;	


stm32_iap g_iap;

iapfun jump2app; 
u16 iapbuf[1024];  

//appxaddr:应用程序的起始地址
//appbuf:应用程序CODE.
//appsize:应用程序大小(字节).
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize)
{
	u16 t;
	u16 i=0;
	u16 temp;
	u32 fwaddr=appxaddr;//当前写入的地址
	u8 *dfu=appbuf;
	for(t=0;t<appsize;t+=2)
	{						    
		temp=(u16)dfu[1]<<8;
		temp+=(u16)dfu[0];	  
		dfu+=2;//偏移2个字节
		iapbuf[i++]=temp;	    
		if(i==1024)
		{
			i=0;
			STMFLASH_Write(fwaddr,iapbuf,1024);	
			fwaddr+=2048;//偏移2048  16=2*8.所以要乘以2.
		}
	}
	if(i)STMFLASH_Write(fwaddr,iapbuf,i);//将最后的一些内容字节写进去.  
}

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
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
//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
static u8 IAP_Can_Send_Msg(u8 ID,u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId = ID;			// 标准标识符  
	TxMessage.ExtId = ID;			// 设置扩展标示符 
	TxMessage.IDE=CAN_Id_Standard; 	// 标准帧
	TxMessage.RTR=CAN_RTR_Data;		// 数据帧
	TxMessage.DLC=len;				// 要发送的数据长度
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];			          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)!=CAN_TxStatus_Ok)&&(i<0XFFF))
		i++;	//等待发送结束
	if(i>=0XFFF)
		return 1;
	return 0;	 
}

//输入CAN总线接收的一帧数据
u8 check_data(u8 *pBuf ,u16 pBufSize)	//1:成功 0:错误 
{
	u32 get_PC_crcVel=0;
	u8 *pbuff;
	
	pbuff = pBuf+pBufSize-4;
	g_iap.CRC_PC_vel = pbuff[0]<<24|pbuff[1]<<16|pbuff[2]<<8|pbuff[3];	//接收的CRC32
	
	get_PC_crcVel 	 = CRC32Calculate(pBuf,pBufSize-4);	//MCU端计算的CRC32
	
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
	设备类型：01:主板
			  02:磁导航-8 
			  03:磁导航-16
			  04:
			  05：
			  06:RFID 地标
			  07:超声波避障
2.RunMode
	运行模式：01:BootLoader 
			  02:APP*/
void Send_response(u8 Can_ID,u8 Byte1,u8 Byte6,u8 Byte7,u8 Byte8)	//发送响应
{
	u8 temp[8]={0};

	GetLockCode(&g_Lock_Code);			//获取芯片唯一ID
		
	temp[0] = Byte1;					//命令字
	temp[1] = g_Lock_Code>>24&0xff;		//唯一ID
	temp[2] = g_Lock_Code>>16&0xff;	
	temp[3] = g_Lock_Code>>8&0xff;
	temp[4] = g_Lock_Code&0xff;
	temp[5] = Byte6;					//CAN_ID	//刷固件模式:接收数据长度3个字节
	temp[6] = Byte7;					//设备类型
	temp[7] = Byte8;					//运行模式 01:BootLoader  02:APP

	IAP_Can_Send_Msg(Can_ID,temp,8);
}


void IAP_BootLoad_Init(void)	//上电等待更新固件的延时
{
	u8 led_num=0;
	CRC32TableCreate();				//CRC校验的表
	Send_response(MCU2PC_Rend_device_info,0x81,0,0,1);	//告诉上位机设备信息
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
	11:接收成功
	1:校验和错误 
	2:接收数据不足1K
	3:FLAHSH内程序出错	*/
void IAP_BootLoad_UpData(void)
{
	if(g_iap.flag_RX_ == 1)			//1:更新固件
	{
		if(g_iap.RX_data_len)
		{	
			if(g_iap.APP_RX_len == g_iap.APP_Bin_len+IAP_CRC_byte_len)			//所以的Bin文件接收完毕
			{
				g_iap.APP_RX_len = g_iap.APP_RX_len - IAP_CRC_byte_len;			//减去检验和的4个字节
				//检测校验和是否正确
				if(check_data(g_iap.RX_BUFF,g_iap.RX_data_len))//1:成功	
				{
					g_iap.flag_Recive_Bin = 11;	
				}
				else
				{
					g_iap.flag_Recive_Bin = 1;					//1:校验和错误
				}
			}
			else
			{
				if(g_iap.RX_data_len == iap_start_updata_LEN)	//接收够1K数据	
				{
					g_iap.APP_RX_len = g_iap.APP_RX_len - IAP_CRC_byte_len;	//减去检验和的4个字节
					//检测校验和是否正确
					if(check_data(g_iap.RX_BUFF,g_iap.RX_data_len))//1:成功	
					{
						g_iap.flag_Recive_Bin = 11;					
					}
					else
					{
						g_iap.flag_Recive_Bin = 1;			//1:校验和错误 
					}
				}
				else											
				{
					if(!g_iap.TIME_jian)	
					{					
						g_iap.flag_Recive_Bin = 2;	 		//2:接收数据不足1K			
					}
				}													
			}				
			if(g_iap.flag_Recive_Bin == 11)					//接收数据帧成功 
			{
				g_iap.Flash_write_address = FLASH_APP1_ADDR+g_iap.APP_RX_len-(g_iap.RX_data_len-IAP_CRC_byte_len);
				//更新FLASH代码 
				iap_write_appbin(g_iap.Flash_write_address,g_iap.RX_BUFF,g_iap.RX_data_len-IAP_CRC_byte_len);
				g_iap.RX_data_len 	   = 0;					//清零数据缓存指针
				g_iap.flag_Recive_Bin  = 0;					//清零标志
				if(g_iap.APP_RX_len == g_iap.APP_Bin_len)
				{
					g_iap.OK_Recive        = 1;					//接收完毕Bin文件
				}
				//告诉上位机接收固件长度
				Send_response(MCU2PC_IAP,0x82,g_iap.APP_RX_len>>16&0xff,g_iap.APP_RX_len>>8&0xff,g_iap.APP_RX_len&0xff);				
			}
			else if(g_iap.flag_Recive_Bin == 1 || g_iap.flag_Recive_Bin == 2)	//接收数据帧成功 
			{
				//错误代码0x83,02:接收少于1024,告诉上位机接收当前帧固件长度
				Send_response(MCU2PC_IAP,0x83,g_iap.flag_Recive_Bin,(g_iap.RX_data_len-IAP_CRC_byte_len)>>8&0xff,(g_iap.RX_data_len-IAP_CRC_byte_len)&0xff);
				//软复位
				systeam_ReStart();				
			}	
			//
			//固件跳转
			//			
			if(g_iap.OK_Recive)
			{
				delay_ms(100);
				if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
				{	 						
					g_iap.flag_Recive_Bin = 12;	
					Send_response(MCU2PC_IAP,0x84,1,0,0);	//1:成功 //2:失败
					iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
					
				}
				else //非FLASH应用程序,无法执行
				{
					g_iap.RX_data_len = 0;
					g_iap.flag_RX_    = 0;	//操作完成						
					g_iap.flag_Recive_Bin = 3;	
					Send_response(MCU2PC_IAP,0x84,2,0,0);	//1:成功 //2:失败				
					//软复位
					systeam_ReStart();					
				}				
			}		
		}
	}
	else
	{
		if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
		{	 
			g_iap.flag_Recive_Bin = 12;	
			iap_load_app(FLASH_APP1_ADDR);	//执行FLASH APP代码	
		}
		else 									
		{
			g_iap.flag_Recive_Bin = 3;		//非FLASH应用程序,无法执行
		}				
	}
	
	if(g_iap.TIME_jian)
	{
		g_iap.TIME_jian--;
	}
	delay_ms(10);

}

//BootLoad程序CAN中断内
void IAP_BootLoad_CAN_RX(CanRxMsg temp_CAN_Msg)
{
	int i=0;
	
	if(temp_CAN_Msg.StdId == PC2MCU_Rend_device_info)	//PC读设备唯一ID
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
			GetLockCode(&g_Lock_Code);					//获取芯片唯一ID

			if(	temp_CAN_Msg.Data[0]==0x01&&	
				temp_CAN_Msg.Data[1]==(g_Lock_Code>>24&0xff)&& 
				temp_CAN_Msg.Data[2]==(g_Lock_Code>>16&0xff)&&
				temp_CAN_Msg.Data[3]==(g_Lock_Code>>8&0xff)&&	
				temp_CAN_Msg.Data[4]==(g_Lock_Code&0xff) )
			{
				g_iap.flag_RX_ = 1;	
				g_iap.APP_Bin_len = temp_CAN_Msg.Data[5]<<16|temp_CAN_Msg.Data[6]<<8|temp_CAN_Msg.Data[7];//获取APP文件的大小		
				Send_response(MCU2PC_IAP,0x81,0,0,0);	//告诉上位机更新固件准备就绪
				
			}
		}			
		else if(g_iap.flag_RX_ == 1) //1:更新固件
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
		//默认端口初始化CAN
		CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal,0);		//默认,波特率500Kbps 
		if(IAP_Can_Send_Msg(1,temp,8))		//出错
		{
			CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal,1);	//重映射,波特率500Kbps 
			if(!IAP_Can_Send_Msg(1,temp,8))	
			{
				break;						//正确跳出
			}
		}	
		else
		{
			break;							//正确跳出
		}
	}

}




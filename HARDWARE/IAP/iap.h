#ifndef __IAP_H__
#define __IAP_H__
#include "stm32f10x.h" 
#include "crc.h" 
	
#define IAP_CRC_byte_len 		4						//每帧数据的CRC32校验字节数
#define STM32_SRAM_SIZE 		20 	 					//所选STM32的SRAM容量大小(单位为K) 
#define IAP_Bootloat_SIZE 		0x2000	//8K
#define FLASH_APP1_ADDR			(0x08000000+IAP_Bootloat_SIZE) 	//+8*1024	//第一个应用程序起始地址(存放在FLASH)   
#define CAN_RX_BUFF_LEN			1024+IAP_CRC_byte_len			//1K或者2KSRAM缓存CAN中断的数据,根据MCU的FLASH大小决定	

#define iap_time 				100						//10ms为单位
#define iap_wait_updata_time 	100						//1ms为单位
#define iap_start_updata_LEN 	CAN_RX_BUFF_LEN

#define PC2MCU_Rend_device_info 0x7D
#define MCU2PC_Rend_device_info 0x7B

#define PC2MCU_IAP				0x7F
#define MCU2PC_IAP 				0x7E


#define  g_STM32_96_ID 			0x17AADB73	//可做加密处理





typedef  void (*iapfun)(void);							//定义一个函数类型的参数.  

typedef struct
{
	
	u8  RX_BUFF[CAN_RX_BUFF_LEN];	
	u16 RX_data_len;			//每帧数据的长度
	u8  flag_RX_;
	u8  flag_TX_;
	u8  password;
	u16 TIME_jian;
	u8  RX_err;
	u16 APP_RX_len;				//接收的bin文件总长度
	u32 APP_Bin_len;			//APP的Bin文件的总大小
	u32 CRC_PC_vel;
	u8  CRC_buff[4];
	u8  flag_Recive_Bin;
	u8	OK_Recive;
	u32 Flash_write_address;	
	u16  CAN_PIN_Remap;			//CAN引脚映射标志存入bootload的flash最后一个字节

}stm32_iap;


extern stm32_iap g_iap;

extern u32 g_CpuID[3];
extern u32 g_Lock_Code;






void IAP_BootLoad_Init(void);
void IAP_BootLoad_UpData(void);

//BootLoad程序CAN中断内
void IAP_BootLoad_CAN_RX(CanRxMsg temp_CAN_Msg);
void IAP_CAN_Remap_Init(void);




#endif








































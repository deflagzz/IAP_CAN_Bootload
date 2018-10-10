#ifndef __IAP_H__
#define __IAP_H__
#include "stm32f10x.h" 
#include "crc.h" 
	
#define IAP_CRC_byte_len 		4						//ÿ֡���ݵ�CRC32У���ֽ���
#define STM32_SRAM_SIZE 		20 	 					//��ѡSTM32��SRAM������С(��λΪK) 
#define IAP_Bootloat_SIZE 		0x2000	//8K
#define FLASH_APP1_ADDR			(0x08000000+IAP_Bootloat_SIZE) 	//+8*1024	//��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)   
#define CAN_RX_BUFF_LEN			1024+IAP_CRC_byte_len			//1K����2KSRAM����CAN�жϵ�����,����MCU��FLASH��С����	

#define iap_time 				100						//10msΪ��λ
#define iap_wait_updata_time 	100						//1msΪ��λ
#define iap_start_updata_LEN 	CAN_RX_BUFF_LEN

#define PC2MCU_Rend_device_info 0x7D
#define MCU2PC_Rend_device_info 0x7B

#define PC2MCU_IAP				0x7F
#define MCU2PC_IAP 				0x7E


#define  g_STM32_96_ID 			0x17AADB73	//�������ܴ���





typedef  void (*iapfun)(void);							//����һ���������͵Ĳ���.  

typedef struct
{
	
	u8  RX_BUFF[CAN_RX_BUFF_LEN];	
	u16 RX_data_len;			//ÿ֡���ݵĳ���
	u8  flag_RX_;
	u8  flag_TX_;
	u8  password;
	u16 TIME_jian;
	u8  RX_err;
	u16 APP_RX_len;				//���յ�bin�ļ��ܳ���
	u32 APP_Bin_len;			//APP��Bin�ļ����ܴ�С
	u32 CRC_PC_vel;
	u8  CRC_buff[4];
	u8  flag_Recive_Bin;
	u8	OK_Recive;
	u32 Flash_write_address;	
	u16  CAN_PIN_Remap;			//CAN����ӳ���־����bootload��flash���һ���ֽ�

}stm32_iap;


extern stm32_iap g_iap;

extern u32 g_CpuID[3];
extern u32 g_Lock_Code;






void IAP_BootLoad_Init(void);
void IAP_BootLoad_UpData(void);

//BootLoad����CAN�ж���
void IAP_BootLoad_CAN_RX(CanRxMsg temp_CAN_Msg);
void IAP_CAN_Remap_Init(void);




#endif








































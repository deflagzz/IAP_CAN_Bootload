#ifndef __CAN_H
#define __CAN_H	 
#include "my_include.h"	    
 	 

//CAN����RX0�ж�ʹ��
#define CAN_RX0_INT_ENABLE	1							//0,��ʹ��;1,ʹ��.	

extern  u8 g_UT_CAN_ID ;

							 							 				    
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode,u8 Remap_type);

u8 Can_Send_Msg(u8 ID,u8* msg,u8 len);						//��������

u8 Can_Receive_Msg(u8 *buf);							//��������







#endif


















#ifndef __CAN_H
#define __CAN_H	 
#include "my_include.h"	    
 	 

//CAN接收RX0中断使能
#define CAN_RX0_INT_ENABLE	1							//0,不使能;1,使能.	

extern  u8 g_UT_CAN_ID ;

							 							 				    
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode,u8 Remap_type);

u8 Can_Send_Msg(u8 ID,u8* msg,u8 len);						//发送数据

u8 Can_Receive_Msg(u8 *buf);							//接收数据







#endif


















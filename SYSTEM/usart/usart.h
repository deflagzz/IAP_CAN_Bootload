#ifndef __USART_H
#define __USART_H

#include "my_include.h"	



#define USART_REC_LEN  			512  	//定义最大接收字节数 512

#define EN_USART1_RX 			0		//使能（1）/禁止（0）串口1接收
	  	
extern char  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
//extern u16 USART_RX_STA;         		//接收状态标记	
//extern u8 cont;


extern u8 usart_rx_flag ;
extern u16 usart_recv_len;

//如果想串口中断接收，请不要注释以下宏定义
void usart_init(u32 bound);
#endif



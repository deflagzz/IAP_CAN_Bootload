#ifndef __USART_H
#define __USART_H

#include "my_include.h"	



#define USART_REC_LEN  			512  	//�����������ֽ��� 512

#define EN_USART1_RX 			0		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern char  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
//extern u16 USART_RX_STA;         		//����״̬���	
//extern u8 cont;


extern u8 usart_rx_flag ;
extern u16 usart_recv_len;

//����봮���жϽ��գ��벻Ҫע�����º궨��
void usart_init(u32 bound);
#endif



#include "TCP_user.h"
#include "stm32f4xx.h"
#include "usr_FreeRTOS.h"

void TcpInit(void)
{
  
}

void TcpNetTest(void)
{
  int32_t iCount;
	uint8_t *sendbuf;
	uint8_t tcp_status;
	uint16_t maxlen;
	uint8_t res;
	const TickType_t xTicksToWait = 2; /* ?��3��2ms */
	EventBits_t uxBits;
	
	/* 
	   ���?��TCP Socket2����?��?����y��??��?��??��??����t???��o����?10???��?T��y?Y����D??????a��??��?��
	   ��?��?��������?a��?��1?����?TCP_TYPE_KEEP_ALIVE��??����??������3?��??����?2?����10??��?����???T???��
	*/
    socket_tcp = tcp_get_socket (TCP_TYPE_SERVER|TCP_TYPE_KEEP_ALIVE, 0, 10, tcp_callback);
	if(socket_tcp != 0)
	{
		res = tcp_listen (socket_tcp, PORT_NUM);
		printf_debug("tcp listen res = %d\r\n", res);
	}
	
	while (1) 
	{
		/* RL-TCPnet��|����o����y */
		main_TcpNet();
		
		/* ��?������???2?��?��?��|���� */
		tcp_status = TCP_StatusCheck();

		/* �̨���y?����D��???���騤�䨺??t����?? */
		uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* ��??t����??����??���� */
							         0xFFFF,       		 /* �̨���y0xFFFF?3��???��?����?? */
							         pdTRUE,             /* ��?3??��0xFFFF??��???3y��??a��?��?��?��a0xFFFF??��?����???��?�㨪?3??��*/
							         pdFALSE,            /* ����???apdTRUE������?�̨���y0xFFFF��?��a??��?����??*/
							         xTicksToWait); 	 /* �̨���y?��3������?? */

		if((uxBits != 0)&&(tcp_status == __TRUE))
		{
			switch (uxBits)
			{
				/* ?����?��?K1?�����??��?????3��TCP?��?��??����?��8��??����y?Y */
				case KEY1_BIT0:			  
					printf_debug("tcp_get_state(socket_tcp) = %d\r\n", tcp_get_state(socket_tcp));
					iCount = 8;
					do
					{
						main_TcpNet();
						if (tcp_check_send (socket_tcp) == __TRUE) 
						{
							maxlen = tcp_max_dsize (socket_tcp);
							iCount -= maxlen;
							
							if(iCount < 0)
							{
								/* ?a?��???????����a��? */
								maxlen = iCount + maxlen;
							}
							
							sendbuf = tcp_get_buf(maxlen);
							sendbuf[0] = '1';
							sendbuf[1] = '2';
							sendbuf[2] = '3';
							sendbuf[3] = '4';
							sendbuf[4] = '5';
							sendbuf[5] = '6';
							sendbuf[6] = '7';
							sendbuf[7] = '8';
							
							/* 2a��?����?????����1��???��?��??����? */
							tcp_send (socket_tcp, sendbuf, maxlen);
						}
						
					}while(iCount > 0);
					break;

				/* ?����?��?K2?�����??��?????3��TCP?��?��??����?��1024��??����?��y?Y */
				case KEY2_BIT1:		
					printf_debug("tcp_get_state(socket_tcp) = %d\r\n", tcp_get_state(socket_tcp));
					iCount = 1024;
					do
					{
						main_TcpNet();
						if (tcp_check_send (socket_tcp) == __TRUE) 
						{
							maxlen = tcp_max_dsize (socket_tcp);
							iCount -= maxlen;
							
							if(iCount < 0)
							{
								/* ?a?��???????����a��? */
								maxlen = iCount + maxlen;
							}
							
							/* ?a��???3?��??����???��??������?����y?Y�㨹��??��8??��??�� */
							sendbuf = tcp_get_buf(maxlen);
							sendbuf[0] = 'a';
							sendbuf[1] = 'b';
							sendbuf[2] = 'c';
							sendbuf[3] = 'd';
							sendbuf[4] = 'e';
							sendbuf[5] = 'f';
							sendbuf[6] = 'g';
							sendbuf[7] = 'h';
							
							/* 2a��?����?????����1��???��?��??����? */
							tcp_send (socket_tcp, sendbuf, maxlen);
						}
						
					}while(iCount > 0);					
					break;
					
				/* ?����?��?K3?�����??��?????3��TCP?��?��??����?��5MB��y?Y */
				case KEY3_BIT2:			  
					printf_debug("tcp_get_state(socket_tcp) = %d\r\n", tcp_get_state(socket_tcp));
					iCount = 5*1024*1024;
					do
					{
						main_TcpNet();
						if (tcp_check_send (socket_tcp) == __TRUE) 
						{
							maxlen = tcp_max_dsize (socket_tcp);
							iCount -= maxlen;
							
							if(iCount < 0)
							{
								/* ?a?��???????����a��? */
								maxlen = iCount + maxlen;
							}
							
							/* ?a��???3?��??����???��??������?����y?Y�㨹��??��8??��??�� */
							sendbuf = tcp_get_buf(maxlen);
							sendbuf[0] = 'a';
							sendbuf[1] = 'b';
							sendbuf[2] = 'c';
							sendbuf[3] = 'd';
							sendbuf[4] = 'e';
							sendbuf[5] = 'f';
							sendbuf[6] = 'g';
							sendbuf[7] = 'h';
							
							/* 2a��?����?????����1��???��?��??����? */
							tcp_send (socket_tcp, sendbuf, maxlen);
						}
						
					}while(iCount > 0);
					break;
				
				 /* ????��??��?��2?��|���� */
				default:                     
					break;
			}
		}
	}
}

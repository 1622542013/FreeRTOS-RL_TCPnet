#include "TCP_user.h"
#include "stm32f4xx.h"
#include "usr_FreeRTOS.h"
#include "usart.h"

uint8_t socket_tcp;
#define PORT_NUM       1001    /* TCP�����������˿ں� */

void TcpInit(void)
{
  
}

U16 tcp_callback (U8 soc, U8 evt, U8 *ptr, U16 par)
{
	char buf[100];
	uint16_t i;
  uint32_t num;
	
	/* ȷ����socket_tcp�Ļص� */
	if (soc != socket_tcp) 
	{
		return (0);
	}

	switch (evt) 
	{
		/*
			Զ�̿ͻ���������Ϣ
		    1������ptr�洢Զ���豸��IP��ַ��par�д洢�˿ںš�
		    2��������ֵ1�������ӣ�������ֵ0��ֹ���ӡ�
		*/
		case TCP_EVT_CONREQ:
			num = snprintf(buf,100,"Զ�̿ͻ�����������IP: %d.%d.%d.%d\r\n�˿�port:%d\r\n", ptr[0], ptr[1], ptr[2], ptr[3],par);
    
      USART_SetSendData(USART1, buf, num);
			return (1);
		
		/* ������ֹ */
		case TCP_EVT_ABORT:
			break;
		
		/* SocketԶ�������Ѿ����� */
		case TCP_EVT_CONNECT:
      
      num = snprintf(buf,100,"Socket is connected to remote peer\r\n");
			USART_SetSendData(USART1, buf, num);
			break;
		
		/* ���ӶϿ� */
		case TCP_EVT_CLOSE:     
      num = snprintf(buf,100,"Connection has been closed\r\n");
			USART_SetSendData(USART1, buf, num);
			break;
		
		/* ���͵������յ�Զ���豸Ӧ�� */
		case TCP_EVT_ACK:
			break;
		
		/* ���յ�TCP����֡��ptrָ�����ݵ�ַ��par��¼���ݳ��ȣ���λ�ֽ� */
		case TCP_EVT_DATA:
      num = snprintf(buf,100,"Data length = %d\r\n", par);
			USART_SetSendData(USART1, buf, num);  
    
			for(i = 0; i < par; i++)
			{
        num = snprintf(buf,100,"ptr[%d] = %d\r\n", i, ptr[i]);
        USART_SetSendData(USART1, buf, num); 
			}
			break;
	}
	return (0);
}

uint8_t TCP_StatusCheck(void) 
{
	uint8_t res;
	
	switch (tcp_get_state(socket_tcp)) 
	{
		case TCP_STATE_FREE:
		case TCP_STATE_CLOSED:
			res = tcp_listen (socket_tcp, PORT_NUM);
      char buf[50];
      uint32_t num;
      num = snprintf(buf,100,"tcp listen res = %d\r\n", res);
      USART_SetSendData(USART1, buf, num); 
			break;
		
		case TCP_STATE_LISTEN:
			break;
		
		case TCP_STATE_CONNECT:
			return (__TRUE);
			
		default:  
			break;
	}
	
	return (__FALSE);
}

void TcpNetTest(void)
{
  char buf[100];
  uint32_t num;
  
	uint8_t tcp_status;
	uint16_t maxlen;
	uint8_t res;
	const TickType_t xTicksToWait = 2; 
	
  socket_tcp = tcp_get_socket (TCP_TYPE_SERVER|TCP_TYPE_KEEP_ALIVE, 0, 10, tcp_callback);
  
	if(socket_tcp != 0)
	{
		res = tcp_listen (socket_tcp, PORT_NUM);   
    num = snprintf(buf,100,"tcp listen res = %d\r\n", res);
    USART_SetSendData(USART1, buf, num); 
	}
	
	while (1) 
	{
		/* RL-TCPnet������ */
		main_TcpNet();
		
////		/* �������߲�εĴ��� */
//		tcp_status = TCP_StatusCheck();

	}
}

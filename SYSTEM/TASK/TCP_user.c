#include "TCP_user.h"
#include "stm32f4xx.h"
#include "usr_FreeRTOS.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

uint8_t socket_tcp;
#define PORT_NUM       1001    /* TCP�����������˿ں� */

void TcpInit(void)
{
  
}

U16 tcp_callback (U8 soc, U8 evt, U8 *ptr, U16 par)
{
	uint16_t i;
	
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
      u1_printf("Զ�̿ͻ�����������IP: %d.%d.%d.%d\r\n�˿�port:%d\r\n", ptr[0], ptr[1], ptr[2], ptr[3],par);
			return (1);
		
		/* ������ֹ */
		case TCP_EVT_ABORT:
			break;
		
		/* SocketԶ�������Ѿ����� */
		case TCP_EVT_CONNECT:
      u1_printf("SocketԶ�������Ѿ�����\r\n");
			break;
		
		/* ���ӶϿ� */
		case TCP_EVT_CLOSE: 
      u1_printf("���ӶϿ�\r\n");      
			break;
		
		/* ���͵������յ�Զ���豸Ӧ�� */
		case TCP_EVT_ACK:
			break;
		
		/* ���յ�TCP����֡��ptrָ�����ݵ�ַ��par��¼���ݳ��ȣ���λ�ֽ� */
		case TCP_EVT_DATA:
      u1_printf("���յ�TCP����֡��ptrָ�����ݵ�ַ�����ݳ��� = %d\r\n", par);
          
			for(i = 0; i < par; i++)
			{
        u1_printf("ptr[%d] = %d\r\n", i, ptr[i]);
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
      u1_printf("tcp listen res = %d\r\n", res);
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
uint32_t send_size = 0;
void TcpNetTest(void)
{
	uint8_t res = 0;
  uint32_t maxlen = 0;
  
  uint8_t* sendbuf;
  static uint32_t count = 0;
  
  socket_tcp = tcp_get_socket (TCP_TYPE_SERVER|TCP_TYPE_KEEP_ALIVE, 0, 10, tcp_callback);
  
	if(socket_tcp != 0)
	{
		res = tcp_listen (socket_tcp, PORT_NUM); 
    if(res == __TRUE)
    {
      u1_printf("tcp listen �����ɹ�\r\n");   
    }
    else
    {
      u1_printf("tcp listen ����ʧ��\r\n");   
    } 
	}
	
	while (1) 
	{
		/* RL-TCPnet������ */
		main_TcpNet();
    
    if (tcp_check_send (socket_tcp) == __TRUE) /*�ж��Ƿ��ܷ���*/
    {
      maxlen = tcp_max_dsize (socket_tcp);
      send_size = strlen("tcp���ⷢ������ �� %d\r\n");
      
      if(send_size > maxlen)
      {
        send_size = maxlen;
      }
      
      sendbuf = tcp_get_buf(send_size);
      
      send_size = snprintf((char*)sendbuf,100,"tcp���ⷢ������ �� %d\r\n",count);
      count++;
      
      /* ���Է���ֻ��ʹ�û�ȡ���ڴ� */
      tcp_send (socket_tcp, sendbuf, send_size);
      
      TickType_t xLastSYSTime;
      vTaskDelay(1000);
    }
	}
}

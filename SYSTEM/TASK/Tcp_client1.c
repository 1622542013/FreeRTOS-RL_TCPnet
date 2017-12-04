#include "TCP_user.h"

#define IP1 192
#define IP2 168
#define IP3 1
#define IP4 2

#define PORT_NUM 1001

/* ����Ǳ��ض˿� */
#define LocalPort_NUM 1024

void TcpClientTest1(void)
{
	uint8_t res = 0;
  uint32_t maxlen = 0;
  uint32_t send_size = 0;
  uint8_t* sendbuf;
  static uint32_t count = 0;

  uint8_t Rem_IP[4] = {IP1,IP2,IP3,IP4};/*Զ��ip*/
  uint8_t socket_tcp_fp;/*socket ���*/
  
                                 /* tcp �ͻ���        һֱ���� */
  socket_tcp_fp = tcp_get_socket (TCP_TYPE_CLIENT|TCP_TYPE_KEEP_ALIVE, 0, 1, tcpserver_callback);
  
	if(socket_tcp_fp != 0)
	{
    /*����socket tcp���ӣ� ip��ַ��Զ�̶˿ںţ��Լ��˿ں�*/
    res = tcp_connect (socket_tcp_fp, Rem_IP, PORT_NUM, LocalPort_NUM);
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
		/* RL-TCPnet������(��Ҫһֱ���ã����ò�������) */
		main_TcpNet();
    
    /*��� tcp �ͻ�������״̬*/
    TCP_Client_StatusCheck(socket_tcp_fp,Rem_IP,PORT_NUM,LocalPort_NUM);
    
    if (tcp_check_send (socket_tcp_fp) == __TRUE) /*�ж��Ƿ��ܷ���*/
    {
      main_TcpNet();
      maxlen = tcp_max_dsize (socket_tcp_fp);/*��ȡ����ܷ����ֽ�*/
      send_size = strlen("tcp���ⷢ������ �� %d\r\n");
      
      if(send_size > maxlen)
      {
        send_size = maxlen;
      }
      
      sendbuf = tcp_get_buf(send_size);
      
      send_size = snprintf((char*)sendbuf,100,"�ͻ���1 ���ⷢ������ 1111�� %d\r\n",count);
      count++;
      
      /* ���Է���ֻ��ʹ�û�ȡ���ڴ� */
      tcp_send (socket_tcp_fp, sendbuf, send_size);     
    }
    vTaskDelay(1000);
	}
}

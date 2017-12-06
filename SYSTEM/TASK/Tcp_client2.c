#include "TCP_user.h"

#define IP1 192
#define IP2 168
#define IP3 1
#define IP4 2

#define RemotePort_NUM 1001

/* ����Ǳ��ض˿� */
#define LocalPort_NUM 1025

TCP_status TCP_c2;

void TcpClientTest2(void)
{
	uint8_t socket_status = 0;
  uint8_t connect_status = 0;
  
  uint32_t maxsend_size = 0;
  int32_t send_size = 0;
  
  uint8_t* sendbuf;
  static uint32_t count = 0;
  
  TCP_c2.mode = TCP_CLIENT;/*tcp ģʽ*/
  
  TCP_c2.client.ip[0] = IP1;/*Զ��ip*/
  TCP_c2.client.ip[1] = IP2;/*Զ��ip*/
  TCP_c2.client.ip[2] = IP3;/*Զ��ip*/
  TCP_c2.client.ip[3] = IP4;/*Զ��ip*/

  TCP_c2.client.local_port = LocalPort_NUM;
  TCP_c2.client.remote_port = RemotePort_NUM;
  
  /* tcp  socket ��� */                  /* tcp �ͻ���        һֱ���� */
  TCP_c2.socket_fp = tcp_get_socket (TCP_TYPE_CLIENT|TCP_TYPE_KEEP_ALIVE, 0, 1, tcp_callback);
  
	if(TCP_c2.socket_fp != 0)
	{
    /*����socket tcp���ӣ� ip��ַ��Զ�̶˿ںţ��Լ��˿ں�*/
    socket_status = tcp_connect (TCP_c2.socket_fp, TCP_c2.client.ip, TCP_c2.client.remote_port, TCP_c2.client.local_port);
    if(socket_status == __TRUE)
    {
      u1_printf("tcp socket �����ɹ�\r\n");   
    }
    else
    {
      u1_printf("tcp socket ����ʧ��\r\n");   
    } 
	}
	
	while (1) 
	{
		/* RL-TCPnet������(��Ҫһֱ���ã����ò�������) */
		main_TcpNet();
    
    /*��� tcp �ͻ�������״̬*/
    connect_status = TCP_StatusCheck(TCP_c2);
    
    if((tcp_check_send (TCP_c2.socket_fp) == __TRUE)&&(connect_status == TCP_STATE_CONNECT)) /*�ж��Ƿ��ܷ���*/
    {
      main_TcpNet();
         
      uint8_t sendbuf_temp[100];
      uint32_t send_pt = 0;
      
      send_size = snprintf((char*)sendbuf_temp,100,"�ͻ���2 ���ⷢ������--2222--�� %d\r\n",count);
      count++;
      
      do
      {
        maxsend_size = tcp_max_dsize (TCP_c2.socket_fp);/*��ȡ����ܷ����ֽ�*/
        
        send_size -= maxsend_size;
        
        if(send_size > 0)/*���ͳ��ȣ���������ܷ��ͳ���*/
        {
          sendbuf = tcp_get_buf(maxsend_size); 

          memcpy(sendbuf,sendbuf_temp + send_pt,maxsend_size);
          send_pt += maxsend_size;
          
          tcp_send (TCP_c2.socket_fp, sendbuf, maxsend_size);/* ���Է���ֻ��ʹ�û�ȡ���ڴ� */ 
        }
        else/*���ͳ��ȣ�С������ܷ��ͳ���*/
        {
          sendbuf = tcp_get_buf(send_size + maxsend_size);
          
          memcpy(sendbuf,sendbuf_temp + send_pt,send_size + maxsend_size);
          
          tcp_send (TCP_c2.socket_fp, sendbuf, send_size + maxsend_size);/* ���Է���ֻ��ʹ�û�ȡ���ڴ� */ 
        } 
      }
      while(send_size > 0);/*����δ�������ݣ���������*/
      
    }
    vTaskDelay(1000);
	}
}

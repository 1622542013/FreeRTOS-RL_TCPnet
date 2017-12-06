#include "TCP_user.h"

#define PORT_NUM       1001    /* TCP�����������˿ں� */

TCP_status TCP_S;

void TcpServer(void)
{
  uint32_t maxsend_size = 0;
  int32_t send_size = 0;
  
  uint8_t* sendbuf;
  static uint32_t count = 0;
  
  uint8_t connect_status = 0;
  uint8_t socket_status = 0;
  
  TCP_S.mode = TCP_SERVER;
  TCP_S.server.local_port = PORT_NUM;
  
  TCP_S.socket_fp = tcp_get_socket (TCP_TYPE_SERVER | TCP_TYPE_KEEP_ALIVE, 0, 10, tcp_callback);
  
  if(TCP_S.socket_fp != 0)
  {
      socket_status = tcp_listen (TCP_S.socket_fp, PORT_NUM);
    
      if(socket_status == __TRUE)
      {
        u1_printf("tcp ���� �ɹ�\r\n");
      }
      else
      {
        u1_printf("tcp ���� ʧ��\r\n");
      }     
  }
  
  while (1) 
	{
		/* RL-TCPnet������(��Ҫһֱ���ã����ò�������) */
		main_TcpNet();
    
    /*��� tcp �ͻ�������״̬*/
    connect_status = TCP_StatusCheck(TCP_S);
    
    if((tcp_check_send (TCP_S.socket_fp) == __TRUE)&&(connect_status == TCP_STATE_CONNECT)) /*�ж��Ƿ��ܷ���*/
    {
      main_TcpNet();
      
      uint8_t sendbuf_temp[100];
      uint32_t send_pt = 0;
      
      send_size = snprintf((char*)sendbuf_temp,100,"�ͻ���1 ���ⷢ������--1111--�� %d\r\n",count);
      count++;
      
      do
      {
        maxsend_size = tcp_max_dsize (TCP_S.socket_fp);/*��ȡ����ܷ����ֽ�*/
        
        send_size -= maxsend_size;
        
        if(send_size > 0)/*���ͳ��ȣ���������ܷ��ͳ���*/
        {
          sendbuf = tcp_get_buf(maxsend_size); 

          memcpy(sendbuf,sendbuf_temp + send_pt,maxsend_size);
          send_pt += maxsend_size;
          
          tcp_send (TCP_S.socket_fp, sendbuf, maxsend_size);/* ���Է���ֻ��ʹ�û�ȡ���ڴ� */ 
        }
        else/*���ͳ��ȣ�С������ܷ��ͳ���*/
        {
          sendbuf = tcp_get_buf(send_size + maxsend_size);
          
          memcpy(sendbuf,sendbuf_temp + send_pt,send_size + maxsend_size);
          
          tcp_send (TCP_S.socket_fp, sendbuf, send_size + maxsend_size);/* ���Է���ֻ��ʹ�û�ȡ���ڴ� */ 
        } 
      }
      while(send_size > 0);/*����δ�������ݣ���������*/
    }
      
    vTaskDelay(1000);
	}
}

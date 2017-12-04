#include "TCP_user.h"

/*TCP server socket �ص�����
����1��TCP socket ���
����2���¼�����
    TCP_EVT_CONREQ   Զ�̿ͻ���������Ϣ��
    TCP_EVT_CONNECT  ���ӳɹ���Ϣ��
    TCP_EVT_CLOSE    ���ӶϿ���Ϣ��
    TCP_EVT_ABORT    ������ֹ��Ϣ��
    TCP_EVT_ACK      �������ݺ��յ�Զ��������Ӧ��
    TCP_EVT_DATA     ���յ� TCP ���ݰ���
����3��
����4��
    ������2������������ TCP_EVT_DATA��
        ����3 ptr ָ����ǽ��յ������ݵ�ַ
        ����4 par �ǽ��յ����ݵĸ���
    ������2���������� �����ǡ�TCP_EVT_DATA���������¼����ͣ�
        ����3 ptr �� Զ��IP
        ����4 par ��Զ�̶˿ں�    
*/
U16 tcpserver_callback (U8 soc, U8 evt, U8 *ptr, U16 par)
{
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
      u1_printf("----������ֹ\r\n");
			break;
		
		/* SocketԶ�������Ѿ����� */
		case TCP_EVT_CONNECT:
      u1_printf("----SocketԶ�������Ѿ�����\r\n");
			break;
		
		/* ���ӶϿ� */
		case TCP_EVT_CLOSE: 
      u1_printf("----���ӶϿ�\r\n");      
			break;
		
		/* ���͵������յ�Զ���豸Ӧ�� */
		case TCP_EVT_ACK:
  //   u1_printf("----���͵������յ�Զ���豸Ӧ��\r\n");     
			break;
		
		/* ���յ�TCP����֡��ptrָ�����ݵ�ַ��par��¼���ݳ��ȣ���λ�ֽ� */
		case TCP_EVT_DATA:
      u1_printf("���յ�TCP����֡��ptrָ�����ݵ�ַ�����ݳ��� = %d\r\n", par);

      USART_OUT(USART_1, (uint8_t*)ptr, par);    
	}
	return (0);
}

uint8_t TCP_Client_StatusCheck(uint8_t socket_fp,uint8_t ip[4],uint16_t server_portnum,uint16_t local_port_num) 
{	
	switch (tcp_get_state(socket_fp)) 
	{
		case TCP_STATE_FREE:
		case TCP_STATE_CLOSED:
			tcp_connect (socket_fp, ip, server_portnum, local_port_num);
      u1_printf("������������\r\n");
			break;
		
		case TCP_STATE_LISTEN:
			break;
		
		case TCP_STATE_CONNECT:
      u1_printf("TCP�����Խ���\r\n");
			return (__TRUE);
			
		default:  
			break;
	}
	
	return (__FALSE);
}



#include "UDP_user.h"

///*TCP socket �ص�����
//����1��TCP socket ���
//����2���¼�����
//    TCP_EVT_CONREQ   Զ�̿ͻ���������Ϣ��
//    TCP_EVT_CONNECT  ���ӳɹ���Ϣ��
//    TCP_EVT_CLOSE    ���ӶϿ���Ϣ��
//    TCP_EVT_ABORT    ������ֹ��Ϣ��
//    TCP_EVT_ACK      �������ݺ��յ�Զ��������Ӧ��
//    TCP_EVT_DATA     ���յ� TCP ���ݰ���
//����3��
//����4��
//    ������2������������ TCP_EVT_DATA��
//        ����3 ptr ָ����ǽ��յ������ݵ�ַ
//        ����4 par �ǽ��յ����ݵĸ���
//    ������2���������� �����ǡ�TCP_EVT_DATA���������¼����ͣ�
//        ����3 ptr �� Զ��IP
//        ����4 par ��Զ�̶˿ں�    
//*/
//U16 tcp_callback (U8 soc, U8 evt, U8 *ptr, U16 par)
//{
//	switch (evt) 
//	{
//		/*
//			Զ�̿ͻ���������Ϣ
//		    1������ptr�洢Զ���豸��IP��ַ��par�д洢�˿ںš�
//		    2��������ֵ1�������ӣ�������ֵ0��ֹ���ӡ�
//		*/
//		case TCP_EVT_CONREQ:
//      u1_printf("Զ�̿ͻ�����������IP: %d.%d.%d.%d\r\n�˿�port:%d\r\n", ptr[0], ptr[1], ptr[2], ptr[3],par);
//			return (1);
//		
//		/* ������ֹ */
//		case TCP_EVT_ABORT:
//      u1_printf("----������ֹ\r\n");
//			break;
//		
//		/* SocketԶ�������Ѿ����� */
//		case TCP_EVT_CONNECT:
//      u1_printf("----SocketԶ�������Ѿ�����\r\n");
//			break;
//		
//		/* ���ӶϿ� */
//		case TCP_EVT_CLOSE: 
//      u1_printf("----���ӶϿ�\r\n");      
//			break;
//		
//		/* ���͵������յ�Զ���豸Ӧ�� */
//		case TCP_EVT_ACK:
//  //   u1_printf("----���͵������յ�Զ���豸Ӧ��\r\n");     
//			break;
//		
//		/* ���յ�TCP����֡��ptrָ�����ݵ�ַ��par��¼���ݳ��ȣ���λ�ֽ� */
//		case TCP_EVT_DATA:
//      u1_printf("���յ�TCP����֡��ptrָ�����ݵ�ַ�����ݳ��� = %d\r\n", par);

//      USART_OUT(USART_1, (uint8_t*)ptr, par);    
//	}
//	return (0);
//}
//uint8_t TCP_StatusCheck(TCP_status tcp) 
//{	 
//  uint8_t status = 0;
//  uint16_t  local_port = 0;
//  
//  if(tcp.mode == TCP_CLIENT)
//  {
//    local_port = tcp.client.local_port;
//  }
//  else if(tcp.mode == TCP_SERVER)
//  {
//    local_port = tcp.server.local_port;
//  }
//  
//  status = tcp_get_state(tcp.socket_fp);
//  
//	switch(status) 
//	{
//		case TCP_STATE_FREE:
//      u1_printf("tcp �����Ч\r\n");break;
//		case TCP_STATE_CLOSED:
//      if(tcp.mode == TCP_CLIENT)
//      {
//                      /*socket���  Զ�̷�����ip    Զ�̷������˿�          ���ض˿�*/
//        tcp_connect (tcp.socket_fp, tcp.client.ip, tcp.client.remote_port, tcp.client.local_port);
//        u1_printf("TCP �ͻ��� %d ���� �������� --> Զ�̷����\r\n",tcp.client.local_port);  
//      }
//      else if(tcp.mode == TCP_SERVER)
//      {
//        uint8_t res;
//        res = tcp_listen (tcp.socket_fp, tcp.client.local_port);
//        if(res == __TRUE)
//        {
//          u1_printf("tcp ����� ���� %d�˿ڳɹ�\r\n", tcp.server.local_port);
//        }
//        else
//        {
//          u1_printf("tcp ����� %d ���� ʧ��\r\n",tcp.server.local_port);
//        }       
//      }
//      else
//      {
//        u1_printf("tcp ģʽδѡ��\r\n");
//      }
//			break;	
//      
//		case TCP_STATE_LISTEN:   u1_printf("Socket �ȴ������У�%d ����ing��\r\n",local_port);break; 
//    case TCP_STATE_SYN_REC:  u1_printf("%d ���յ�SYN��\r\n",local_port);break;    
//    case TCP_STATE_SYN_SENT: u1_printf("%d ����SYN��,��������\r\n",local_port);break;
//    case TCP_STATE_FINW1:    u1_printf("%d ����FIN����׼����������\r\n",local_port);break;
//    case TCP_STATE_FINW2:    u1_printf("%d ���͵� FIN ���Ѿ��յ�Զ�̻�����Ӧ�𣬵�ǰ���ڵȴ�Զ�̻������� FIN ��\r\n",local_port);break;
//    case TCP_STATE_CLOSING:  u1_printf("%d ��Զ�̻����յ� FIN ����\r\n",local_port);break;
//    case TCP_STATE_LAST_ACK: u1_printf("%d �����Ѿ������� FIN �������һ�εȴ� ACK Ӧ��\r\n",local_port);break;
//    case TCP_STATE_TWAIT:    u1_printf("%d �ر�ǰ�ȴ� 2ms��\r\n",local_port);break;
//    
//		case TCP_STATE_CONNECT:  u1_printf("TCP %d �����ѽ���������ͨ��\r\n",local_port);break;
//			
//		default:  	break;	
//	}
//  
//  return  status;
//}

//uint8_t TCP_Server_StatusCheck(uint8_t socket_fp,uint16_t local_port_num) 
//{	
//	uint8_t res;
//	
//	switch (tcp_get_state(socket_fp)) 
//	{
//		case TCP_STATE_FREE:
//      u1_printf("tcp �����Ч\r\n", res);
//		case TCP_STATE_CLOSED:
//			res = tcp_listen (socket_fp, local_port_num);
//			u1_printf("tcp listen res = %d\r\n", res);
//			break;
//		
//		case TCP_STATE_LISTEN:
//			break;
//		
//		case TCP_STATE_CONNECT:
//			return (__TRUE);
//			
//		default:  
//			break;
//	}
//	
//	return (__FALSE);
//}



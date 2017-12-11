/*
*********************************************************************************************************
*
*	ģ������ : TCPnet����Э��ջ����
*	�ļ����� : app_tcpnet_lib.c
*	��    �� : V1.0
*	˵    �� : ���ԵĹ���˵��
*              1. ǿ���Ƽ������߽ӵ�·�������߽�����������ԣ���Ϊ�Ѿ�ʹ����DHCP�������Զ���ȡIP��ַ��
*              2. �����̴�����һ��socket�����������õ�TCPͨ��Э�飬����ʹ���˾���������NetBIOS���û�ֻ
*                 ���ڵ��Զ�ping armfly�Ϳ��Ի�ð��ӵ�IP��ַ�����ض˿ڱ�����Ϊ1024��
*              3. �û������ڵ��Զ�����������������TCP Client���Ӵ˷�������
*              4. ����������ַ��������ַ�1�����ӻظ��ַ�1��8�Լ��س��ͻ��������ַ�����10����
*              5. ����������ַ��������ַ�2�����ӻظ�1024���ַ���ǰ4���ַ���abcd�����4���ַ���efgh��
*                 �м��1016��ȫ�����ַ�0��
*
*	�޸ļ�¼ :
*		�汾��   ����         ����        ˵��
*		V1.0    2017-04-17   Eric2013     �׷�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/			
#include "includes.h"	



/*
*********************************************************************************************************
*	                                  ���ڱ��ļ��ĵ���
*********************************************************************************************************
*/
#if 1
	#define printf_debug printf
#else
	#define printf_debug(...)
#endif


/*
*********************************************************************************************************
*	                          �궨�壬���ض˿�
*********************************************************************************************************
*/
/* ����Ǳ��ض˿� */
#define LocalPort_NUM    1001


/*
*********************************************************************************************************
*	                                     ����
*********************************************************************************************************
*/
/* RL-TCPnet API�ķ���ֵ */
const char * ReVal_Table[]= 
{
	" 0: SCK_SUCCESS       Success                             ",
	"-1: SCK_ERROR         General Error                       ",
	"-2: SCK_EINVALID      Invalid socket descriptor           ",
	"-3: SCK_EINVALIDPARA  Invalid parameter                   ",
	"-4: SCK_EWOULDBLOCK   It would have blocked.              ",
	"-5: SCK_EMEMNOTAVAIL  Not enough memory in memory pool    ",
	"-6: SCK_ECLOSED       Connection is closed or aborted     ",
	"-7: SCK_ELOCKED       Socket is locked in RTX environment ",
	"-8: SCK_ETIMEOUT      Socket, Host Resolver timeout       ",
	"-9: SCK_EINPROGRESS   Host Name resolving in progress     ",
	"-10: SCK_ENONAME      Host Name not existing              ",
};

uint8_t sendbuf[1024];


/*
*********************************************************************************************************
*	�� �� ��: TCPnetTest
*	����˵��: TCPnetӦ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TCPnetTest(void)
{  
	char dbuf[10];
	int len;
	int sock, sd, res;
	SOCKADDR_IN addr;
	SOCKADDR_IN ReAddr;

	
	while (1) 
	{
		/* ����һ��socket 
		   ��1������AF_INET����ǰ��֧��������͵ĵ�ַ�塣
		   ��2������SOCK_STREAM����ʾ������ͨ�����ͣ���ʹ�õ�TCP��
		   ��3������0 ������Ϊ0�Ļ����Զ�����2����������Э��ƥ�䣬�������TCPЭ�顣
		*/
		sock = socket (AF_INET, SOCK_STREAM, 0);

		/* �˿ں�����Ϊ1001 */
		addr.sin_port        = htons(LocalPort_NUM);
		
		/* �뺯��socket�е�AF_INET����һ�� */
		addr.sin_family      = PF_INET;
		/* 
		   INADDR_ANY����ָ����ַΪ0.0.0.0�ĵ�ַ�������ַ��ʵ�ϱ�ʾ��ȷ����ַ�������е�ַ��
		   �����ַ����������Ļ��ͱ�ʾ��ض˿ں�Ϊddr.sin_port������IP��ַ��Ϣ��һ����Ҫ��
		   ���ж����������IP��ַ�������������ֻ����DM9161�����ڣ����Ǽ���������ڵ�IP��ַ��
		*/
		addr.sin_addr.s_addr = INADDR_ANY;
		
		/* ��socket��IP�Ͷ˿ں� */
		bind (sock, (SOCKADDR *)&addr, sizeof(addr));

		/* ���ü�����������1������ */
		listen (sock, 1);
		
		/* 
		   �ȴ�soket���������еĻ����Զ�����1���µ�socket��������ͨ�ţ�û�еĻ����ȴ����ӡ�
		   ע�⣬�ܹ�accept�ĸ����ܵ�listen���������ƣ���listen�������ܵ�Net_Config.c�к궨��
		   BSD_NUMSOCKS �����ơ�
		*/
		len = sizeof(ReAddr);
		sd = accept (sock, (SOCKADDR *)&ReAddr, &len);
		printf_debug ("Զ�̿ͻ�����������IP: %d.%d.%d.%d\n", ReAddr.sin_addr.s_b1,
                                                             ReAddr.sin_addr.s_b2,
															 ReAddr.sin_addr.s_b3,
                                                             ReAddr.sin_addr.s_b4);
		printf_debug ("Զ�̿ͻ��˶˿ں�: %d\n", ntohs(ReAddr.sin_port));
		
		/* �رռ���socket���������socket�ǵ��ú���socket���Զ������� */
		closesocket (sock);
		sock = sd;

		
		while (1) 
		{
			/*
			  socket���ݽ��պ��������recv����������ģʽ��ʹ���������ע���������
			  1. �˺��������ʱ���ܵ�Net_Config.c�к궨�� BSD_RCVTOUT �����ơ����ʱ�䵽���Զ��˳���
			  2. ����������յ�һ�����ݰ��ͻ᷵�أ����ڻ���С�����õĻ�������С��û�й�ϵ�����������
			     ���ڽ��ջ�������С���û�ֻ���ε��ú���recv���н��ռ��ɡ�
			  3. ʵ�ʽ��յ����ݴ�Сͨ���жϴ˺����ķ���ֵ���ɡ�
			*/
			res = recv (sock, dbuf, sizeof(dbuf), 0);
			if (res <= 0) 
			{
				printf_debug("�˳����պ��������¿�ʼ����%s\r\n", ReVal_Table[abs(res)]);
				break;
			}
			else
			{
				printf_debug("Receive Data Length = %d\r\n", res);
				switch(dbuf[0])
				{
					/* �ַ����� 1 */
					case '1':
						sendbuf[0] = '1';
						sendbuf[1] = '2';
						sendbuf[2] = '3';
						sendbuf[3] = '4';
						sendbuf[4] = '5';
						sendbuf[5] = '6';
						sendbuf[6] = '7';
						sendbuf[7] = '8';
						sendbuf[8] = '\r';
						sendbuf[9] = '\n';						
						res = send (sock, (char *)sendbuf, 10, 0);
						if (res < 0) 
						{
							printf_debug("����send��������ʧ��\r\n");
						}
						else
						{
							printf_debug("����send�������ݳɹ�\r\n");							
						}
						break;
					
					/* �ַ����� 2 */
					case '2':
						/* �����ݻ���������ַ�0����������������ֲ鿴���� */
						len = sizeof(sendbuf);
						memset(sendbuf, 48, len);
					
						/* �������ʼ�������ݰ���ǰ4���ֽں����4���ֽ� */
						sendbuf[0] = 'a';
						sendbuf[1] = 'b';
						sendbuf[2] = 'c';
						sendbuf[3] = 'd';
						sendbuf[len - 4] = 'e';
						sendbuf[len - 3] = 'f';
						sendbuf[len - 2] = 'g';
						sendbuf[len - 1] = 'h';					
						res = send (sock, (char *)sendbuf, len, 0);
						if (res < 0) 
						{
							printf_debug("����send��������ʧ��%s\r\n", ReVal_Table[abs(res)]);
						}
						else
						{
							printf_debug("����send�ɹ��������� = %d�ֽ�\r\n", res);							
						}
						break;
				
					/* ������ֵ�������� */
					default:                     
						break;
				}
			}

		}
		
		/* 
		   ���ʱ�䵽��Զ���豸�Ͽ����ӵȣ����򶼻�ִ�е��������������ر�socket��
		   ���򷵻ص���һ����whileѭ���Ŀ�ͷ���´���socket��������
		*/
		closesocket (sock);
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

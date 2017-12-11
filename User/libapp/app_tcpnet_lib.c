/*
*********************************************************************************************************
*
*	模块名称 : TCPnet网络协议栈测试
*	文件名称 : app_tcpnet_lib.c
*	版    本 : V1.0
*	说    明 : 测试的功能说明
*              1. 强烈推荐将网线接到路由器或者交换机上面测试，因为已经使能了DHCP，可以自动获取IP地址。
*              2. 本例程创建了一个socket服务器，采用的TCP通信协议，而且使能了局域网域名NetBIOS，用户只
*                 需在电脑端ping armfly就可以获得板子的IP地址，本地端口被设置为1024。
*              3. 用户可以在电脑端用网络调试软件创建TCP Client连接此服务器。
*              4. 网络调试助手发送命令字符1，板子回复字符1到8以及回车和换行两个字符，共10个。
*              5. 网络调试助手发送命令字符2，板子回复1024个字符，前4个字符是abcd，最后4个字符是efgh，
*                 中间的1016个全部是字符0。
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2017-04-17   Eric2013     首发
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/			
#include "includes.h"	



/*
*********************************************************************************************************
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#if 1
	#define printf_debug printf
#else
	#define printf_debug(...)
#endif


/*
*********************************************************************************************************
*	                          宏定义，本地端口
*********************************************************************************************************
*/
/* 这个是本地端口 */
#define LocalPort_NUM    1001


/*
*********************************************************************************************************
*	                                     变量
*********************************************************************************************************
*/
/* RL-TCPnet API的返回值 */
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
*	函 数 名: TCPnetTest
*	功能说明: TCPnet应用
*	形    参: 无
*	返 回 值: 无
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
		/* 创建一个socket 
		   第1个参数AF_INET：当前仅支持这个类型的地址族。
		   第2个参数SOCK_STREAM：表示数据流通信类型，即使用的TCP。
		   第3个参数0 ：配置为0的话，自动跟第2个参数进行协议匹配，这里就是TCP协议。
		*/
		sock = socket (AF_INET, SOCK_STREAM, 0);

		/* 端口号设置为1001 */
		addr.sin_port        = htons(LocalPort_NUM);
		
		/* 与函数socket中的AF_INET作用一样 */
		addr.sin_family      = PF_INET;
		/* 
		   INADDR_ANY就是指定地址为0.0.0.0的地址，这个地址事实上表示不确定地址，或所有地址，
		   任意地址。用在这里的话就表示监控端口号为ddr.sin_port的所有IP地址消息。一般主要用
		   于有多个网卡或者IP地址的情况。开发板只用了DM9161的网口，就是监听这个网口的IP地址。
		*/
		addr.sin_addr.s_addr = INADDR_ANY;
		
		/* 给socket绑定IP和端口号 */
		bind (sock, (SOCKADDR *)&addr, sizeof(addr));

		/* 设置监听，最大监听1个连接 */
		listen (sock, 1);
		
		/* 
		   等待soket连接请求，有的话，自动创建1个新的socket进行连接通信，没有的话，等待连接。
		   注意，能够accept的个数受到listen函数的限制，而listen函数又受到Net_Config.c中宏定义
		   BSD_NUMSOCKS 的限制。
		*/
		len = sizeof(ReAddr);
		sd = accept (sock, (SOCKADDR *)&ReAddr, &len);
		printf_debug ("远程客户端请求连接IP: %d.%d.%d.%d\n", ReAddr.sin_addr.s_b1,
                                                             ReAddr.sin_addr.s_b2,
															 ReAddr.sin_addr.s_b3,
                                                             ReAddr.sin_addr.s_b4);
		printf_debug ("远程客户端端口号: %d\n", ntohs(ReAddr.sin_port));
		
		/* 关闭监听socket，这个监听socket是调用函数socket后自动创建的 */
		closesocket (sock);
		sock = sd;

		
		while (1) 
		{
			/*
			  socket数据接收函数，如果recv工作在阻塞模式，使用这个函数注意以下事项：
			  1. 此函数的溢出时间受到Net_Config.c中宏定义 BSD_RCVTOUT 的限制。溢出时间到会自动退出。
			  2. 这个函数接收到一次数据包就会返回，大于或者小于设置的缓冲区大小都没有关系，如果数据量
			     大于接收缓冲区大小，用户只需多次调用函数recv进行接收即可。
			  3. 实际接收到数据大小通过判断此函数的返回值即可。
			*/
			res = recv (sock, dbuf, sizeof(dbuf), 0);
			if (res <= 0) 
			{
				printf_debug("退出接收函数，重新开始监听%s\r\n", ReVal_Table[abs(res)]);
				break;
			}
			else
			{
				printf_debug("Receive Data Length = %d\r\n", res);
				switch(dbuf[0])
				{
					/* 字符命令 1 */
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
							printf_debug("函数send发送数据失败\r\n");
						}
						else
						{
							printf_debug("函数send发送数据成功\r\n");							
						}
						break;
					
					/* 字符命令 2 */
					case '2':
						/* 将数据缓冲区清成字符0，方便网络调试助手查看数据 */
						len = sizeof(sendbuf);
						memset(sendbuf, 48, len);
					
						/* 这里仅初始化了数据包的前4个字节和最后4个字节 */
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
							printf_debug("函数send发送数据失败%s\r\n", ReVal_Table[abs(res)]);
						}
						else
						{
							printf_debug("函数send成功发送数据 = %d字节\r\n", res);							
						}
						break;
				
					/* 其它数值不做处理 */
					default:                     
						break;
				}
			}

		}
		
		/* 
		   溢出时间到，远程设备断开连接等，程序都会执行到这里，我们在这里关闭socket，
		   程序返回到第一个大while循环的开头重新创建socket并监听。
		*/
		closesocket (sock);
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

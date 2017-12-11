/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : RL-TCPnet的socket服务器实现，采用的TCP通信协议
*              实验目的：
*                1. 学习RL-TCPnet的socket服务器创建和数据收发。
*              实验内容：
*                1. 按下摇杆的OK键可以通过串口打印任务执行情况（波特率115200，数据位8，奇偶校验位无，停止位1）
*                   =================================================
*                   任务名      任务状态 优先级   剩余栈 任务序号
*                   vTaskUserIF     R       1       197     1
*                   IDLE            R       0       113     7
*                   vTaskMsgPro     B       3       489     3
*                   vTaskLED        B       2       483     2
*                   vTaskStart      B       6       456     6
*                   vTaskTCPnet     B       5       450     5
*                   vTaskSocket     B       4       436     4
*                   
*                   
*                   任务名       运行计数         使用率
*                   vTaskUserIF     730             <1%
*                   IDLE            7808926         99%
*                   vTaskTCPnet     3773            <1%
*                   vTaskLED        29              <1%
*                   vTaskStart      37250           <1%
*                   vTaskMsgPro     33804           <1%
*                   vTaskSocket     1929            <1%
*                  串口软件建议使用SecureCRT（V5光盘里面有此软件）查看打印信息。
*                  各个任务实现的功能如下：
*                   vTaskTaskUserIF 任务: 按键消息处理。
*                   vTaskLED        任务: LED闪烁。
*                   vTaskMsgPro     任务: 消息处理，这里用作按键检测。
*                   vTaskSocket     任务: socket服务器。
*                   vTaskTCPnet     任务: RL-TCPnet网络主任务。
*                   vTaskStart      任务: 启动任务，实现RL-TCPnet的时间基准更新。
*                2. 任务运行状态的定义如下，跟上面串口打印字母B, R, D, S对应：
*                    #define tskBLOCKED_CHAR		( 'B' )  阻塞
*                    #define tskREADY_CHAR		    ( 'R' )  就绪
*                    #define tskDELETED_CHAR		( 'D' )  删除
*                    #define tskSUSPENDED_CHAR	    ( 'S' )  挂起
*                3. 强烈推荐将网线接到路由器或者交换机上面测试，因为已经使能了DHCP，可以自动获取IP地址。
*                4. 本例程创建了一个socket服务器，采用的TCP通信协议，而且使能了局域网域名NetBIOS，用户只
*                   需在电脑端ping armfly就可以获得板子的IP地址，本地端口被设置为1024。
*                5. 用户可以在电脑端用网络调试软件创建TCP Client连接此服务器。
*                6. 网络调试助手发送命令字符1，板子回复字符1到8以及回车和换行两个字符，共10个。
*                7. 网络调试助手发送命令字符2，板子回复1024个字符，前4个字符是abcd，最后4个字符是efgh，
*                   中间的1016个全部是字符0。
*              注意事项：
*                1. 本实验推荐使用串口软件SecureCRT，要不串口打印效果不整齐。此软件在
*                   V5开发板光盘里面有。
*                2. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*
*	修改记录 :
*		版本号    日期         作者            说明
*       V1.0    2017-04-28   Eric2013    1. ST固件库到V1.5.0版本
*                                        2. BSP驱动包V1.2
*                                        3. FreeRTOS版本V8.2.3
*                                        4. RL-TCPnet版本V4.74
*
*	Copyright (C), 2016-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"



/*
**********************************************************************************************************
											函数声明
**********************************************************************************************************
*/
static void vTaskTaskUserIF(void *pvParameters);
static void vTaskLED(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskSocket(void *pvParameters);
static void vTaskTCPnet(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void AppTaskCreate (void);
static void AppObjCreate (void);
static void App_Printf(char *format, ...);


/*
**********************************************************************************************************
											变量声明
**********************************************************************************************************
*/
static TaskHandle_t xHandleTaskUserIF = NULL;
static TaskHandle_t xHandleTaskLED = NULL;
static TaskHandle_t xHandleTaskMsgPro = NULL;
static TaskHandle_t xHandleTaskSocket = NULL;
static TaskHandle_t xHandleTaskTCPnet = NULL;
static TaskHandle_t xHandleTaskStart = NULL;
static SemaphoreHandle_t  xMutex = NULL;
EventGroupHandle_t xCreatedEventGroup = NULL;


/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
#include "USART.h"
void USART_Init_Usr()
{
   USART_Configuration(USART_1, BAUD_RATE_230400, USART_Mode_Tx|USART_Mode_Rx, USART_IT_IDLE, USART_DMAReq_Tx|USART_DMAReq_Rx);
   USART_DMA_TX_Configuration(USART_1, (uint32_t)USART1_Buffer_Tx, USART1_BufferSize_Tx);
   USART_DMA_RX_Configuration(USART_1, (uint32_t)USART1_Buffer_Rx, USART1_BufferSize_Rx);
}
int main(void)
{
	/* 
	  在启动调度前，为了防止初始化STM32外设时有中断服务程序执行，这里禁止全局中断(除了NMI和HardFault)。
	  这样做的好处是：
	  1. 防止执行的中断服务程序中有FreeRTOS的API函数。
	  2. 保证系统正常启动，不受别的中断影响。
	  3. 关于是否关闭全局中断，大家根据自己的实际情况设置即可。
	  在移植文件port.c中的函数prvStartFirstTask中会重新开启全局中断。通过指令cpsie i开启，__set_PRIMASK(1)
	  和cpsie i是等效的。
     */
	__set_PRIMASK(1);  
	
	/* 硬件初始化 */
	bsp_Init(); 
	
	USART_Init_Usr();
	
	/* 1. 初始化一个定时器中断，精度高于滴答定时器中断，这样才可以获得准确的系统信息 仅供调试目的，实际项
		  目中不要使用，因为这个功能比较影响系统实时性。
	   2. 为了正确获取FreeRTOS的调试信息，可以考虑将上面的关闭中断指令__set_PRIMASK(1); 注释掉。 
	*/
	vSetupSysInfoTest();
	
	/* 创建任务 */
	AppTaskCreate();

	/* 创建任务通信机制 */
	AppObjCreate();
	
    /* 启动调度，开始执行任务 */
    vTaskStartScheduler();

	/* 
	  如果系统正常启动是不会运行到这里的，运行到这里极有可能是用于定时器任务或者空闲任务的
	  heap空间不足造成创建失败，此要加大FreeRTOSConfig.h文件中定义的heap大小：
	  #define configTOTAL_HEAP_SIZE	      ( ( size_t ) ( 17 * 1024 ) )
	*/
	while(1);
}

/*
*********************************************************************************************************
*	函 数 名: vTaskTaskUserIF
*	功能说明: 接口消息处理。
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 1  (数值越小优先级越低，这个跟uCOS相反)
*********************************************************************************************************
*/
static void vTaskTaskUserIF(void *pvParameters)
{
	uint8_t ucKeyCode;
	uint8_t pcWriteBuffer[500];

	
    while(1)
    {
		ucKeyCode = bsp_GetKey();
		
		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				/* K1键按下 */
				case KEY_DOWN_K1:
					App_Printf("K1键按下\r\n");	
					break;	

				/* K2键按下 */
				case KEY_DOWN_K2:
					App_Printf("K2键按下\r\n");
					break;
				
				/* K3键按下 */
				case KEY_DOWN_K3:
					App_Printf("K3键按下\r\n");
					break;
				
				/* 摇杆的OK键按下，打印任务执行情况 */
				case JOY_DOWN_OK:			 
					App_Printf("=================================================\r\n");
					App_Printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
					vTaskList((char *)&pcWriteBuffer);
					App_Printf("%s\r\n", pcWriteBuffer);
				
					App_Printf("\r\n任务名       运行计数         使用率\r\n");
					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
					App_Printf("%s\r\n", pcWriteBuffer);
					App_Printf("当前动态内存剩余大小 = %d字节\r\n", xPortGetFreeHeapSize());
					break;
				
				/* 其他的键值不处理 */
				default:                     
					break;
			}
		}
		
		vTaskDelay(20);
	}
}

/*
*********************************************************************************************************
*	函 数 名: vTaskLED
*	功能说明: LED闪烁	
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 2  
*********************************************************************************************************
*/
static void vTaskLED(void *pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 500;

	/* 获取当前的系统时间 */
    xLastWakeTime = xTaskGetTickCount();
	
    while(1)
    {
		bsp_LedToggle(2);
		
		/* vTaskDelayUntil是绝对延迟，vTaskDelay是相对延迟。*/
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/*
*********************************************************************************************************
*	函 数 名: vTaskMsgPro
*	功能说明: 消息处理，这里用作按键检测
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 3  
*********************************************************************************************************
*/
static void vTaskMsgPro(void *pvParameters)
{
    while(1)
    {
		/* 按键扫描 */
		bsp_KeyScan();
		vTaskDelay(10);
    }
}

/*
*********************************************************************************************************
*	函 数 名: vTaskSocket
*	功能说明: RL-TCPnet网络测试任务
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 4  
*********************************************************************************************************
*/
static void vTaskSocket(void *pvParameters)
{
    while(1)
    {
		TCPnetTest();
    }
}

/*
*********************************************************************************************************
*	函 数 名: vTaskTCPnet
*	功能说明: RL-TCPnet网络主任务
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 5  
*********************************************************************************************************
*/
static void vTaskTCPnet(void *pvParameters)
{
    while(1)
    {
		/* RL-TCPnet处理函数 */
		main_TcpNet();
		vTaskDelay(2);
    }
}

/*
*********************************************************************************************************
*	函 数 名: vTaskStart
*	功能说明: 启动任务，也是最高优先级任务，这里实现RL-TCPnet的时间基准更新
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 6  
*********************************************************************************************************
*/
static void vTaskStart(void *pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 100;
	
	/* 初始化RL-TCPnet */
	init_TcpNet ();
	
	/* 获取当前的系统时间 */
    xLastWakeTime = xTaskGetTickCount();
	
    while(1)
    {	
		/* RL-TCPnet时间基准更新函数 */
		timer_tick ();
		
		/* vTaskDelayUntil是绝对延迟，vTaskDelay是相对延迟。*/
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
				
/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    xTaskCreate( vTaskTaskUserIF,   	/* 任务函数  */
                 "vTaskUserIF",     	/* 任务名    */
                 512,               	/* 任务栈大小，单位word，也就是4字节 */
                 NULL,              	/* 任务参数  */
                 1,                 	/* 任务优先级*/
                 &xHandleTaskUserIF );  /* 任务句柄  */
	
	
	xTaskCreate( vTaskLED,    		/* 任务函数  */
                 "vTaskLED",  		/* 任务名    */
                 512,         		/* stack大小，单位word，也就是4字节 */
                 NULL,        		/* 任务参数  */
                 2,           		/* 任务优先级*/
                 &xHandleTaskLED ); /* 任务句柄  */
	
	xTaskCreate( vTaskMsgPro,     		/* 任务函数  */
                 "vTaskMsgPro",   		/* 任务名    */
                 512,             		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 3,               		/* 任务优先级*/
                 &xHandleTaskMsgPro );  /* 任务句柄  */

    xTaskCreate( vTaskSocket,     		/* 任务函数  */
                 "vTaskSocket",   		/* 任务名    */
                 512,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 4,              		/* 任务优先级*/
                 &xHandleTaskSocket );  /* 任务句柄  */
				 
    xTaskCreate( vTaskTCPnet,     		/* 任务函数  */
                 "vTaskTCPnet",   		/* 任务名    */
                 512,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 5,              		/* 任务优先级*/
                 &xHandleTaskTCPnet );  /* 任务句柄  */
	
	
	xTaskCreate( vTaskStart,     		/* 任务函数  */
                 "vTaskStart",   		/* 任务名    */
                 512,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 6,              		/* 任务优先级*/
                 &xHandleTaskStart );   /* 任务句柄  */
}

/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通信机制
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppObjCreate (void)
{
	/* 创建互斥信号量 */
    xMutex = xSemaphoreCreateMutex();
	
	if(xMutex == NULL)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    }
	
	/* 创建事件标志组 */
	xCreatedEventGroup = xEventGroupCreate();
	
	if(xCreatedEventGroup == NULL)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    }
}

/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式		  			  
*	形    参: 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
*********************************************************************************************************
*/


#include <stdarg.h>
#include <stdio.h>
void App_Printf(char* fmt,...)  
{  
  static char buff[USART1_BufferSize_Tx];
  
    memset(buff,0,USART1_BufferSize_Tx);
    va_list ap;
    va_start(ap,fmt);
    vsprintf((char*)buff,fmt,ap);
    va_end(ap);
  
    USART_OUT(USART_1, (uint8_t*)buff, strlen(buff));
}
//#define u1_printf(format, ...);
//static void  App_Printf(char *format, ...)
//{
////    char  buf_str[512 + 1];
////    va_list   v_args;


////    va_start(v_args, format);
////   (void)vsnprintf((char       *)&buf_str[0],
////                   (size_t      ) sizeof(buf_str),
////                   (char const *) format,
////                                  v_args);
////    va_end(v_args);

//	/* 互斥信号量 */
//	xSemaphoreTake(xMutex, portMAX_DELAY);

////    printf("%s", buf_str);
//	u1_printf(format, ...);  

//   	xSemaphoreGive(xMutex);
//}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

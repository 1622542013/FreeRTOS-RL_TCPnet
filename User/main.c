/*
*********************************************************************************************************
*
*	ģ������ : ������ģ�顣
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : RL-TCPnet��socket������ʵ�֣����õ�TCPͨ��Э��
*              ʵ��Ŀ�ģ�
*                1. ѧϰRL-TCPnet��socket�����������������շ���
*              ʵ�����ݣ�
*                1. ����ҡ�˵�OK������ͨ�����ڴ�ӡ����ִ�������������115200������λ8����żУ��λ�ޣ�ֹͣλ1��
*                   =================================================
*                   ������      ����״̬ ���ȼ�   ʣ��ջ �������
*                   vTaskUserIF     R       1       197     1
*                   IDLE            R       0       113     7
*                   vTaskMsgPro     B       3       489     3
*                   vTaskLED        B       2       483     2
*                   vTaskStart      B       6       456     6
*                   vTaskTCPnet     B       5       450     5
*                   vTaskSocket     B       4       436     4
*                   
*                   
*                   ������       ���м���         ʹ����
*                   vTaskUserIF     730             <1%
*                   IDLE            7808926         99%
*                   vTaskTCPnet     3773            <1%
*                   vTaskLED        29              <1%
*                   vTaskStart      37250           <1%
*                   vTaskMsgPro     33804           <1%
*                   vTaskSocket     1929            <1%
*                  �����������ʹ��SecureCRT��V5���������д�������鿴��ӡ��Ϣ��
*                  ��������ʵ�ֵĹ������£�
*                   vTaskTaskUserIF ����: ������Ϣ����
*                   vTaskLED        ����: LED��˸��
*                   vTaskMsgPro     ����: ��Ϣ������������������⡣
*                   vTaskSocket     ����: socket��������
*                   vTaskTCPnet     ����: RL-TCPnet����������
*                   vTaskStart      ����: ��������ʵ��RL-TCPnet��ʱ���׼���¡�
*                2. ��������״̬�Ķ������£������洮�ڴ�ӡ��ĸB, R, D, S��Ӧ��
*                    #define tskBLOCKED_CHAR		( 'B' )  ����
*                    #define tskREADY_CHAR		    ( 'R' )  ����
*                    #define tskDELETED_CHAR		( 'D' )  ɾ��
*                    #define tskSUSPENDED_CHAR	    ( 'S' )  ����
*                3. ǿ���Ƽ������߽ӵ�·�������߽�����������ԣ���Ϊ�Ѿ�ʹ����DHCP�������Զ���ȡIP��ַ��
*                4. �����̴�����һ��socket�����������õ�TCPͨ��Э�飬����ʹ���˾���������NetBIOS���û�ֻ
*                   ���ڵ��Զ�ping armfly�Ϳ��Ի�ð��ӵ�IP��ַ�����ض˿ڱ�����Ϊ1024��
*                5. �û������ڵ��Զ�����������������TCP Client���Ӵ˷�������
*                6. ����������ַ��������ַ�1�����ӻظ��ַ�1��8�Լ��س��ͻ��������ַ�����10����
*                7. ����������ַ��������ַ�2�����ӻظ�1024���ַ���ǰ4���ַ���abcd�����4���ַ���efgh��
*                   �м��1016��ȫ�����ַ�0��
*              ע�����
*                1. ��ʵ���Ƽ�ʹ�ô������SecureCRT��Ҫ�����ڴ�ӡЧ�������롣�������
*                   V5��������������С�
*                2. ��ؽ��༭��������������TAB����Ϊ4���Ķ����ļ���Ҫ��������ʾ�����롣
*
*	�޸ļ�¼ :
*		�汾��    ����         ����            ˵��
*       V1.0    2017-04-28   Eric2013    1. ST�̼��⵽V1.5.0�汾
*                                        2. BSP������V1.2
*                                        3. FreeRTOS�汾V8.2.3
*                                        4. RL-TCPnet�汾V4.74
*
*	Copyright (C), 2016-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"



/*
**********************************************************************************************************
											��������
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
											��������
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
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    �Σ���
*	�� �� ֵ: ��
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
	  ����������ǰ��Ϊ�˷�ֹ��ʼ��STM32����ʱ���жϷ������ִ�У������ֹȫ���ж�(����NMI��HardFault)��
	  �������ĺô��ǣ�
	  1. ��ִֹ�е��жϷ����������FreeRTOS��API������
	  2. ��֤ϵͳ�������������ܱ���ж�Ӱ�졣
	  3. �����Ƿ�ر�ȫ���жϣ���Ҹ����Լ���ʵ��������ü��ɡ�
	  ����ֲ�ļ�port.c�еĺ���prvStartFirstTask�л����¿���ȫ���жϡ�ͨ��ָ��cpsie i������__set_PRIMASK(1)
	  ��cpsie i�ǵ�Ч�ġ�
     */
	__set_PRIMASK(1);  
	
	/* Ӳ����ʼ�� */
	bsp_Init(); 
	
	USART_Init_Usr();
	
	/* 1. ��ʼ��һ����ʱ���жϣ����ȸ��ڵδ�ʱ���жϣ������ſ��Ի��׼ȷ��ϵͳ��Ϣ ��������Ŀ�ģ�ʵ����
		  Ŀ�в�Ҫʹ�ã���Ϊ������ܱȽ�Ӱ��ϵͳʵʱ�ԡ�
	   2. Ϊ����ȷ��ȡFreeRTOS�ĵ�����Ϣ�����Կ��ǽ�����Ĺر��ж�ָ��__set_PRIMASK(1); ע�͵��� 
	*/
	vSetupSysInfoTest();
	
	/* �������� */
	AppTaskCreate();

	/* ��������ͨ�Ż��� */
	AppObjCreate();
	
    /* �������ȣ���ʼִ������ */
    vTaskStartScheduler();

	/* 
	  ���ϵͳ���������ǲ������е�����ģ����е����Ｋ�п��������ڶ�ʱ��������߿��������
	  heap�ռ䲻����ɴ���ʧ�ܣ���Ҫ�Ӵ�FreeRTOSConfig.h�ļ��ж����heap��С��
	  #define configTOTAL_HEAP_SIZE	      ( ( size_t ) ( 17 * 1024 ) )
	*/
	while(1);
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskTaskUserIF
*	����˵��: �ӿ���Ϣ����
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 1  (��ֵԽС���ȼ�Խ�ͣ������uCOS�෴)
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
				/* K1������ */
				case KEY_DOWN_K1:
					App_Printf("K1������\r\n");	
					break;	

				/* K2������ */
				case KEY_DOWN_K2:
					App_Printf("K2������\r\n");
					break;
				
				/* K3������ */
				case KEY_DOWN_K3:
					App_Printf("K3������\r\n");
					break;
				
				/* ҡ�˵�OK�����£���ӡ����ִ����� */
				case JOY_DOWN_OK:			 
					App_Printf("=================================================\r\n");
					App_Printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
					vTaskList((char *)&pcWriteBuffer);
					App_Printf("%s\r\n", pcWriteBuffer);
				
					App_Printf("\r\n������       ���м���         ʹ����\r\n");
					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
					App_Printf("%s\r\n", pcWriteBuffer);
					App_Printf("��ǰ��̬�ڴ�ʣ���С = %d�ֽ�\r\n", xPortGetFreeHeapSize());
					break;
				
				/* �����ļ�ֵ������ */
				default:                     
					break;
			}
		}
		
		vTaskDelay(20);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskLED
*	����˵��: LED��˸	
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 2  
*********************************************************************************************************
*/
static void vTaskLED(void *pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 500;

	/* ��ȡ��ǰ��ϵͳʱ�� */
    xLastWakeTime = xTaskGetTickCount();
	
    while(1)
    {
		bsp_LedToggle(2);
		
		/* vTaskDelayUntil�Ǿ����ӳ٣�vTaskDelay������ӳ١�*/
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskMsgPro
*	����˵��: ��Ϣ�������������������
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 3  
*********************************************************************************************************
*/
static void vTaskMsgPro(void *pvParameters)
{
    while(1)
    {
		/* ����ɨ�� */
		bsp_KeyScan();
		vTaskDelay(10);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskSocket
*	����˵��: RL-TCPnet�����������
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 4  
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
*	�� �� ��: vTaskTCPnet
*	����˵��: RL-TCPnet����������
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 5  
*********************************************************************************************************
*/
static void vTaskTCPnet(void *pvParameters)
{
    while(1)
    {
		/* RL-TCPnet������ */
		main_TcpNet();
		vTaskDelay(2);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskStart
*	����˵��: ��������Ҳ��������ȼ���������ʵ��RL-TCPnet��ʱ���׼����
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 6  
*********************************************************************************************************
*/
static void vTaskStart(void *pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 100;
	
	/* ��ʼ��RL-TCPnet */
	init_TcpNet ();
	
	/* ��ȡ��ǰ��ϵͳʱ�� */
    xLastWakeTime = xTaskGetTickCount();
	
    while(1)
    {	
		/* RL-TCPnetʱ���׼���º��� */
		timer_tick ();
		
		/* vTaskDelayUntil�Ǿ����ӳ٣�vTaskDelay������ӳ١�*/
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
				
/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    xTaskCreate( vTaskTaskUserIF,   	/* ������  */
                 "vTaskUserIF",     	/* ������    */
                 512,               	/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,              	/* �������  */
                 1,                 	/* �������ȼ�*/
                 &xHandleTaskUserIF );  /* ������  */
	
	
	xTaskCreate( vTaskLED,    		/* ������  */
                 "vTaskLED",  		/* ������    */
                 512,         		/* stack��С����λword��Ҳ����4�ֽ� */
                 NULL,        		/* �������  */
                 2,           		/* �������ȼ�*/
                 &xHandleTaskLED ); /* ������  */
	
	xTaskCreate( vTaskMsgPro,     		/* ������  */
                 "vTaskMsgPro",   		/* ������    */
                 512,             		/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,           		/* �������  */
                 3,               		/* �������ȼ�*/
                 &xHandleTaskMsgPro );  /* ������  */

    xTaskCreate( vTaskSocket,     		/* ������  */
                 "vTaskSocket",   		/* ������    */
                 512,            		/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,           		/* �������  */
                 4,              		/* �������ȼ�*/
                 &xHandleTaskSocket );  /* ������  */
				 
    xTaskCreate( vTaskTCPnet,     		/* ������  */
                 "vTaskTCPnet",   		/* ������    */
                 512,            		/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,           		/* �������  */
                 5,              		/* �������ȼ�*/
                 &xHandleTaskTCPnet );  /* ������  */
	
	
	xTaskCreate( vTaskStart,     		/* ������  */
                 "vTaskStart",   		/* ������    */
                 512,            		/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,           		/* �������  */
                 6,              		/* �������ȼ�*/
                 &xHandleTaskStart );   /* ������  */
}

/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨ�Ż���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppObjCreate (void)
{
	/* ���������ź��� */
    xMutex = xSemaphoreCreateMutex();
	
	if(xMutex == NULL)
    {
        /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
    }
	
	/* �����¼���־�� */
	xCreatedEventGroup = xEventGroupCreate();
	
	if(xCreatedEventGroup == NULL)
    {
        /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
    }
}

/*
*********************************************************************************************************
*	�� �� ��: App_Printf
*	����˵��: �̰߳�ȫ��printf��ʽ		  			  
*	��    ��: ͬprintf�Ĳ�����
*             ��C�У����޷��г����ݺ���������ʵ�ε����ͺ���Ŀʱ,������ʡ�Ժ�ָ��������
*	�� �� ֵ: ��
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

//	/* �����ź��� */
//	xSemaphoreTake(xMutex, portMAX_DELAY);

////    printf("%s", buf_str);
//	u1_printf(format, ...);  

//   	xSemaphoreGive(xMutex);
//}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

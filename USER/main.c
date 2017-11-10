/**
 * @file        main.c
 * @brief
 */

/*============================================================================*/
/*                               Header include                               */
/*============================================================================*/

#include "config.h"
#include "stm32f4xx_it.h"
#include <stdlib.h>
#include <string.h>

#include "usr_FreeRTOS.h"
#include "TCP_user.h"

void TaskCreatUser(void);
void TaskGroupCreatUser(void);
void TaskQueueCreatUser(void);

TpInt32 main(TpVoid)
{
	/* Tim,Usart.. config*/ 
  __set_PRIMASK(1);
  /* ��������*/

  HardWareInit(); 
  
  TaskCreatUser();

  /* �������ȣ���ʼִ������ */
  vTaskStartScheduler();
   /*
     ϵͳ�������е����
     ������е�����п����Ƕѿռ䲻��ʹ�õ���
     #define configTOTAL_HEAP_SIZE        ( ( size_t ) ( 17 * 1024 ) )
   */
   while(1);		
}


/*-----------����--------------*/
TaskHandle_t HandleTaskStart = NULL;
void TaskStart(void* pv)
{  
  static uint8_t send_buff[200];
  uint16_t num;
  TickType_t xLastSYSTime;
  
  TcpInit();
  
  xLastSYSTime = xTaskGetTickCount();
  
  while(1)
  {
      timer_tick ();
    
      num = snprintf((char *)send_buff,200,"��������\r\n");
      USART_SetSendData(USART1, send_buff, num); 
      vTaskDelayUntil(&xLastSYSTime, 1000);
  }
}

TaskHandle_t HandleTaskTCPnet = NULL;
void TaskTCPnet(void* pv)
{  
  static uint8_t send_buff[200];
  uint16_t num;
  TickType_t xLastSYSTime;
  
  while(1)
  {
      TcpNetTest();
      num = snprintf((char *)send_buff,200,"��������\r\n");
      USART_SetSendData(USART1, send_buff, num); 
      vTaskDelayUntil(&xLastSYSTime, 1000);
  }
}

/*-------------�����������------------*/

void TaskCreatUser(void)
{ 
  xTaskCreate( TaskStart,         /* ������ */
               "TaskStart",       /* ������    */
               500,               /* ����ջ��С����λ��4�ֽ� */
               NULL,              /* �������  */
               2,                 /* �������ȼ�*/
               &HandleTaskStart); /* ������  */
  
  xTaskCreate( TaskTCPnet,         /* ������ */
               "TaskTCPnet",       /* ������    */
               500,               /* ����ջ��С����λ��4�ֽ� */
               NULL,              /* �������  */
               2,                 /* �������ȼ�*/
               &HandleTaskTCPnet); /* ������  */
}

void USART_ReceiveDataFinishCallback(USART_TypeDef* USARTx)
{
  int a = 0;
  
  if(USARTx == USART1)
  {  
      a ++;
      a --;
  }
}




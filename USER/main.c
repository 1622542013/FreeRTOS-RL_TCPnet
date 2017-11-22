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
#include <stdio.h>

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
  
  	/* ��ʼ��RL-TCPnet */
	init_TcpNet ();
  
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
  static uint32_t time_count;

  TickType_t xLastSYSTime;
  
  xLastSYSTime = xTaskGetTickCount();
  
  while(1)
  {
      timer_tick ();
    
      time_count++;
    
      u1_printf("ʱ�䣺 %d��\r\n",time_count);

      vTaskDelayUntil(&xLastSYSTime, 1000);
  }
}

TaskHandle_t HandleTaskTCPnet = NULL;
void TaskTCPnet(void* pv)
{  
  TickType_t xLastSYSTime;
  
  xLastSYSTime = xTaskGetTickCount();
  
  while(1)
  {
      TcpNetTest();
 
     // vTaskDelayUntil(&xLastSYSTime, 1000);
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


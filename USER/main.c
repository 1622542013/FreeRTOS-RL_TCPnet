/**
 * @file        main.c
 * @brief
 */

/*============================================================================*/
/*                               Header include                               */
/*============================================================================*/

#include "config.h"
#include "stm32f4xx_it.h"

#include "usr_FreeRTOS.h"

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
  TaskGroupCreatUser();
  TaskQueueCreatUser();
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
TaskHandle_t HandleTaskStartCollect = NULL;

void TaskStartCollect(void* pv)
{  
  while(1)
  {

  }
}

/*-------------�����������------------*/
void TaskGroupCreatUser(void)
{

}

void TaskQueueCreatUser(void)
{
  
}


void TaskCreatUser(void)
{
  
  xTaskCreate( TaskStartCollect,   /* ������ */
               "TaskStartCollect",    /* ������    */
               500,              /* ����ջ��С����λ��4�ֽ� */
               NULL,             /* �������  */
               2,                /* �������ȼ�*/
               &HandleTaskStartCollect); /* ������  */
}



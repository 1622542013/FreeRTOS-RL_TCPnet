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
  while(1)
  {
        num = snprintf((char *)send_buff,200,"��������");
        USART_SetSendData(UART4, send_buff, num); 
        vTaskDelay(1000);
  }
}

/*-------------�����������------------*/

void TaskCreatUser(void)
{ 
  xTaskCreate( TaskStart,           /* ������ */
               "TaskStart",    /* ������    */
               500,              /* ����ջ��С����λ��4�ֽ� */
               NULL,             /* �������  */
               2,                /* �������ȼ�*/
               &HandleTaskStart); /* ������  */
}

void USART_ReceiveDataFinishCallback(USART_TypeDef* USARTx)
{
  if(USARTx == UART4)
  {
      int a = 0;
      a= 1;
  }
}




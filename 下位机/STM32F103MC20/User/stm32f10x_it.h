/**
  ******************************************************************************
  * @file    EXTI/stm32f10x_it.h 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    04/16/2010
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_IT_H
#define __STM32F10x_IT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#define BUFLEN 1460
typedef struct TIME2_T
{
    unsigned int    time_overflow_1ms;
		unsigned int    recivetime;
    unsigned int    time_overflow_100ms;
		unsigned int    time_overflow_1s;
		unsigned int    time_overflow_3s;
		unsigned int    relinktime;
		unsigned int    hearttime;
    unsigned char   flag;
    unsigned int    send_gprs_position;
	
//		unsigned char   time_overflow_10ms_flag;
		unsigned char   time_overflow_1s_flag;
		unsigned char   time_overflow_3s_flag;
		unsigned char   relinktime_flag;
		unsigned char   hearttime_flag;
}TIME2_T;
extern TIME2_T time2;


typedef struct
{
  __IO u16 result;
  __IO u8  FilterCnt;
}FilterTypeDef;

typedef struct _TP_FLAG
{
    unsigned char tp_analysis;     //透传数据 数据分析中
    unsigned char tp_receiver;     //接收中
    unsigned char tp_stop;         //接收完毕
}TP_FLAG;





extern FilterTypeDef Filter;

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void USART1_IRQHandler(void);
void TIM2_IRQHandler(void);


#endif /* __STM32F10x_IT_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

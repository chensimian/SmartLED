#include "includes.h"

extern TICK_TIMES Tick;
WLMeter meter;
/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{

}

void EXTI0_IRQHandler(void)
{

		EXTI_ClearITPendingBit(EXTI_Line0);  //清除EXTI0线路挂起位
}



 void EXTI4_IRQHandler(void)
{			

 	 EXTI_ClearITPendingBit(EXTI_Line4);    //清除LINE5上的中断标志位  
}


void EXTI9_5_IRQHandler(void)
{

	 EXTI_ClearITPendingBit(EXTI_Line5);  //清除LINE15线路挂起位
}



/**
  * @brief  This function handles USART2 interrupt request.
  * @param  None
  * @retval None
  */


void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE)==SET)              //接收到数据
    {
				buf_uart1.time = 0; 
				/*if(buf_uart1.recFlag ==RESET)
				{
						buf_uart1.cnt = 0 ;
						buf_uart1.Rebuf[buf_uart1.cnt] = USART_ReceiveData(USART1);
						if(buf_uart1.Rebuf[buf_uart1.cnt]!='#')
						{
								buf_uart1.cnt = 0 ;	
						}
						else
						{
								buf_uart1.recFlag = SET;
						}
						
				}
				else*/
				{buf_uart1.recFlag=SET;
						
						buf_uart1.Rebuf[buf_uart1.cnt] = USART_ReceiveData(USART1);
						buf_uart1.cnt++;
						if(buf_uart1.cnt >= 200)
						{
								buf_uart1.cnt = 0 ;			
						}
				}
				
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}


/**
  * @brief  This function handles USART2 interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE)==SET)              //接收到数据
    {
				buf_uart2.time = 0; 
				if(buf_uart2.recFlag ==RESET)
				{
						buf_uart2.cnt = 0 ;
						buf_uart2.Rebuf[buf_uart2.cnt] = USART_ReceiveData(USART2);
						if(buf_uart2.Rebuf[buf_uart2.cnt]!='#')
						{
								buf_uart2.cnt = 0 ;	
						}
						else
						{
								buf_uart2.recFlag = SET;
						}
						
				}
				else
				{
						buf_uart2.cnt++;
						buf_uart2.Rebuf[buf_uart2.cnt] = USART_ReceiveData(USART2);
						if(buf_uart2.cnt >= 200)
						{
								buf_uart2.cnt = 0 ;			
						}
				}
				
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
}


void TIM2_IRQHandler(void)          //定时器中断约1ms
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {     
				Tick._1ms++;			   //1S
				Tick._5stime++;
				Tick._30s++;
			
				if(buf_uart1.recFlag==SET)
				{
						buf_uart1.time++;		   //1ms
					
						if(buf_uart1.time >= 50)  //50ms 
						{
								buf_uart1.time = 0;
								Get_Clinet();
								buf_uart1.flay = SET;
								buf_uart1.recFlag = RESET;
						}
				}
				
				if(buf_uart2.recFlag==SET)
				{
						buf_uart2.time++;		   //1ms
					
						if(buf_uart2.time >= 50)  //50ms 
						{
								buf_uart2.time = 0;
								buf_uart2.flay = SET;
								
						}
				}
        TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}


/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

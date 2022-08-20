#include "includes.h"


#if 1
#pragma import(__use_no_semihosting)             
                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;         
_sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{   

  while((USART2->SR&0x40) == 0);
  USART2->DR = (u8)ch;      
  return ch;
}
#endif





void Usart1_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//PA9	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//PA10	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_10;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //���պͷ���
	usartInitStruct.USART_Parity = USART_Parity_No; //��У��
	usartInitStruct.USART_StopBits = USART_StopBits_1; //1λֹͣλ
	usartInitStruct.USART_WordLength = USART_WordLength_8b; //8λ����λ
	USART_Init(USART1, &usartInitStruct);
	
	USART_Cmd(USART1, ENABLE); //ʹ�ܴ���
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //ʹ�ܽ����ж�
	
	nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&nvicInitStruct);

}

void Usart2_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//PA2	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//PA3	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //���պͷ���
	usartInitStruct.USART_Parity = USART_Parity_No; //��У��
	usartInitStruct.USART_StopBits = USART_StopBits_1; //1λֹͣλ
	usartInitStruct.USART_WordLength = USART_WordLength_8b; //8λ����λ
	USART_Init(USART2, &usartInitStruct);
	
	USART_Cmd(USART2, ENABLE); //ʹ�ܴ���
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //ʹ�ܽ����ж�
	
	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&nvicInitStruct);

}




/**
  * @brief  ��ʱ��2��ʼ��
  * @param  None
  * @retval None  csic.taobao.com
  */
  //((1+TIM_Prescaler )/72M)*(1+TIM_Period )=((1+2)/72M)*(1+65535)=0.00273��=366.2Hz */
void TIM2_init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  TIM_TimeBaseStructure.TIM_Period = 359;             //1ms ��ʱ (1+199)/72M *(1+359) = 0.001s
  TIM_TimeBaseStructure.TIM_Prescaler = 199;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
  TIM_ARRPreloadConfig(TIM2, DISABLE);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
}
/**
  * @brief  ��ʱ��3��ʼ��
  * @param  None
  * @retval None  csic.taobao.com
  */
  //((1+TIM_Prescaler )/36M)*(1+TIM_Period )
void TIM3_init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  TIM_TimeBaseStructure.TIM_Period = 179;
  TIM_TimeBaseStructure.TIM_Prescaler = 199;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIM3, TIM_FLAG_Update);
  TIM_ARRPreloadConfig(TIM3, DISABLE);
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM3, ENABLE);
}
/**
  * @brief  ���ö�ʱ��2�ж�
  * @param  None
  * @retval None   csic.taobao.com
  */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
      





/**
  * @brief  ���ָ�����ڴ�ռ�ʹ�С
  * @param  ram: �ڴ�ռ���׵�ַ n:Ҫ����ռ�Ĵ�С
  * @retval None	 csic.taobao.com
  */
void ClearRAM(u8* ram,unsigned int n)
{
  unsigned int i;
  for (i = 0;i < n;i++)
  {
    ram[i] = 0x00;
  }
}

/**
  * @brief  ���ָ�����ڴ�ռ�ʹ�С
  * @param  ram: �ڴ�ռ���׵�ַ n:Ҫ����ռ�Ĵ�С
  * @retval None	 csic.taobao.com
  */
void ClearCharRAM(char* ram,unsigned int n)
{
  unsigned int i;
  for (i = 0;i < n;i++)
  {
    ram[i] = 0x00;
  }
}




void ClearRebuf(void)
{
		buf_uart1.cnt = 0;
		buf_uart1.flay = RESET;
		Usart_SendString(USART2,(char*)buf_uart1.Rebuf);
		ClearRAM(buf_uart1.Rebuf,200);	
		delay_ms(20);
}

/*****************  ����һ���ַ� **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch )
{
	/* ����һ���ֽ����ݵ�USART1 */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ�������� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}
/*****************  ָ�����ȵķ����ַ��� **********************/
void Usart_SendStr_length( USART_TypeDef * pUSARTx, char *str,uint32_t strlen )
{
	unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(k < strlen);
}

/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(*(str + k)!='\0');
}

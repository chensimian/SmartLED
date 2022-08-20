#ifndef __BSP_H
#define __BSP_H

#include "stm32f10x.h"

#define MC20_USART_IO     GPIOA
#define MC20_USART_TXD    GPIO_Pin_2
#define MC20_USART_RXD	 	GPIO_Pin_3
#define MC20_USART        USART2
#define MC20_USART_IRQn	 	USART2_IRQn


void ClearCharRAM(char* ram,unsigned int n);
void ClearRAM(unsigned char* ram,unsigned int n);
void EXTIX_Init(void);
void TIM2_init(void);
void TIM3_init(void);
void NVIC_Configuration(void);
void DMA_init(void);
void ADC_init(void);
void GPIO_init(void);
void KEY_Init(void);
void PrintfClear(void);

void Usart1_Init(unsigned int baud);
void Usart2_Init(unsigned int baud);
void ClearRebuf(void);
void GPIO_ini(void);

//void USARTx_Config(void);
void Usart_SendStr_length( USART_TypeDef * pUSARTx, char *str,uint32_t strlen );
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch );









#endif


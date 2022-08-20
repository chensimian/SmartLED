#include "includes.h"


void KEY_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;       

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);//
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE); 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//∏°ø’ ‰»Î
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50M
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4; //PB3
		GPIO_Init(GPIOB, &GPIO_InitStructure);//
}

unsigned char Read_KEY0_Status(void)
{
		return (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3));
}	
	

unsigned char Read_KEY1_Status(void)
{
		return (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4));
}	



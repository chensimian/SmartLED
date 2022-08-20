#include "includes.h"

//IO1		PA4
//IO2   PA5



/**
  * @brief  人体检测引脚初始化.
  * @param  None
  * @retval None   
  */
void Sensor_hc_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
}





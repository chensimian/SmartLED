#include "stm32f10x.h"


#define HIGH  1
#define LOW   0

#define VS1838_CLK     RCC_APB2Periph_GPIOA
#define VS1838_PIN     GPIO_Pin_1                  
#define VS1838_PORT		 GPIOA

 //读取引脚的电平
#define  VS1838_DATA_IN()	 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)

          
#define	User_code		0xFD02		                //定义红外接收用户码   
          
#define	IR_sample			100		                  // 定义采样时间，在60us~250us之间          
#define IR_SYNC_MAX		(15000/IR_sample)	      // 同步信号SYNC 最大时间 15ms(标准值9+4.5=13.5ms)
#define IR_SYNC_MIN		(9700 /IR_sample)	      // 同步信号SYNC 最小时间 9.5ms，(连发信号标准值9+2.25=11.25ms)
#define IR_SYNC_DIVIDE	(12375/IR_sample)   	// 区分13.5ms同步信号与11.25ms连发信号，11.25+（13.5-11.25）/2=12.375ms
#define IR_DATA_MAX		(3000 /IR_sample)     	// 数据最大时间3ms    (标准值2.25 ms)
#define IR_DATA_MIN		(600  /IR_sample)     	// 数据最小时,0.6ms   (标准值1.12 ms)
#define IR_DATA_DIVIDE	(1687 /IR_sample)    	// 区分数据 0与1,1.12+ (2.25-1.12)/2 =1.685ms
#define IR_BIT_NUMBER		32				            // 32位数据          
                
void VS1838_PA1_Init(void);
uint8_t IR_RX_VS1838(void);
void Get_IR_Data(void);



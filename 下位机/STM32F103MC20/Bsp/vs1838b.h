#include "stm32f10x.h"


#define HIGH  1
#define LOW   0

#define VS1838_CLK     RCC_APB2Periph_GPIOA
#define VS1838_PIN     GPIO_Pin_1                  
#define VS1838_PORT		 GPIOA

 //��ȡ���ŵĵ�ƽ
#define  VS1838_DATA_IN()	 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)

          
#define	User_code		0xFD02		                //�����������û���   
          
#define	IR_sample			100		                  // �������ʱ�䣬��60us~250us֮��          
#define IR_SYNC_MAX		(15000/IR_sample)	      // ͬ���ź�SYNC ���ʱ�� 15ms(��׼ֵ9+4.5=13.5ms)
#define IR_SYNC_MIN		(9700 /IR_sample)	      // ͬ���ź�SYNC ��Сʱ�� 9.5ms��(�����źű�׼ֵ9+2.25=11.25ms)
#define IR_SYNC_DIVIDE	(12375/IR_sample)   	// ����13.5msͬ���ź���11.25ms�����źţ�11.25+��13.5-11.25��/2=12.375ms
#define IR_DATA_MAX		(3000 /IR_sample)     	// �������ʱ��3ms    (��׼ֵ2.25 ms)
#define IR_DATA_MIN		(600  /IR_sample)     	// ������Сʱ,0.6ms   (��׼ֵ1.12 ms)
#define IR_DATA_DIVIDE	(1687 /IR_sample)    	// �������� 0��1,1.12+ (2.25-1.12)/2 =1.685ms
#define IR_BIT_NUMBER		32				            // 32λ����          
                
void VS1838_PA1_Init(void);
uint8_t IR_RX_VS1838(void);
void Get_IR_Data(void);



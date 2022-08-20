#include "stm32f10x.h"
#include "led.h"
#include "delay.h"

/***************************************************************************************************
 * ��  �� : ��ʼ����Ƭ������D1��D2��D3��D4������PG6��PA8��PB8��PB9����4���û�LED�ĳ�ʼ״̬����ΪϨ��
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************************/
void leds_init(void)
{
   //����IO��ʼ�����ýṹ��
   GPIO_InitTypeDef  GPIO_InitStructure;
   //��PA  PB  PG�˿�ʱ��
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOG, ENABLE);	 
	 
	 //���õ�IO��PG6
   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;		
	 //����Ϊ�������
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	 //IO���ٶ�Ϊ10MHz
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 //����IO
   GPIO_Init(GPIOG, &GPIO_InitStructure);	
	
	
	//���õ�IO��PA8
   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;	
	//����IO
   GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	//���õ�IO��PB8��PB9
   GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9;	
	//����IO
   GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
   //����D1��D2��D3��D4��ʼ��״̬ΪϨ��	
	 leds_off();					
}
/***************************************************************************
 * ��  �� : �����������ϵ�4��ָʾ��(D1 D2 D3 D4)
 * ��  �� : ��
 * ����ֵ : ��
 ***************************************************************************/
void leds_on(void)
{
   //LEDָʾ��D1������(PG6)����͵�ƽ��������D1
   GPIO_ResetBits(GPIOG,GPIO_Pin_6);
   //LEDָʾ��D2������(PA8)����͵�ƽ��������D2
	 GPIO_ResetBits(GPIOA,GPIO_Pin_8);
	 //LEDָʾ��D3��D4������(PB8 PB9)����͵�ƽ��������D3��D4
	 GPIO_ResetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9);
}

/***************************************************************************
 * ��  �� : Ϩ�𿪷����ϵ�4��ָʾ��(D1 D2 D3 D4)
 * ��  �� : ��
 * ����ֵ : ��
 ***************************************************************************/
void leds_off(void)
{
   //LEDָʾ��D1������(PG6)����ߵ�ƽ����Ϩ��D1
   GPIO_SetBits(GPIOG,GPIO_Pin_6);
   //LEDָʾ��D1������(PA8)����ߵ�ƽ����Ϩ��D2
	 GPIO_SetBits(GPIOA,GPIO_Pin_8);
	 //LEDָʾ��D1������(PB8 PB9)����ߵ�ƽ����Ϩ��D3 D4
	 GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9);
}

/***************************************************************************
 * ��  �� : ����һ��ָ����LED
 * ��  �� : [IN]led_idx:LED ��Ӧ�����ű��
 * ����ֵ : ��
 ***************************************************************************/
void led_on(uint32_t led_idx)
{
   //ȡ�����ŵı��
	 uint16_t pin_num = (uint16_t)(led_idx&0xFFFF);
	 //�������ݶ˿ڣ�������pin_num��Ӧ����������͵�ƽ����������Ӧ��LED
	 if(led_idx == LED_1)GPIOG->BRR = pin_num;
	 if(led_idx == LED_2)GPIOA->BRR = pin_num;
	 if((led_idx == LED_3) || (led_idx == LED_4))
	 {
		  GPIOB->BRR = pin_num;
	 }
}

/***************************************************************************
 * ��  �� : Ϩ��һ��ָ����LED
 * ��  �� : [IN]led_idx:LED ��Ӧ�����ű��
 * ����ֵ : ��
 ***************************************************************************/
void led_off(uint32_t led_idx)
{
   //ȡ�����ŵı��
	 uint16_t pin_num = (uint16_t)(led_idx&0xFFFF);
	 //�������ݶ˿ڣ�������pin_num��Ӧ����������ߵ�ƽ����Ϩ���Ӧ��LED
	 if(led_idx == LED_1)GPIOG->BSRR = pin_num;
	 if(led_idx == LED_2)GPIOA->BSRR = pin_num;
	 if((led_idx == LED_3) || (led_idx == LED_4))
	 {
		   GPIOB->BSRR = pin_num;
	 }
}

/***************************************************************************
 * ��  �� : ��תһ��ָ����LED��״̬
 * ��  �� : [IN]led_idx:LED ��Ӧ�����ű��
 * ����ֵ : ��
 ***************************************************************************/
void led_toggle(uint32_t led_idx)
{
   //ȡ�����ŵı��
	 uint16_t pin_num = (uint16_t)(led_idx&0xFFFF);
	
	 //�������ݶ˿ڣ�ͨ����ODR�Ĵ�����Ӧ��λ������㷭ת��Ӧ��IO���״̬
	 if(led_idx == LED_1)
	 {
		 GPIOG->ODR ^= pin_num;
	 }
	 if(led_idx == LED_2)
	 {
		 GPIOA->ODR ^= pin_num;
	 }
	 if((led_idx == LED_3) || (led_idx == LED_4))
	 {
		 GPIOB->ODR ^= pin_num;
	 }
}



#include "oled.h"
#include "codetab.h"
#include "delay.h"

// ------------------------------------------------------------
// IO��ģ��I2Cͨ��
// SCL��PB6
// SDA��PB7
// ------------------------------------------------------------
void OLED_GPIO_Init(void)
{
		RCC->APB2ENR|=1<<3;			//��ʹ������IO PORTBʱ�� 							 
		GPIOB->CRL&=0X00FFFFFF;	//PB6/7 ������� 
		GPIOB->CRL|=0X77000000;	//PB6/7��©���  
		GPIOB->ODR|=2<<7;     	//PC6,7 �����
}


/**************************ʵ�ֺ���********************************************
*����ԭ��:		void IIC_Start(void)
*��������:		����IIC��ʼ�ź�
*******************************************************************************/
void OLED_IIC_Start(void)
{
		//SDA_OUT();     //sda�����
		OLED_IIC_SDA=1;
		if(!OLED_WRITE_SDA)//return 0;	
		OLED_IIC_SCL=1;
		delay_us(1);
		OLED_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
		if(OLED_WRITE_SDA)//return 0;
		delay_us(1);
		OLED_IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
		//return 1;
}


/**************************ʵ�ֺ���********************************************
*����ԭ��:		void IIC_Stop(void)
*��������:	    //����IICֹͣ�ź�
*******************************************************************************/	  
void  OLED_IIC_Stop(void)
{
		//SDA_OUT();//sda�����
		OLED_IIC_SCL=0;
		OLED_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
		delay_us(1);
		OLED_IIC_SCL=1; 
		OLED_IIC_SDA=1;//����I2C���߽����ź�
		delay_us(1);							   	
}


void Write_IIC_Byte(unsigned char IIC_Byte)
{
		unsigned char i;
		for(i=0;i<8;i++)
		{
				if(IIC_Byte & 0x80)
						OLED_IIC_SDA=1;
				else
						OLED_IIC_SDA=0;
				OLED_IIC_SCL=1;
				delay_us(1);  //�����б���SCL�������ʱ
				OLED_IIC_SCL=0;
				IIC_Byte<<=1;
		}
		OLED_IIC_SDA = 1;//ԭ����������һ������SDA������OLED��DATASHEET���˾����ȥ����
		OLED_IIC_SCL=1;
		delay_us(1);
		OLED_IIC_SCL=0;
}


/*********************OLEDд����************************************/ 
void OLED_WrDat(unsigned char IIC_Data)
{
		OLED_IIC_Start();
		Write_IIC_Byte(0x78);
		Write_IIC_Byte(0x40);			//write data
		Write_IIC_Byte(IIC_Data);
		OLED_IIC_Stop();
}
/*********************OLEDд����************************************/
void OLED_WrCmd(unsigned char IIC_Command)
{
		OLED_IIC_Start();
		Write_IIC_Byte(0x78);            //Slave address,SA0=0
		Write_IIC_Byte(0x00);			//write command
		Write_IIC_Byte(IIC_Command);
		OLED_IIC_Stop();
}
/*********************OLED ��������************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(((x&0xf0)>>4)|0x10);
		OLED_WrCmd((x&0x0f)|0x01);
} 
/*********************OLEDȫ��************************************/
void OLED_Fill(unsigned char bmp_dat) 
{
		unsigned char y,x;
		for(y=0;y<8;y++)
		{
				OLED_WrCmd(0xb0+y);
				OLED_WrCmd(0x01);
				OLED_WrCmd(0x10);
				for(x=0;x<X_WIDTH;x++)
				OLED_WrDat(bmp_dat);
		}
}
/*********************OLED��λ************************************/
void OLED_CLS(void)
{
		unsigned char y,x;
		for(y=0;y<8;y++)
		{
				OLED_WrCmd(0xb0+y);
				OLED_WrCmd(0x01);
				OLED_WrCmd(0x10);
				for(x=0;x<X_WIDTH;x++)
				OLED_WrDat(0);
		}
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : ��OLED���� -- ����ģʽ��,OLED���Ĳ���10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
		OLED_WrCmd(0X8D);  //���õ�ɱ�
		OLED_WrCmd(0X10);  //�رյ�ɱ�
		OLED_WrCmd(0XAE);  //OLED����
}


//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : ��OLED�������л���
//--------------------------------------------------------------
void OLED_ON(void)
{
		OLED_WrCmd(0X8D);  //���õ�ɱ�
		OLED_WrCmd(0X14);  //������ɱ�
		OLED_WrCmd(0XAF);  //OLED����
}


/*********************OLED��ʼ��************************************/
void OLED_Init(void)
{
		delay_ms(500);//��ʼ��֮ǰ����ʱ����Ҫ��
		OLED_WrCmd(0xae);//--turn off oled panel
		OLED_WrCmd(0x00);//---set low column address
		OLED_WrCmd(0x10);//---set high column address
		OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
		OLED_WrCmd(0x81);//--set contrast control register
		OLED_WrCmd(Brightness); // Set SEG Output Current Brightness ���ȵ��� 0x00~0xff
		OLED_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
		OLED_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
		OLED_WrCmd(0xa6);//--set normal display
		OLED_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
		OLED_WrCmd(0x3f);//--1/64 duty
		OLED_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
		OLED_WrCmd(0x00);//-not offset
		OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
		OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
		OLED_WrCmd(0xd9);//--set pre-charge period
		OLED_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
		OLED_WrCmd(0xda);//--set com pins hardware configuration
		OLED_WrCmd(0x12);
		OLED_WrCmd(0xdb);//--set vcomh
		OLED_WrCmd(0x40);//Set VCOM Deselect Level
		OLED_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
		OLED_WrCmd(0x02);//
		OLED_WrCmd(0x8d);//--set Charge Pump enable/disable
		OLED_WrCmd(0x14);//--set(0x10) disable
		OLED_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
		OLED_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
		OLED_WrCmd(0xaf);//--turn on oled panel
		OLED_Fill(0x00); //��ʼ����
		OLED_Set_Pos(0,0);
} 


/***************������������ʾ6*8һ���׼ASCII�ַ���	��ʾ�����꣨x,y����yΪҳ��Χ0��7****************/
void OLED_P6x8Str(unsigned char x,unsigned char y,char ch[])
{
		unsigned char c=0,i=0,j=0;
		while (ch[j]!='\0')
		{
				c =ch[j]-32;
				if(x>126){x=0;y++;}
				OLED_Set_Pos(x,y);
				for(i=0;i<6;i++)
				OLED_WrDat(F6x8[c][i]);
				x+=6;
				j++;
		}
}

/*******************������������ʾ8*16һ���׼ASCII�ַ���	 ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************/
void OLED_P8x16Str(unsigned char x,unsigned  char y, char ch[])
{
		unsigned char c=0,i=0,j=0;
		while (ch[j]!='\0')
		{
				c =ch[j]-32;
				if(x>120){x=0;y++;}
				OLED_Set_Pos(x,y);
				for(i=0;i<8;i++)
				OLED_WrDat(F8X16[c*16+i]);
				OLED_Set_Pos(x,y+1);
				for(i=0;i<8;i++)
				OLED_WrDat(F8X16[c*16+i+8]);
				x+=8;
				j++;
		}
}

//--------------------------------------------------------------
// Prototype      : OLED_P16x16Ch(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); N:������codetab.h�е�����
// Description    : ��ʾcodetab.h�еĺ���,16*16����
//--------------------------------------------------------------

/*****************������������ʾ16*16����  ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************************/
void OLED_P16x16Ch(unsigned char x,unsigned char y,unsigned int N)
{
	unsigned char wm=0;
	unsigned int adder=32*N;
	OLED_Set_Pos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	}
	OLED_Set_Pos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	} 	  	
}
/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
void Draw_BMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WrDat(BMP[j++]);
	    }
	}
}


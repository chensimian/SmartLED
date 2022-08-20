#include "includes.h"

//mqtt IP:111.229.82.92 PORT:61613 admin password SUB:slapp/123456  sldev/123456

_Bool ESP8266_SendCmd(char *cmd, char *res);
extern char wifi[20] ;
extern char password[20];

unsigned char getflag;
unsigned char	Recwifi_data[50];


void ESP8266_SERVER(void)																													//服务器建立
{
		char temp[40];
		ESP8266_SendCmd("AT+RST\r\n","ok");            															 //ESP8266复位

		ESP8266_SendCmd("AT\r\n","ok");
		 
		ESP8266_SendCmd("AT+RATE=9600\r\n","ok");                     
		 
		sprintf(temp,"AT+AP=%s,%s\r\n",wifi,password);
		ESP8266_SendCmd(temp,"ok");
		 
		ESP8266_SendCmd("AT+DEVICEID=aqwsdr52847\r\n","ok");													//设备ID aqwsdr52847 需要客户自己修改随机ID与其他用户不冲突
		 
		ESP8266_SendCmd("AT+KEEPALIVE=120\r\n","ok");        													//MQTT存活期 0-3600秒
		 
		ESP8266_SendCmd("AT+MQTTCLOUD=111.229.82.92,61613,admin,password\r\n","ok");   //服务器IP:111.229.82.92 port:61613 mqtt账号:admin 密码：password
		 
		ESP8266_SendCmd("AT+GSERVERSTATUS\r\n","ok");																	//建立服务器

		ESP8266_SendCmd("AT+SUB=sldev/123456,0\r\n","ok");               							//订阅主题 sldev/123456
		 
}




void Send_DATA(char *buffer)
{
    Usart_SendString(USART1,"AT+PUB=slapp/123456,");                  //发布主题 slapp/123456
		Usart_SendString(USART1,buffer);
		Usart_SendString(USART1,",0\r\n");
    delay_ms(20);		 
}


void Get_Clinet(void)
{
		u8 i;
		char *strx;
		strx=strstr((const char*)buf_uart1.Rebuf,(const char*)"AT+RECTOPIC=sldev/123456");//?????
		if(strx)
		{
				strx=strstr((const char*)buf_uart1.Rebuf,",");
				for(i=0;i<8;i++)
				{
						Recwifi_data[i]=strx[1+i];		  
				}
				if(Recwifi_data[0]=='S')          //S1150
				{
						if(Recwifi_data[1]=='0')
						{
								meter.auto_flag = 0;
						}
						else
						{
								meter.auto_flag = 1;
						}
						meter.led_pwm = (Recwifi_data[2]-0x30)*100+(Recwifi_data[3]-0x30)*10+(Recwifi_data[4]-0x30);
						if(meter.led_pwm>999)
						{
								meter.led_pwm = 999;
						}
				}            
			// ClearRebuf();	
   }	 
}



_Bool ESP8266_SendCmd(char *cmd, char *res)
{
		unsigned int timeOut = 500;
		Usart_SendStr_length(USART1, (char *)cmd, strlen((const char *)cmd));
		while(timeOut--)
		{		
				if (buf_uart1.flay == SET)
				{
						if (strstr((char*)(buf_uart1.Rebuf),res))
						{
								ClearRebuf();
								return 0;
						}
				}
				delay_ms(1);
		}
		ClearRebuf();
		return 1;
}



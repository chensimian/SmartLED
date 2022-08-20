#ifndef _ESP8266_H_
#define _ESP8266_H_



void Send_DATA(char *buffer);
void Get_Clinet(void);
void ESP8266_SERVER(void);
_Bool ESP8266_SendCmd(char *cmd, char *res);
#endif

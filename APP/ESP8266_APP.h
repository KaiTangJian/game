#ifndef _ESP01S_H_
#define _ESP01S_H_
#include <stdbool.h>
#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标�?

void ESP01S_Init(void);
void ESP01S_Clear(void);
void ESP01S_SendData(unsigned char *data, unsigned short len);
unsigned char *ESP01S_GetIPD(unsigned short timeOut);
// OneNET MQTT���ܺ�������
_Bool OneNET_MQTT_Connect(void);
_Bool OneNET_MQTT_Publish(char* topic, char* data, uint8_t qos);
_Bool OneNET_Upload_Game_Score(uint32_t score, uint8_t level);
_Bool OneNET_Init(void);

#endif


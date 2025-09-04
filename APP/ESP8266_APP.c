//单片机头文件
#include "main.h"
#include <stdbool.h>
//网络设备驱动
#include "ESP8266_APP.h"

//硬件驱动
#include "usart.h"
#include "USART_APP.h"  // 添加这个头文件以使用 my_printf

//C�?
#include <string.h>
#include <stdio.h>
#include <string.h>

#define ESP01S_WIFI_INFO		"AT+CWJAP=\"Xiaomi\",\"Aa18928337280\"\r\n"
#define ESP01S_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"  	//新版OneNET地址
// 添加OneNET设备
#define ONENET_PRODUCT_ID    "cd8uB9Qod2"
#define ONENET_DEVICE_ID     "01s"
#define ONENET_AUTH_INFO     "Y2dBTU5yampRSjNHQ1UyQ1NFdXdqVThETGhWaUh5Tmo="

unsigned char ESP01S_buf[128];
unsigned short ESP01S_cnt = 0, ESP01S_cntPre = 0;

uint8_t aRxBuffer;			//接收中断缓冲

//==========================================================
//	函数名称�?	ESP01S_Clear
//
//	函数功能�?	清空缓存
//
//	入口参数�?	�?
//
//	返回参数�?	�?
//
//	说明�?		
//==========================================================
void ESP01S_Clear(void)
{
	memset(ESP01S_buf, 0, sizeof(ESP01S_buf));
	ESP01S_cnt = 0;
}

//==========================================================
//	函数名称�?	ESP01S_WaitRecive
//
//	函数功能�?	等待接收完成
//
//	入口参数�?	�?
//
//	返回参数�?	REV_OK-接收完成		REV_WAIT-接收超时未完�?
//
//	说明�?		循环调用检测是否接收完�?
//==========================================================
bool ESP01S_WaitRecive(void)
{
	if(ESP01S_cnt == 0) 							//如果接收计数�?0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(ESP01S_cnt == ESP01S_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		ESP01S_cnt = 0;							//�?0接收计数
		return REV_OK;								//返回接收完成标志
	}
		
	ESP01S_cntPre = ESP01S_cnt;					//置为相同
	return REV_WAIT;								//返回接收未完成标�?
}

//==========================================================
//	函数名称	ESP01S_SendCmd
//
//	函数功能	发送命
//
//	入口参数	cmd：
//				res：需要检查的返回指令
//
//	返回参数	0-成功	1-失败
//
//	说明		
//==========================================================
bool ESP01S_SendCmd(char *cmd, char *res)
{
    unsigned char timeOut = 200;

    // 调试信息：显示发送的命令
    //my_printf(&huart1, "发送cmd: %s", cmd);
    
    // 构造完整的AT命令
    char full_cmd[256];
    int cmd_len = snprintf(full_cmd, sizeof(full_cmd), "%s", cmd);
    
    // 发送命令
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, (uint8_t*)full_cmd, cmd_len, 1000);
    
    if (status != HAL_OK) {
        my_printf(&huart1, "发送失败:HAL状态码%d\r\n", status);
        return 1;
    }
    
    // 等待响应
    while(timeOut--)
    {
        // 添加调试信息
        if (timeOut % 50 == 0) {
            my_printf(&huart1, "等待响应计数:%d,接收计数:%d\r\n", timeOut, ESP01S_cnt);
        }
        
        if(ESP01S_WaitRecive() == REV_OK)
        {
            my_printf(&huart1, "收到响应:%s\r\n", (char*)ESP01S_buf);
            
            if(strstr((const char *)ESP01S_buf, res) != NULL)
            {
                ESP01S_Clear();
                my_printf(&huart1, "命令成功:%s\r\n", cmd);
                return 0;
            }
            else
            {
                my_printf(&huart1, "响应不匹配，期望:%s\r\n", res);
            }
        }
        HAL_Delay(10);
    }
    
    my_printf(&huart1, "命令超时:%s\r\n", cmd);
    return 1;
}

//==========================================================
//	函数名称：ESP01S_Init
//	函数功能：初始化ESP01S
//	入口参数：无
//	返回参数：无
//	说明：		
//==========================================================
void ESP01S_Init(void)
{
	ESP01S_Clear();
	
	my_printf(&huart1, "AT\r\n");
	// 移除while循环，只尝试一次
	if(ESP01S_SendCmd("AT\r\n", "OK") != 0)
	{
		my_printf(&huart1, "AT命令失败ESP8266可能未连接或未响应\r\n");
		// 可以选择继续初始化其他部分或返回错误
	}
	HAL_Delay(500);
	
	// 原代码: UsartPrintf(USART_DEBUG, "1. RST\r\n");
	// 修改为使用 my_printf
	my_printf(&huart1, "1.RST\r\n");
	ESP01S_SendCmd("AT+RST\r\n", "");
	HAL_Delay(500);	
	

	
	// 原代码: UsartPrintf(USART_DEBUG, "2. CWMODE\r\n");
	// 修改为使用 my_printf
	my_printf(&huart1, "2.CWMODE\r\n");
	// 移除while循环，只尝试一次
	if(ESP01S_SendCmd("AT+CWMODE=1\r\n", "OK") != 0)     //模式1(Station),默认保存Flash
	{
		my_printf(&huart1, "CWMODE设置失败\r\n");
	}
	HAL_Delay(500);
	
	// 原代码: UsartPrintf(USART_DEBUG, "3. AT+CWDHCP\r\n");
	// 修改为使用 my_printf
	my_printf(&huart1, "3.AT+CWDHCP\r\n");
	// 移除while循环，只尝试一次
	if(ESP01S_SendCmd("AT+CWDHCP=1,1\r\n", "OK") != 0)
	{
		my_printf(&huart1, "CWDHCP设置失败\r\n");
	}
	HAL_Delay(500);
	
	// 原代码: UsartPrintf(USART_DEBUG, "4. CWJAP\r\n");
	// 修改为使用 my_printf
	my_printf(&huart1, "4.CWJAP\r\n");
	// 移除while循环，只尝试一次
	if(ESP01S_SendCmd(ESP01S_WIFI_INFO, "GOT IP") != 0)
	{
		my_printf(&huart1, "WiFi连接失败\r\n");
	}
	HAL_Delay(500);
	
	// 原代码: UsartPrintf(USART_DEBUG, "5. CIPSTART\r\n");
	// 修改为使用 my_printf
	my_printf(&huart1, "5.CIPSTART\r\n");
	// 移除while循环，只尝试一次
	if(ESP01S_SendCmd(ESP01S_ONENET_INFO, "CONNECT") != 0)
	{
		my_printf(&huart1, "TCP连接失败\r\n");
	}
	HAL_Delay(500);
	
	// 原代码: UsartPrintf(USART_DEBUG, "6. ESP01S Init OK\r\n");
	// 修改为使用 my_printf
	my_printf(&huart1, "6.ESP01S Init OK\r\n");
	OneNET_Init();
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
   if(huart == &huart2)
   {
 
	if(ESP01S_cnt >= sizeof(ESP01S_buf))  //溢出判断
	{
		ESP01S_cnt = 0;
		memset(ESP01S_buf,0x00,sizeof(ESP01S_buf));
		HAL_UART_Transmit(&huart1, (uint8_t *)"接收缓存溢出", 10,0xFFFF); 	      
	}
	else
	{
		ESP01S_buf[ESP01S_cnt++] = aRxBuffer;   //接收数据转存	
 	}
	
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);   //再开启接收中�?
    }
}

//==========================================================
//	函数名称�?	OneNET_MQTT_Connect
//
//	函数功能�?	使用MQTT AT指令连接OneNET平台（使用计算签名）
//
//	入口参数�?	�?
//
//	返回参数�?	0-成功	1-失败
//==========================================================
bool OneNET_MQTT_Connect(void)
{
    char mqtt_config[256];
    char mqtt_conn[128];
    
    // 使用你提供的完整签名参数
    snprintf(mqtt_config, sizeof(mqtt_config), 
             "AT+MQTTUSERCFG=0,1,\"01s\",\"cd8uB9Qod2\",\"version=2018-10-31&res=products%%2Fcd8uB9Qod2%%2Fdevices%%2F01s&et=1768982414&method=md5&sign=ksDkkO3wg27Ze33ihE5OXQ%%3D%%3D\",0,0,\"\"\r\n");
    
    if(ESP01S_SendCmd(mqtt_config, "OK") != 0)
    {
        my_printf(&huart1, "MQTT用户配置失败\r\n");
        return 1;
    }
    
    // 连接OneNET MQTT服务�?
    snprintf(mqtt_conn, sizeof(mqtt_conn), 
             "AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n");
    
    if(ESP01S_SendCmd(mqtt_conn, "OK") != 0)
    {
        my_printf(&huart1, "MQTT连接失败\r\n");
        return 1;
    }
    
    my_printf(&huart1, "MQTT连接成功\r\n");
    return 0;
}

//==========================================================
//	函数名称�?	OneNET_MQTT_Publish
//
//	函数功能�?	发布数据到OneNET平台
//
//	入口参数�?	topic: 主题
//				data: 数据
//				qos: 服务质量等级
//
//	返回参数�?	0-成功	1-失败
//==========================================================
bool OneNET_MQTT_Publish(char* topic, char* data, uint8_t qos)
{
    char publish_cmd[256];
    
    snprintf(publish_cmd, sizeof(publish_cmd), 
             "AT+MQTTPUB=0,\"%s\",\"%s\",%d,0\r\n", 
             topic, data, qos);
    
    if(ESP01S_SendCmd(publish_cmd, "OK") != 0)
    {
        my_printf(&huart1, "MQTT发布失败\r\n");
        return 1;
    }
    
    my_printf(&huart1, "MQTT发布成功: %s\r\n", data);
    return 0;
}

//==========================================================
//	函数名称�?	OneNET_Upload_Game_Score
//
//	函数功能�?	上传游戏分数到OneNET平台（游戏胜利时�?
//
//	入口参数�?	score: 分数
//				level: 关卡
//
//	返回参数�?	0-成功	1-失败
//==========================================================
bool OneNET_Upload_Game_Score(uint32_t score, uint8_t level)
{
    char json_data[128];
    char topic[64];
    
    // 构造JSON数据，格�?: {"id":"123","params":{"level_X_score":{"value":score}}}
    snprintf(json_data, sizeof(json_data), 
             "{\"id\":\"123\",\"params\":{\"level_%d_score\":{\"value\":%lu}}}", 
             level, score);
    
    // 构造主�?
    snprintf(topic, sizeof(topic), "$sys/%s/%s/thing/property/post", 
             ONENET_PRODUCT_ID, ONENET_DEVICE_ID);
    
    // 发布数据
    return OneNET_MQTT_Publish(topic, json_data, 0);
}

//==========================================================
//	函数名称�?	OneNET_Init
//
//	函数功能�?	初始化OneNET连接
//
//	返回参数�?	0-成功	1-失败
//==========================================================
bool OneNET_Init(void)
{
    // 连接MQTT
    if(OneNET_MQTT_Connect() != 0)
    {
        my_printf(&huart1, "OneNET连接失败\r\n");
        return 1;
    }
    
    my_printf(&huart1, "OneNET连接成功\r\n");
    return 0;
}
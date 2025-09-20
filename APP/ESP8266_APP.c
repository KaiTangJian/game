// 单片机头文件
#include "main.h"
#include <stdbool.h>
// 网络设备驱动
#include "ESP8266_APP.h"

#include "usart.h"
#include "USART_APP.h" // 添加这个头文件以使用 my_printf

#include <string.h>
#include <stdio.h>
#include <string.h>

#define ESP01S_WIFI_INFO "AT+CWJAP=\"Xiaomi\",\"Aa18928337280\"\r\n"
#define ESP01S_ONENET_INFO "AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n" // 新版OneNET地址
// 添加OneNET设备
#define ONENET_PRODUCT_ID "cd8uB9Qod2"
#define ONENET_DEVICE_ID "01s"
#define MAX_PASSWORD_LENGTH 8
unsigned char ESP01S_buf[128];
unsigned short ESP01S_cnt = 0, ESP01S_cntPre = 0;
extern char system_password[MAX_PASSWORD_LENGTH + 1];
uint8_t aRxBuffer; // 接收中断缓冲

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
    if (ESP01S_cnt == 0) // 如果接收计数�?0 则说明没有处于接收数据中，所以直接跳出，结束函数
        return REV_WAIT;

    if (ESP01S_cnt == ESP01S_cntPre) // 如果上一次的值和这次相同，则说明接收完毕
    {
        ESP01S_cnt = 0; // �?0接收计数
        return REV_OK;  // 返回接收完成标志
    }

    ESP01S_cntPre = ESP01S_cnt; // 置为相同
    return REV_WAIT;            // 返回接收未完成标�?
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
    // my_printf(&huart1, "发送cmd: %s", cmd);

    // 构造完整的AT命令
    char full_cmd[256];
    int cmd_len = snprintf(full_cmd, sizeof(full_cmd), "%s", cmd);

    // 发送命令
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, (uint8_t *)full_cmd, cmd_len, 1000);

    if (status != HAL_OK)
    {
        my_printf(&huart1, "发送失败:HAL状态码%d\r\n", status);
        return 1;
    }

    // 等待响应
    while (timeOut--)
    {
        // 添加调试信息
        if (timeOut % 50 == 0)
        {
            my_printf(&huart1, "等待响应计数:%d,接收计数:%d\r\n", timeOut, ESP01S_cnt);
        }

        if (ESP01S_WaitRecive() == REV_OK)
        {
            my_printf(&huart1, "收到响应:%s\r\n", (char *)ESP01S_buf);

            if (strstr((const char *)ESP01S_buf, res) != NULL)
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

    if (ESP01S_SendCmd("AT\r\n", "OK") != 0)
    {
        my_printf(&huart1, "AT命令失败ESP8266可能未连接或未响应\r\n");
        // 可以选择继续初始化其他部分或返回错误
    }
    HAL_Delay(500);

    my_printf(&huart1, "1.RST\r\n");
    ESP01S_SendCmd("AT+RST\r\n", "");
    HAL_Delay(500);

    my_printf(&huart1, "Disable Echo (ATE0)\r\n"); // 打印提示信息
    if (ESP01S_SendCmd("ATE0\r\n", "OK") != 0)     // 发送ATE0命令，期望收到"OK"
    {
        my_printf(&huart1, "ATE0命令失败，可能ECHO已禁用或模块无响应\r\n"); // 如果失败，打印错误
    }
    HAL_Delay(500); // 延时等待模块处理

    my_printf(&huart1, "2.CWMODE\r\n");
    // 移除while循环，只尝试一次
    if (ESP01S_SendCmd("AT+CWMODE=1\r\n", "OK") != 0) // 模式1(Station),默认保存Flash
    {
        my_printf(&huart1, "CWMODE设置失败\r\n");
    }
    HAL_Delay(500);

    my_printf(&huart1, "3.AT+CWDHCP\r\n");

    if (ESP01S_SendCmd("AT+CWDHCP=1,1\r\n", "OK") != 0)
    {
        my_printf(&huart1, "CWDHCP设置失败\r\n");
    }
    HAL_Delay(1000);

    my_printf(&huart1, "4.CWJAP\r\n");

    if (ESP01S_SendCmd(ESP01S_WIFI_INFO, "GOT IP") != 0)
    {
        my_printf(&huart1, "WiFi连接失败\r\n");
    }
    HAL_Delay(1000);

    my_printf(&huart1, "5.CIPSTART\r\n");

    if (ESP01S_SendCmd(ESP01S_ONENET_INFO, "CONNECT") != 0)
    {
        my_printf(&huart1, "TCP连接失败\r\n");
    }
    HAL_Delay(1000);

    my_printf(&huart1, "6.ESP01S Init OK\r\n");
    OneNET_Init();
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart2)
    {

        if (ESP01S_cnt >= sizeof(ESP01S_buf)) // 溢出判断
        {
            ESP01S_cnt = 0;
            memset(ESP01S_buf, 0x00, sizeof(ESP01S_buf));
            HAL_UART_Transmit(&huart1, (uint8_t *)"接收缓存溢出", 10, 0xFFFF);
        }
        else
        {
            ESP01S_buf[ESP01S_cnt++] = aRxBuffer; // 接收数据转存

            // 检查是否收到完整的一行数据

                // 检查是否收到完整的一行数据
            if (aRxBuffer == '\n' || aRxBuffer == '\r')
            {
                // 直接查找连续的四位数字，不检查MQTT消息标识和主题
                // 查找密码字段 (数字格式: "password":2111)
                char *password_start = strstr((char *)ESP01S_buf, "\"password\":");
                if (password_start != NULL)
                {
                    password_start += 10; // 跳过"password":字符串

                    // 查找密码结束位置
                    char *password_end = strchr(password_start, '}');
                    if (password_end != NULL)
                    {
                        *password_end = '\0'; // 临时结束字符串

                        // 去除空格
                        while (*password_start == ' ')
                        {
                            password_start++;
                        }

                        // 验证是否为纯数字
                        char *temp = password_start;
                        bool is_valid_number = true;
                        while (*temp != '\0' && *temp != ',' && *temp != '}' && *temp != ' ')
                        {
                            if (!isdigit((unsigned char)*temp))
                            {
                                is_valid_number = false;
                                break;
                            }
                            temp++;
                        }

                        if (is_valid_number)
                        {
                            // 提取数字密码
                            char password_str[16];
                            int len = temp - password_start;
                            if (len < sizeof(password_str) - 1)
                            {
                                strncpy(password_str, password_start, len);
                                password_str[len] = '\0';

                                // 更新系统密码
                                extern char system_password[];
                                if (strlen(password_str) <= 8) // MAX_PASSWORD_LENGTH
                                {
                                    strncpy(system_password, password_str, 8);
                                    system_password[8] = '\0';
                                    my_printf(&huart1, "密码已更新为: %s\r\n", system_password);

                                    // 发送响应确认
                                    char response_topic[64];
                                    char response_payload[128];

                                    snprintf(response_topic, sizeof(response_topic),
                                             "$sys/%s/%s/thing/property/set_reply", ONENET_PRODUCT_ID, ONENET_DEVICE_ID);

                                    snprintf(response_payload, sizeof(response_payload),
                                             "{\\\"id\\\":\\\"123\\\"\\,\\\"code\\\":200\\,\\\"msg\\\":\\\"success\\\"}");

                                    OneNET_MQTT_Publish(response_topic, response_payload, 0);
                                }
                                else
                                {
                                    my_printf(&huart1, "新密码长度超限\r\n");
                                }
                            }
                        }
                        else
                        {
                            my_printf(&huart1, "密码格式错误，应为纯数字\r\n");
                        }

                        *password_end = '}'; // 恢复原字符
                    }
                }
            
                // 清空缓冲区
                //ESP01S_Clear();
            }
        }

        HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1); // 再开启接收中�?
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

    if (ESP01S_SendCmd(mqtt_config, "OK") != 0)
    {
        my_printf(&huart1, "MQTT用户配置失败\r\n");
        return 1;
    }

    // 连接OneNET MQTT服务�?
    snprintf(mqtt_conn, sizeof(mqtt_conn),
             "AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n");

    if (ESP01S_SendCmd(mqtt_conn, "OK") != 0)
    {
        my_printf(&huart1, "MQTT连接失败\r\n");
        return 1;
    }

    my_printf(&huart1, "MQTT连接成功\r\n");
    return 0;
}

bool OneNET_MQTT_Publish(char *topic, char *data, uint8_t qos)
{
    char publish_cmd[256];

    // 构造AT命令
    int len = snprintf(publish_cmd, sizeof(publish_cmd),
                       "AT+MQTTPUB=0,\"%s\",\"%s\",%d,0\r\n",
                       topic, data, qos);

    my_printf(&huart1, "AT命令长度: %d 字节\r\n", len);
    my_printf(&huart1, "发送命令: %s", publish_cmd);

    // 增加超时时间和重试机制
    for (int retry = 0; retry < 3; retry++)
    {
        my_printf(&huart1, "第%d次尝试发布数据\r\n", retry + 1);

        // 发送命令并等待响应
        unsigned char timeOut = 200; // 增加超时时间到2000
        char full_cmd[256];
        int cmd_len = snprintf(full_cmd, sizeof(full_cmd), "%s", publish_cmd);

        // 发送命令
        HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, (uint8_t *)full_cmd, cmd_len, 2000);

        if (status != HAL_OK)
        {
            my_printf(&huart1, "发送失败:HAL状态码%d\r\n", status);
            HAL_Delay(1000);
            continue;
        }

        // 清空接收缓冲区
        ESP01S_Clear();

        // 等待响应
        while (timeOut--)
        {
            if (ESP01S_WaitRecive() == REV_OK)
            {
                my_printf(&huart1, "收到完整响应:%s\r\n", (char *)ESP01S_buf);

                // 检查多种成功响应
                if (strstr((const char *)ESP01S_buf, "OK") != NULL)
                {
                    ESP01S_Clear();
                    my_printf(&huart1, "MQTT发布成功\r\n");
                    return 0;
                }
                else if (strstr((const char *)ESP01S_buf, "SEND OK") != NULL)
                {
                    ESP01S_Clear();
                    my_printf(&huart1, "MQTT数据发送成功\r\n");
                    return 0;
                }
                else if (strstr((const char *)ESP01S_buf, "ERROR") != NULL)
                {
                    my_printf(&huart1, "MQTT发布错误\r\n");
                    ESP01S_Clear();
                    break; // 不再重试
                }
                else
                {
                    my_printf(&huart1, "响应不匹配，继续等待...\r\n");
                    ESP01S_Clear();
                }
            }
            HAL_Delay(15);
        }

        my_printf(&huart1, "第%d次尝试超时\r\n", retry + 1);
        if (retry < 2)
        {
            HAL_Delay(2000); // 重试前等待
        }
    }

    my_printf(&huart1, "MQTT发布最终失败\r\n");
    return 1;
}

bool OneNET_Upload_Game_Score(uint32_t score, uint8_t level)
{
    char json_data[128];
    char topic[64];

    char onenet_product_id[100] = ONENET_PRODUCT_ID;
    char onenet_device_id[100] = ONENET_DEVICE_ID;

    // 构造JSON数据，正确转义双引号
    snprintf(json_data, sizeof(json_data),
             "{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"level_%d_score\\\":{\\\"value\\\":%lu\\}}}",
             level, score);

    // 构造主题
    snprintf(topic, sizeof(topic), "$sys/%s/01s/thing/property/post",
             onenet_product_id);

    my_printf(&huart1, "准备上传分数: 关卡=%d, 分数=%lu\r\n", level, score);
    my_printf(&huart1, "JSON数据: %s\r\n", json_data);
    my_printf(&huart1, "主题: %s\r\n", topic);

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
    char subscribe_cmd[128];
    char topic[64];
    // 连接MQTT
    if (OneNET_MQTT_Connect() != 0)
    {
        my_printf(&huart1, "OneNET连接失败\r\n");
        return 1;
    }

    my_printf(&huart1, "OneNET连接成功\r\n");

    // 构造订阅主题 $sys/{pid}/{device-name}/thing/property/set
    snprintf(topic, sizeof(topic), "$sys/%s/%s/thing/property/set",
             ONENET_PRODUCT_ID, ONENET_DEVICE_ID);

    // 构造AT命令
    snprintf(subscribe_cmd, sizeof(subscribe_cmd),
             "AT+MQTTSUB=0,\"%s\",0\r\n", topic);

    my_printf(&huart1, "订阅属性设置主题: %s\r\n", topic);

    if (ESP01S_SendCmd(subscribe_cmd, "OK") != 0)
    {
        my_printf(&huart1, "订阅属性设置主题失败\r\n");
        return 1;
    }

    my_printf(&huart1, "订阅属性设置主题成功\r\n");
    return 0;
}
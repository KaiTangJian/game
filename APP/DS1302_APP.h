#ifndef __DS1302_APP_H
#define __DS1302_APP_H
#include "gpio.h"
#include "USART_APP.h"

#define W_SECOND 0x80
#define W_MINUTE 0x82
#define W_HOUR   0x84
#define W_DAY    0x86
#define W_MONTH  0x88
#define W_WEEK   0x8A
#define W_YEAR   0x8C
#define W_CONTR  0x8E
 
#define R_SECOND 0x81
#define R_MINUTE 0x83
#define R_HOUR   0x85
#define R_DAY    0x87
#define R_MONTH  0x89
#define R_WEEK   0x8B
#define R_YEAR   0x8D
#define R_CONTR  0x8F
 
#define RST_L     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET)
#define RST_H     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET)
#define CLK_L     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET)
#define CLK_H     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET)
#define DATA_L    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET)
#define DATA_H    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET)
 
#define DATA_READ HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)

extern UART_HandleTypeDef huart1;
void DS1302_Set_Time(uint8_t year,uint8_t week,uint8_t month,uint8_t day,uint8_t hour,uint8_t minute,uint8_t second);
typedef struct {
	uint8_t year;
	uint8_t week;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}timeNow_t;
timeNow_t DS1302_Read_Time(void);
void DS1302_Print_Time(void);
uint8_t bcd_to_binary(uint8_t bcd);
uint8_t binary_to_bcd(uint8_t binary);
#endif


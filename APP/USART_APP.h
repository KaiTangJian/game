#ifndef __USART_APP_H__
#define __USART_APP_H__
#include "gpio.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

extern UART_HandleTypeDef huart1;
int my_printf(UART_HandleTypeDef *huart, const char *format, ...);
void uart_task(void);

#endif

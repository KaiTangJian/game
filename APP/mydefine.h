#ifndef my_define_H
#define my_define_H



#include "gpio.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "math.h"



#include "KEY_APP.h"
#include "USART_APP.h"
#include "Scheduler.h"
#include "Lcd.h"
#include "lvgl.h"
#include "lv_port_disp.h"

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_spi1_tx;
/*
lcd : spi1 mosi pb5
miso pb4
sck pb3
cs pb15
dc pb14
led pb13v l
rst pb12

*/
#endif

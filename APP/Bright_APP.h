#ifndef __Bright_APP_H
#define __Bright_APP_H

#include "gpio.h"
#include "adc.h"
#include "USART_APP.h"
#define ADC_MAX_VALUE 4095     // 12位ADC最大�?
#define PWM_MAX_DUTY_CYCLE 999 // PWM计数周期值（与Counter Period一致）
#define BRIGHTNESS_MIN 30      // 最小亮度对应的PWM值（避免完全熄灭�?
#define BRIGHTNESS_MAX 999     // 最大亮度对应的PWM�?
void adc_dma_init(void);
void adc_task(void);
void Update_Action_Time(void);
void Turn_On(void);
void Turn_Off(void);
#endif

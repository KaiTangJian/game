
#include "Bright_APP.h"
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim4;
// --- 全局变量 ---
#define ADC_DMA_BUFFER_SIZE 32                // DMA缓冲区大小，可以根据需要调整
uint32_t adc_dma_buffer[ADC_DMA_BUFFER_SIZE]; // DMA 目标缓冲区
__IO uint32_t adc_val;                        // 用于存储计算后的平均 ADC 值
__IO float voltage;                           // 用于存储计算后的电压值

// --- 初始化 (通常在 main 函数或外设初始化函数中调用一次) ---
void adc_dma_init(void)
{
    // 启动 ADC 并使能 DMA 传输
    // hadc1: ADC 句柄
    // (uint32_t*)adc_dma_buffer: DMA 目标缓冲区地址 (HAL库通常需要uint32_t*)
    // ADC_DMA_BUFFER_SIZE: 本次传输的数据量 (缓冲区大小)
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buffer, ADC_DMA_BUFFER_SIZE);
}

// --- 处理任务 (在主循环或定时器回调中定期调用) ---
void adc_task(void)
{
    uint32_t adc_sum = 0;
    uint32_t pwm_duty_cycle = 0;
    // 1. 计算 DMA 缓冲区中所有采样值的总和
    //    注意：这里直接读取缓冲区，可能包含不同时刻的采样值
    for (uint16_t i = 0; i < ADC_DMA_BUFFER_SIZE; i++)
    {
        adc_sum += adc_dma_buffer[i];
    }

    adc_val = adc_sum / ADC_DMA_BUFFER_SIZE;
    my_printf(&huart1, "ADC Value: %lu\n", adc_val);
    //     uint32_t voltage_mv = (adc_val * 3300) / 4096; // 转换为毫伏特
    //			my_printf(&huart1, "Average ADC: %lu, Voltage: %lu.%03luV\n", adc_val, voltage_mv/1000, voltage_mv%1000);

    if (adc_val > ADC_MAX_VALUE)
        adc_val = ADC_MAX_VALUE;
    //		 pwm_duty_cycle = (uint32_t)(((float)adc_val / ADC_MAX_VALUE) * (BRIGHTNESS_MAX - BRIGHTNESS_MIN) + BRIGHTNESS_MIN);
    pwm_duty_cycle = (uint32_t)(((1.0f - (float)adc_val / ADC_MAX_VALUE) * (BRIGHTNESS_MAX - BRIGHTNESS_MIN)) + BRIGHTNESS_MIN);
    my_printf(&huart1, "pwm_duty_cycle: %lu\n", pwm_duty_cycle);
    if (pwm_duty_cycle > BRIGHTNESS_MAX)
    {
        pwm_duty_cycle = BRIGHTNESS_MAX;
    }
    if (pwm_duty_cycle < BRIGHTNESS_MIN)
    {
        pwm_duty_cycle = BRIGHTNESS_MIN;
    }

    // 鏇存柊PWM鍗犵┖姣�
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm_duty_cycle);
}

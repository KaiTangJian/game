#define ADC_MAX_VALUE       4095      // 12位ADC最大值
#define PWM_MAX_DUTY_CYCLE  999       // PWM计数周期值（与Counter Period一致）
#define BRIGHTNESS_MIN      30        // 最小亮度对应的PWM值（避免完全熄灭）
#define BRIGHTNESS_MAX      999       // 最大亮度对应的PWM值
#include "Bright_APP.h"
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart1;
extern	TIM_HandleTypeDef htim4; 
// --- ȫ�ֱ��� --- 
#define ADC_DMA_BUFFER_SIZE 32 // DMA��������С�����Ը�����Ҫ����
uint32_t adc_dma_buffer[ADC_DMA_BUFFER_SIZE]; // DMA Ŀ�껺����
__IO uint32_t adc_val;  // ���ڴ洢������ƽ�� ADC ֵ
__IO float voltage; // ���ڴ洢�����ĵ�ѹֵ

// --- ��ʼ�� (ͨ���� main �����������ʼ�������е���һ��) ---
void adc_dma_init(void)
{
    // ���� ADC ��ʹ�� DMA ����
    // hadc1: ADC ���
    // (uint32_t*)adc_dma_buffer: DMA Ŀ�껺������ַ (HAL��ͨ����Ҫuint32_t*)
    // ADC_DMA_BUFFER_SIZE: ���δ���������� (��������С)
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma_buffer, ADC_DMA_BUFFER_SIZE);
}

// --- �������� (����ѭ����ʱ���ص��ж��ڵ���) ---
void adc_task(void)
{
    uint32_t adc_sum = 0;
    uint32_t pwm_duty_cycle = 0;
    // 1. ���� DMA �����������в���ֵ���ܺ�
    //    ע�⣺����ֱ�Ӷ�ȡ�����������ܰ�����ͬʱ�̵Ĳ���ֵ
    for(uint16_t i = 0; i < ADC_DMA_BUFFER_SIZE; i++)
    {
        adc_sum += adc_dma_buffer[i];
    }

   
    adc_val = adc_sum / ADC_DMA_BUFFER_SIZE; 
			my_printf(&huart1, "ADC Value: %lu\n", adc_val);
//     uint32_t voltage_mv = (adc_val * 3300) / 4096; // ת��Ϊ������    
//			my_printf(&huart1, "Average ADC: %lu, Voltage: %lu.%03luV\n", adc_val, voltage_mv/1000, voltage_mv%1000);
		
		   if (adc_val > ADC_MAX_VALUE) adc_val = ADC_MAX_VALUE;
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

        // 更新PWM占空比
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm_duty_cycle);
}


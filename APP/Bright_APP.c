#define ADC_MAX_VALUE       4095      // 12ä½ADCæœ€å¤§å€¼
#define PWM_MAX_DUTY_CYCLE  999       // PWMè®¡æ•°å‘¨æœŸå€¼ï¼ˆä¸Counter Periodä¸€è‡´ï¼‰
#define BRIGHTNESS_MIN      30        // æœ€å°äº®åº¦å¯¹åº”çš„PWMå€¼ï¼ˆé¿å…å®Œå…¨ç†„ç­ï¼‰
#define BRIGHTNESS_MAX      999       // æœ€å¤§äº®åº¦å¯¹åº”çš„PWMå€¼
#include "Bright_APP.h"
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart1;
extern	TIM_HandleTypeDef htim4; 
// --- È«¾Ö±äÁ¿ --- 
#define ADC_DMA_BUFFER_SIZE 32 // DMA»º³åÇø´óĞ¡£¬¿ÉÒÔ¸ù¾İĞèÒªµ÷Õû
uint32_t adc_dma_buffer[ADC_DMA_BUFFER_SIZE]; // DMA Ä¿±ê»º³åÇø
__IO uint32_t adc_val;  // ÓÃÓÚ´æ´¢¼ÆËãºóµÄÆ½¾ù ADC Öµ
__IO float voltage; // ÓÃÓÚ´æ´¢¼ÆËãºóµÄµçÑ¹Öµ

// --- ³õÊ¼»¯ (Í¨³£ÔÚ main º¯Êı»òÍâÉè³õÊ¼»¯º¯ÊıÖĞµ÷ÓÃÒ»´Î) ---
void adc_dma_init(void)
{
    // Æô¶¯ ADC ²¢Ê¹ÄÜ DMA ´«Êä
    // hadc1: ADC ¾ä±ú
    // (uint32_t*)adc_dma_buffer: DMA Ä¿±ê»º³åÇøµØÖ· (HAL¿âÍ¨³£ĞèÒªuint32_t*)
    // ADC_DMA_BUFFER_SIZE: ±¾´Î´«ÊäµÄÊı¾İÁ¿ (»º³åÇø´óĞ¡)
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma_buffer, ADC_DMA_BUFFER_SIZE);
}

// --- ´¦ÀíÈÎÎñ (ÔÚÖ÷Ñ­»·»ò¶¨Ê±Æ÷»Øµ÷ÖĞ¶¨ÆÚµ÷ÓÃ) ---
void adc_task(void)
{
    uint32_t adc_sum = 0;
    uint32_t pwm_duty_cycle = 0;
    // 1. ¼ÆËã DMA »º³åÇøÖĞËùÓĞ²ÉÑùÖµµÄ×ÜºÍ
    //    ×¢Òâ£ºÕâÀïÖ±½Ó¶ÁÈ¡»º³åÇø£¬¿ÉÄÜ°üº¬²»Í¬Ê±¿ÌµÄ²ÉÑùÖµ
    for(uint16_t i = 0; i < ADC_DMA_BUFFER_SIZE; i++)
    {
        adc_sum += adc_dma_buffer[i];
    }

   
    adc_val = adc_sum / ADC_DMA_BUFFER_SIZE; 
			my_printf(&huart1, "ADC Value: %lu\n", adc_val);
//     uint32_t voltage_mv = (adc_val * 3300) / 4096; // ×ª»»ÎªºÁ·üÌØ    
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

        // æ›´æ–°PWMå ç©ºæ¯”
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm_duty_cycle);
}


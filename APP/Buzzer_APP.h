#ifndef Buzzer_APP_H
#define Buzzer_APP_H

#include "main.h" 
#include "stdbool.h"

#define P0 	0	// 休止符频率
#define L1 262  // 低音频率
#define L2 294
#define L3 330
#define L4 349
#define L5 392
#define L6 440
#define L7 494

#define M1 523  // 中音频率
#define M2 587
#define M3 659
#define M4 698
#define M5 784
#define M6 880
#define M7 988

#define H1 1047 // 高音频率
#define H2 1175
#define H3 1319
#define H4 1397
#define H5 1568
#define H6 1760
#define H7 1976

// --- 音符结构体定义 ---
typedef struct {
    uint16_t frequency;     // 音高频率 (Hz)，0 代表静音，0xFFFF 代表旋律结束
    float period;   // 持续时间 (毫秒)
} BuzzerNote_t;

// --- 外部声明 (全局变量) ---
extern TIM_HandleTypeDef htim3; 
void Buzzer_APP();
uint32_t TIM_GetCounterFreq(TIM_HandleTypeDef *htim);
//// 音量级别定义
//#define MAX_VOLUME 255
//#define MIN_VOLUME 0

//extern volatile uint8_t master_volume_level; // 主音量
//extern volatile int32_t encoder_rotation_delta; // 编码器旋转量

//// 编码器 GPIO 定义
//#define ENCODER_A_GPIO_PORT GPIOA
//#define ENCODER_A_GPIO_PIN  GPIO_PIN_0
//#define ENCODER_B_GPIO_PORT GPIOA
//#define ENCODER_B_GPIO_PIN  GPIO_PIN_1

//extern volatile uint16_t current_bgm_frequency; // BGM 当前播放频率
//extern volatile uint16_t current_sfx_frequency; // SFX 当前播放频率
//extern volatile bool sfx_is_playing; // SFX 播放状态

//// --- 函数声明 ---
//uint32_t map_volume_to_duty_cycle(uint8_t volume_level);
//void BGM_Buzzer_SetFrequency(uint16_t frequency_hz);
//void SFX_Buzzer_SetFrequency(uint16_t frequency_hz);

//void StartBGMPlaybackTask(void *pvParameters);
//void StartSFXPlaybackTask(void *pvParameters);
//void StartVolumeControlTask(void *pvParameters);
//void PlaySFX(const BuzzerNote_t *sfx_data);

 
#endif

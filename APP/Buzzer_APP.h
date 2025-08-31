#ifndef __Buzzer_APP_H
#define __Buzzer_APP_H

#include "main.h" 
#include "stdbool.h"

#define P0 	0	// 休止符频�?
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

// 音量控制相关定义
#define VOLUME_MIN 0
#define VOLUME_MAX 100
#define DEFAULT_VOLUME 50
// --- 音符结构体定�? ---
typedef struct {
    uint16_t frequency;     // 音高频率 (Hz)�?0 代表静音�?0xFFFF 代表旋律结束
    float period;   // 持续时间 (毫秒)
} BuzzerNote_t;

// --- 外部声明 (全局变量) ---
extern TIM_HandleTypeDef htim3; 
extern UART_HandleTypeDef huart1;
void Buzzer_APP();
uint32_t TIM_GetCounterFreq(TIM_HandleTypeDef *htim);
void Set_Volume(uint8_t volume);
void Encoder_Control_Volume(int16_t encoder_diff);
uint8_t get_volume(void);
#endif

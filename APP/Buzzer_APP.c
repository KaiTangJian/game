#include "Buzzer_APP.h"

uint8_t playState = 1;
uint32_t timFrequency = 1000000;
const uint8_t bpm = 132;
uint32_t playIndex = 0;
float noteDuration = 1000 * 60 / 132;
static uint8_t current_volume = DEFAULT_VOLUME;
static uint32_t last_encoder_time = 0;
static uint8_t volume_multiplier = 1;

const BuzzerNote_t MoChouXiang[] =
    {
        // 我被困在�??
        {M6, 1},
        {M5, 1},
        {M3, 1},
        {M5, 0.5f},
        {M5, 0.5f},
        // 这片混沌 柳暗
        {M6, 0.5f},
        {M5, 1},
        {M5, 0.5f},
        {M3, 0.5f},
        {M3, 0.5f},
        {M3, 0.5f},
        {M3, 0.5f},
        // 花明 一村一村一�??
        {M2, 0.5f},
        {M3, 0.5f},
        {M5, 0.5f},
        {M3, 0.5f},
        {M2, 0.5f},
        {M3, 0.5f},
        {M5, 0.5f},
        {M3, 0.5f},
        // 一村又一�??
        {M2, 0.5f},
        {M3, 0.5f},
        {M3, 0.5f},
        {L7, 0.5f},
        {M3, 1},
        {M1, 1},
        // 不能理顺�??
        {M2, 1},
        {M3, 1},
        {M2, 1},
        {M3, 0.5f},
        {M3, 0.5f},

        // ......

        // 娃儿抬头�??
        {M3, 0.5f},
        {M2, 0.5f},
        {M2, 0.5f},
        {M3, 0.5f},
        {M3, 0.5f},
        {M2, 1.5f},
        // 姥姥在天�??
        {M1, 0.5f},
        {M3, 0.5f},
        {M2, 0.5f},
        {M3, 0.5f},
        {M2, 1},
        {M1, 1},
};
const BuzzerNote_t Sound_Effect[] =
{

        {M6, 1},
};

void Set_Volume(uint8_t volume)
{
  if (volume > VOLUME_MAX)
    current_volume = VOLUME_MAX;
  else if (volume < VOLUME_MIN)
  {
    current_volume = VOLUME_MIN;
  }
  else
    current_volume = volume;
}

// 获取当前音量
uint8_t get_volume(void)
{
  return current_volume;
}
void Encoder_Control_Volume(int16_t encoder_diff)
{
  uint32_t current_time = HAL_GetTick();
  uint32_t time_diff = current_time - last_encoder_time;
  // 转速越快对应的变化因数越大
  if (time_diff < 50)
  {
    volume_multiplier = 5;
  }
  else if (time_diff < 100 && time_diff >= 50)
  {
    volume_multiplier = 3;
  }
  else if (time_diff < 200 && time_diff >= 100)
  {
    volume_multiplier = 2;
  }
  else
  {
    volume_multiplier = 1;
  }

  int16_t volume_change = encoder_diff * volume_multiplier;
  int16_t new_volume = (int16_t)current_volume + volume_change;
  // 限制音量范围 - 不循环，到边界就停止
  if (new_volume > VOLUME_MAX)
  {
    new_volume = VOLUME_MAX; // �?100就停止，不循环到0
  }
  else if (new_volume < VOLUME_MIN)
  {
    new_volume = VOLUME_MIN; // �?0就停止，不循环到100
  }
  Set_Volume(new_volume);
  last_encoder_time = current_time;
}

void Buzzer_APP(void)
{

  const BuzzerNote_t bate = MoChouXiang[playIndex];

  if (bate.frequency == P0)
  {
    // 休止�??
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
  }
  else
  {

    // 将频率转换为计数�??, 设置到自动重装载寄存�??
    uint32_t arr = timFrequency / bate.frequency;
    __HAL_TIM_SET_AUTORELOAD(&htim3, arr);
    uint32_t duty_cycle = (arr * current_volume * 20 / 100) / 100;
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, duty_cycle);
    // 重置PWM波形
    __HAL_TIM_SetCounter(&htim3, 0);
  }
  // 延时该音符的持续时间 (5ms的空白以区分连续两个相同的音�??)
  HAL_Delay((uint32_t)(bate.period * noteDuration) - 5);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
  HAL_Delay(5);

  // 下一个音�??
  playIndex++;
  // 播放结束
  if (playIndex >= (sizeof(MoChouXiang) / sizeof(MoChouXiang[0])))
  {

    playIndex = 0; // 重置索引
  }
}

void play_collect_item_sound(void)
{
  // 播放音效序列
  for (int i = 0; i < sizeof(Sound_Effect) / sizeof(Sound_Effect[0]); i++)
  {
    const BuzzerNote_t note = Sound_Effect[i];

    if (note.frequency == P0)
    {
      // 休止符
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
    }
    else
    {
      // 播放音符 - 使用与背景音乐相同的架构
      uint32_t arr = timFrequency / note.frequency;
      __HAL_TIM_SET_AUTORELOAD(&htim2, arr);
      uint32_t duty_cycle = (arr * current_volume * 20 / 100) / 100;
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, duty_cycle);
    }

    // 延时该音符的持续时间
    HAL_Delay((uint32_t)(note.period * noteDuration) - 5);
  }

  // 停止音效播放
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
}

//#include "Buzzer_APP.h"

//uint8_t playState = 1;
//uint32_t timFrequency = 2000;
//const uint8_t bpm = 132;
//uint32_t playIndex = 0;
//float noteDuration = 1000 * 60 / bpm;
//const BuzzerNote_t MoChouXiang[] = 
//	{
//    // 我被困在了
//    {M6, 1}, {M5, 1}, {M3, 1}, {M5, 0.5f}, {M5, 0.5f},
//    // 这片混沌 柳暗
//    {M6, 0.5f}, {M5, 1}, {M5, 0.5f}, {M3, 0.5f}, {M3, 0.5f}, {M3, 0.5f}, {M3, 0.5f},
//    // 花明 一村一村一村
//    {M2, 0.5f}, {M3, 0.5f}, {M5, 0.5f}, {M3, 0.5f}, {M2, 0.5f}, {M3, 0.5f}, {M5, 0.5f}, {M3, 0.5f},
//    // 一村又一村
//    {M2, 0.5f}, {M3, 0.5f}, {M3, 0.5f}, {L7, 0.5f}, {M3, 1}, {M1, 1},
//    // 不能理顺我
//    {M2, 1}, {M3, 1}, {M2, 1}, {M3, 0.5f}, {M3, 0.5f},
//    
//    // ......
//    
//    // 娃儿抬头望
//    {M3, 0.5f}, {M2, 0.5f}, {M2, 0.5f}, {M3, 0.5f}, {M3, 0.5f}, {M2, 1.5f},
//    // 姥姥在天上
//    {M1, 0.5f}, {M3, 0.5f}, {M2, 0.5f}, {M3, 0.5f}, {M2, 1}, {M1, 1},
//  };  
//	
//	
//void Buzzer_APP()
//{
//	if (playState)
//		{
//  const BuzzerNote_t bate = MoChouXiang[playIndex];
//  if (bate.frequency == P0) 
//	{
//    // 休止符
//    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
//  } 
//	else 
//	{
//    // 将频率转换为计数值, 设置到自动重装载寄存器
//    uint32_t arr = timFrequency / bate.frequency;
//    __HAL_TIM_SET_AUTORELOAD(&htim3,arr);
//    // 设置占空比为20%
//    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, arr / 5); // 20%占空比
//    // 从0开始计数 重置PWM波形
//    __HAL_TIM_SetCounter(&htim3, 0);
//  }
//  // 延时该音符的持续时间 (5ms的空白以区分连续两个相同的音符)
//  HAL_Delay((uint32_t) (bate.period* noteDuration) - 5);
//  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
//  HAL_Delay(5);

//  // 下一个音符
//  playIndex++;
//		// 播放结束
//		if (playIndex >= sizeof(MoChouXiang))
//		{
//    playState = 0; // 停止播放
//    playIndex = 0; // 重置索引
//		}
//	}
//	else
//	{
//		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
//	}
//}
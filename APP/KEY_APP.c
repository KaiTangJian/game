#include "mydefine.h"
#include "UI_Manager.h"
uint8_t key_val,key_down,key_old,key_up;
uint8_t key_read()
{
	uint8_t temp = 0;

  if(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_3) == RESET) temp = 1;
	if(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_4) == RESET) temp = 2;
	if(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_5) == RESET) temp = 3;
	if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_6) == RESET) temp = 4;
	return temp;

}
void key_proc()
{
    key_val = key_read();
    key_down = key_val & (key_old ^ key_val); // 检测按键按下事
    key_up = ~key_val & (key_old ^ key_val);   // 检测按键释放事
    key_old = key_val;                          // 更新旧按键状



		if (key_down != 0) // 只有当有按键按下时才执行以下逻辑
		{
        switch (Current_State)
        {
            case UI_STATE_START: // 当前在开始界
                if (key_down == 3) // 在开始界面按下按
                {
                    Current_State = UI_STATE_SELECT; // 进入选择界面
                }
                // 如果开始界面有其他按键功能，可以在这里添加
                break;

            case UI_STATE_SELECT: // 当前在选择界面
                if (key_down == 1) // 按键1: 选择上一
                {
                    Select_Number--;
                    if (Select_Number < 1) 
                    {
                        Select_Number = 5; 
                    }
                    update_level_labels_highlight(); 
                }
                else if (key_down == 2) // 按键2: 选择下一
                {
                    Select_Number++;
                    if (Select_Number > 5) 
                    {
                        Select_Number = 1; // 循环回到
                    }
                    update_level_labels_highlight(); 
                }
                else if (key_down == 3) // 按键3: 确认选择并进入游戏
                {
                    Current_State = UI_STATE_IN_GAMME; // 进入游戏界面
                }
                else if (key_down == 4) // 按键4: 从选择界面退出到主页
                {
                    Current_State = UI_STATE_START; // 返回开始界面
                }
                break;

            case UI_STATE_IN_GAMME: // 当前在游戏界
                if (key_down == 4) // 按键4: 从游戏界面退出到主页
                {
                    Current_State = UI_STATE_START; // 返回开始界面
                }
                // 在游戏界面，可以添加其他游戏操作的按键逻辑
                // 例如：else if (key_down == 1) { // 游戏操作1 }
                break;
						case UI_STATE_WON:
						     if (key_down == 4) // 按键4: 退出到主页
                {
                    Current_State = UI_STATE_START; // 返回开始界面
                }	
								break;	
						case UI_STATE_LOSE:
						     if (key_down == 4) // 按键4: 退出到主页
                {
                    Current_State = UI_STATE_START; // 返回开始界面
                }	
								break;								

            // 可以根据需要添加其他UI状态的处理
        }
    }

	
}
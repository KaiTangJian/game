#include "FreeRTOS.h"
#include "task.h"
#include "mydefine.h"
#include "queue.h"
#include "semphr.h"
#include "Bright_APP.h"
#include "adc.h"
#include "Lcd.h"
#include "UI_Manager.h"
// 启动任务配置
#define START_TASK_STACK_SIZE 130
#define START_TASK_PRIORITY 4
TaskHandle_t Start_Task_Handle;
void Start_Task(void *pvParameters);

// 任务LvHandler_Task配置
#define START_LvHandler_Task_STACK_SIZE 1024
#define START_LvHandler_Task_PRIORITY 4
TaskHandle_t Start_LvHandler_Task_Handle;
void LvHandler_Task(void *pvParameters);
//// 任务2配置
#define START_TASK2_STACK_SIZE 512
#define START_TASK2_PRIORITY 3
TaskHandle_t Start_Task2_Handle;
void Task2(void *pvParameters);

// 任务3配置
#define START_TASK3_STACK_SIZE 256
#define START_TASK3_PRIORITY 1
TaskHandle_t Start_Task3_Handle;
void Task3(void *pvParameters);

//游戏逻辑任务
#define GAME_LOGIC_TASK_STACK_SIZE 1024
#define GAME_LOGIC_TASK_PRIORITY 2
TaskHandle_t Game_Logic_Task_Handle;
void Game_Logic_Task(void *pvParameters);

//输入处理任务
#define INPUT_TASK_STACK_SIZE 256
#define INPUT_TASK_PRIORITY 3
TaskHandle_t Input_Task_Handle;
void Input_Task(void *pvParameters);

// 启动
QueueHandle_t queue1;
QueueHandle_t queue2;
QueueHandle_t lvgl_mutex;
uint8_t Mode = 0;
uint8_t Tx_cnt;
uint8_t playerId;
float accX;
float accY;
float accZ;
float gyroX;
float gyroY;
float gyroZ;

extern GameState_t current_game_state;     // 现在的游戏状�?
extern const Level_t *current_level_data;  // 关卡�?
extern GamePlayer_t current_player1_state; // 冰人状�?
extern GamePlayer_t current_player2_state; // 火人状�?
extern uint32_t current_game_score;        // 游戏分数
extern uint32_t remaining_game_time_sec;   // 剩余游戏时间
typedef struct
{
    uint8_t Player_ID;
    float AX;
    float AY;
    float AZ;
    float GX;
    float GY;
    float GZ;
} Remote_Data;
Remote_Data Data1_Receive;

// void handlePlayerMovement(uint8_t playerId, float accX, float accY, float accZ, float gyroX, float gyroY, float gyroZ)
//	{
//     // 鎺у埗宸﹀彸绉诲姩
//     if (accX > 0.1) {
//         moveRight();
//     }
//		else if (accX < -0.1)
//		{
//         moveLeft();
//     }
//		else
//		{
//         stopMovement();
//     }
// }

void FreeRTOS_Start()
{
    queue1 = xQueueCreate(2, sizeof(uint8_t));
    queue2 = xQueueCreate(1, sizeof(char *));
    lvgl_mutex = xSemaphoreCreateMutex();
    if (lvgl_mutex == NULL)
    {
        // 创建失败，打印日志或处理错误
        my_printf(&huart1, "lvgl_mutex 创建失败！\r\n");
    }
    // 创建启动任务
    xTaskCreate((TaskFunction_t)Start_Task,
                (char *)"Start_Task",
                (configSTACK_DEPTH_TYPE)START_TASK_STACK_SIZE,
                (void *)NULL,
                (UBaseType_t)START_TASK_PRIORITY,
                (TaskHandle_t *)&Start_Task_Handle);
    // 启动调度�? 自动创建空闲任务
    vTaskStartScheduler();
}

void Start_Task(void *pvParameters)
{
    // 创建任务1
    taskENTER_CRITICAL();
    xTaskCreate((TaskFunction_t)LvHandler_Task,
                (char *)"LvHandler_Task",
                (configSTACK_DEPTH_TYPE)START_LvHandler_Task_STACK_SIZE,
                (void *)NULL,
                (UBaseType_t)START_LvHandler_Task_PRIORITY,
                (TaskHandle_t *)&Start_LvHandler_Task_Handle);

    // 创建任务2
    xTaskCreate((TaskFunction_t)Task2,
                (char *)"Task2",
                (configSTACK_DEPTH_TYPE)START_TASK2_STACK_SIZE,
                (void *)NULL,
                (UBaseType_t)START_TASK2_PRIORITY,
                (TaskHandle_t *)&Start_Task2_Handle);

    // 创建任务3
    xTaskCreate((TaskFunction_t)Task3,
                (char *)"Task3",
                (configSTACK_DEPTH_TYPE)START_TASK3_STACK_SIZE,
                (void *)NULL,
                (UBaseType_t)START_TASK3_PRIORITY,
                (TaskHandle_t *)&Start_Task3_Handle);

        // 创建游戏逻辑任务
    xTaskCreate((TaskFunction_t)Game_Logic_Task,
                (char *)"Game_Logic_Task",
                (configSTACK_DEPTH_TYPE)GAME_LOGIC_TASK_STACK_SIZE,
                (void *)NULL,
                (UBaseType_t)GAME_LOGIC_TASK_PRIORITY,
                (TaskHandle_t *)&Game_Logic_Task_Handle);
    
        // 创建输入处理任务
    xTaskCreate((TaskFunction_t)Input_Task,
                (char *)"Input_Task",
                (configSTACK_DEPTH_TYPE)INPUT_TASK_STACK_SIZE,
                (void *)NULL,
                (UBaseType_t)INPUT_TASK_PRIORITY,
                (TaskHandle_t *)&Input_Task_Handle);

    // 删除启动任务(只要执行一次)
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();
}

void LvHandler_Task(void *pvParameters)
{
	vTaskDelay(pdMS_TO_TICKS(100));
    
		create_home_screen();
    create_game_win_screen();
    create_game_play_screen();
    create_select_screen();
	
    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
    static UI_STATE_t last_state = UI_STATE_START;
    lv_disp_load_scr(Home_Screen);
    Current_State = UI_STATE_START;
    //lv_disp_load_scr(game_play_screen);
    xSemaphoreGive(lvgl_mutex);
		while (1)
    {
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
        lv_task_handler();
        xSemaphoreGive(lvgl_mutex);
        vTaskDelay(pdMS_TO_TICKS(5));

    if (Current_State != last_state)
    {
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY); // 屏幕加载前获取互斥锁

        switch (Current_State)
        {
            case UI_STATE_START:
                create_home_screen(); // ȷ�� home_screen �����Ѵ���
                lv_disp_load_scr(Home_Screen); // ������ҳ��
                break;
            case UI_STATE_SELECT:
                create_select_screen(); // ȷ�� select_screen �����Ѵ����������Ѹ���
                lv_disp_load_scr(Select_Screen); // ����ѡ�����
                break;
            case UI_STATE_IN_GAMME:
               my_printf(&huart1, "LvHandler_Task: Attempting to load game_play_screen...\r\n"); // 这个打印应该会出现  
							create_game_play_screen(); // ȷ�� game_play_screen �����Ѵ���
                lv_disp_load_scr(game_play_screen); // ������Ϸ����
						my_printf(&huart1, "LvHandler_Task: game_play_screen loaded. Releasing mutex...\r\n"); // 如果卡住，这个不会打印
                break;
            
        }
    }
		xSemaphoreGive(lvgl_mutex); // 屏幕加载后释放互斥锁
//     if (Current_State == UI_STATE_IN_GAMME)
//        {
//            xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
//            // ���¶�̬Ԫ�أ����λ�á������ȣ�����Щ����ֱ�Ӳ�����Ϸ��Ļ�ϵ�LVGL����
//            game_screen_update_dynamic_elements(&current_player1_state, &current_player2_state);
//            // ����UI���ǲ㣨�������������ֵ��ʾ��
//            game_screen_update_ui_overlay(current_game_score,
//                                          current_player1_state.health,
//                                          current_player2_state.health,
//                                          remaining_game_time_sec);
//            xSemaphoreGive(lvgl_mutex);
//        }
    
    
    last_state = Current_State;   
		}

}

void Task2(void *pvParameters)
{
    my_printf(&huart1, "OK111\r\n");
    while (1)
    {

		
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void Task3(void *pvParameters)
{
    uint8_t queue1_result = 0;
    BaseType_t result = pdFALSE;
    while (1)
    {
        my_printf(&huart1, "task3OK\r\n");
        adc_task();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void Game_Logic_Task(void *pvParameters)
{
  bool game_initialized_for_current_level = false;  
//	if (Game_LoadLevel(1)) 
//    {
//        // 加载成功后绘制地�?
//        game_screen_draw_map(current_level_data);
//        
//        // 更新玩家显示
//        game_screen_update_dynamic_elements(&current_player1_state, &current_player2_state);
//    }
//    while (1)
//    {
//        Game_Update();
//        // 更新UI
//        game_screen_update_dynamic_elements(&current_player1_state, &current_player2_state);
//        game_screen_update_ui_overlay(current_game_score, 
//                                      current_player1_state.health, 
//                                      current_player2_state.health, 
//                                      remaining_game_time_sec);
//        
//        vTaskDelay(pdMS_TO_TICKS(100)); // 100ms更新一�?
//    }
 while (1)
    {
        // 只有当 UI 状态为 UI_STATE_IN_GAMME 时，才执行游戏逻辑和 UI 更新
        if (Current_State == UI_STATE_IN_GAMME)
        {
            // *******************************************************************
            // 1. 游戏初始化（地图绘制、玩家初始位置等）
            //    仅在首次进入 UI_STATE_IN_GAMME 时执行
            // *******************************************************************
            if (!game_initialized_for_current_level)
            {
//                my_printf(&huart1, "Game_Logic_Task: Entering game state, attempting to load level and draw map.\r\n");
//                if (Game_LoadLevel(1)) // 加载游戏关卡数据（例如关卡1）
//                {
//                    // 绘制地图和更新玩家显示，需要互斥锁保护 LVGL 操作
//                    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
//                    game_screen_draw_map(current_level_data); // 绘制地图
//                    // 更新玩家显示，确保两个玩家都出现
//                    game_screen_update_dynamic_elements(&current_player1_state, &current_player2_state);
//                    xSemaphoreGive(lvgl_mutex);

//                    game_initialized_for_current_level = true; // 标记为已初始化
//                    my_printf(&huart1, "Game_Logic_Task: Game screen initialized successfully.\r\n");
//                }
//                else
//                {
//                    my_printf(&huart1, "Game_Logic_Task: Game_LoadLevel failed! Cannot initialize game screen.\r\n");
//                    // 错误处理：例如可以切换回主屏幕或显示错误信息
//                    // xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
//                    // Current_State = UI_STATE_START; // 返回开始界面
//                    // xSemaphoreGive(lvgl_mutex);
//                }
							if (Game_LoadLevel(1))
							{
            my_printf(&huart1, "Game_Logic_Task: Level loaded successfully. Taking mutex...\r\n");
            //xSemaphoreTake(lvgl_mutex, portMAX_DELAY); // <-- 如果卡在这里，说明有其他任务持有互斥量且不释放
            my_printf(&huart1, "Game_Logic_Task: Mutex taken. Calling draw_map...\r\n");
            game_screen_draw_map(current_level_data); // <-- 如果卡在这里，说明 draw_map 内部有问题（如无限循环或内部尝试再次获取互斥量）
            my_printf(&huart1, "Game_Logic_Task: draw_map finished. Calling update_dynamic_elements...\r\n");
            game_screen_update_dynamic_elements(&current_player1_state, &current_player2_state); // <-- 如果卡在这里，说明 update_dynamic_elements 内部有问题
            my_printf(&huart1, "Game_Logic_Task: update_dynamic_elements finished. Releasing mutex...\r\n");
            //xSemaphoreGive(lvgl_mutex);
            my_printf(&huart1, "Game_Logic_Task: Mutex released. Initialisation complete.\r\n");
            
            game_initialized_for_current_level = true;
            my_printf(&huart1, "Game_Logic_Task: Game screen initialized successfully.\r\n"); 
							}
        else
        {
            my_printf(&huart1, "Game_Logic_Task: Game_LoadLevel failed! Cannot initialize game screen.\r\n");
        }
            }

            // *******************************************************************
            // 2. 持续执行游戏逻辑更新 (例如物理模拟、AI、碰撞检测等)
            // *******************************************************************
            Game_Update();
            
            // *******************************************************************
            // 3. 持续更新游戏内的动态 UI 元素
            //    这些更新操作也需要互斥锁保护。
            // *******************************************************************
            xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
            game_screen_update_dynamic_elements(&current_player1_state, &current_player2_state); // 更新玩家位置/动画
            game_screen_update_ui_overlay(current_game_score, // 更新分数、血量、时间等叠加信息
                                          current_player1_state.health, 
                                          current_player2_state.health, 
                                          remaining_game_time_sec);
            xSemaphoreGive(lvgl_mutex);
        }
        else // 如果当前 UI 状态不是游戏状态，则重置初始化标志
        {
            game_initialized_for_current_level = false;
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // 游戏逻辑和 UI 更新频率，可根据游戏流畅度调整
    }
    

}

void Input_Task(void *pvParameters)
{
    while (1)
    {
        my_printf(&huart1, "task4OK\r\n");
        key_proc();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
void vApplicationTickHook(void)
{
    lv_tick_inc(1);
}

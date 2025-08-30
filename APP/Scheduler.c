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
#define START_LvHandler_Task_PRIORITY 4 // 4
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

// 游戏逻辑任务
#define GAME_LOGIC_TASK_STACK_SIZE 1024
#define GAME_LOGIC_TASK_PRIORITY 2 // 2
TaskHandle_t Game_Logic_Task_Handle;
void Game_Logic_Task(void *pvParameters);

// 输入处理任务
#define INPUT_TASK_STACK_SIZE 256
#define INPUT_TASK_PRIORITY 3
TaskHandle_t Input_Task_Handle;
void Input_Task(void *pvParameters);

// 添加时间显示任务配置
#define TIME_DISPLAY_TASK_STACK_SIZE 256
#define TIME_DISPLAY_TASK_PRIORITY 2
TaskHandle_t Time_Display_Task_Handle;
void Time_Display_Task(void *pvParameters);

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

extern GameState_t current_game_state;     // 现在的游戏状�???
extern const Level_t *current_level_data;  // 关卡�???
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
    // 启动调度
    vTaskStartScheduler();
}

void Start_Task(void *pvParameters)
{
    // 创建lvgl任务
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

        // 创建时间显示任务
    xTaskCreate((TaskFunction_t)Time_Display_Task,
                (char *)"Time_Display_Task",
                (configSTACK_DEPTH_TYPE)TIME_DISPLAY_TASK_STACK_SIZE,
                (void *)NULL,
                (UBaseType_t)TIME_DISPLAY_TASK_PRIORITY,
                (TaskHandle_t *)&Time_Display_Task_Handle);

    // 删除启动任务(只要执行一�???)
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
    // lv_disp_load_scr(game_play_screen);
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
                create_home_screen();
                lv_disp_load_scr(Home_Screen);
                break;
            case UI_STATE_SELECT:
                create_select_screen();
                lv_disp_load_scr(Select_Screen);
                break;
            case UI_STATE_IN_GAMME:

                create_game_play_screen();
                lv_disp_load_scr(game_play_screen);

                break;
            case UI_STATE_WON:
                create_game_win_screen();
                lv_disp_load_scr(game_win_screen);

                break;
            case UI_STATE_LOSE:
                create_game_lose_screen();
                lv_disp_load_scr(game_lose_screen);
                break;
            }
            xSemaphoreGive(lvgl_mutex);
        }
        // xSemaphoreGive(lvgl_mutex); // 屏幕加载后释放互斥锁
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

        adc_task();
        Buzzer_APP();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
void Game_Logic_Task(void *pvParameters)
{
    bool game_initialized_for_current_level = false;
    while (1)
    {
        // 只有�??? UI 状态为 UI_STATE_IN_GAMME 时，才执行游戏逻辑�??? UI 更新
        if (Current_State == UI_STATE_IN_GAMME)
        {

            if (!game_initialized_for_current_level)
            {
                if (Game_LoadLevel(Select_Number))
                {

                    // xSemaphoreTake(lvgl_mutex, portMAX_DELAY); // <-- 如果卡在这里，说明有其他任务持有互斥量且不释�???

                    game_screen_draw_map(current_level_data); // <-- 如果卡在这里，说�??? draw_map 内部有问题（如无限循环或内部尝试再次获取互斥量）

                    game_screen_update_dynamic_elements(&current_player1_state, &current_player2_state); // <-- 如果卡在这里，说�??? update_dynamic_elements 内部有问�???

                    // xSemaphoreGive(lvgl_mutex);

                    game_initialized_for_current_level = true;
                }
                else
                {
                    my_printf(&huart1, "Game_Logic_Task: Game_LoadLevel failed! Cannot initialize game screen.\r\n");
                }
            }

            Game_Update();

            // xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
            if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
            {

                game_screen_update_dynamic_elements(&current_player1_state, &current_player2_state); // 更新玩家位置/动画
                game_screen_update_ui_overlay(current_game_score,                                    // 更新分数、血量、时间等叠加信息
                                              current_player1_state.health,
                                              current_player2_state.health,
                                              remaining_game_time_sec);
                xSemaphoreGive(lvgl_mutex);
            }
            else
            {
                my_printf(&huart1, "Game_Logic_Task: Failed to get lvgl_mutex within timeout\r\n");
            }
        }
        else // 如果当前 UI 状态不是游戏状态，则重置初始化标志
        {
            game_initialized_for_current_level = false;
        }

        vTaskDelay(pdMS_TO_TICKS(200)); // 游戏逻辑�??? UI 更新频率，可根据游戏流畅度调�???
    }
}

void Input_Task(void *pvParameters)
{
    MPU6050_Dual_Init();
    while (1)
    {
        my_printf(&huart1, "task4OK\r\n");
        key_proc();
        MPU6050_Process_Input();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// 在Scheduler.c文件末尾添加时间显示任务
void Time_Display_Task(void *pvParameters)
{
    static uint32_t last_update_time = 0;
    static lv_obj_t *time_label = NULL;
    
    while (1)
    {
        uint32_t current_time = HAL_GetTick();
        
        // 每秒更新一次时间显示
        if (current_time - last_update_time >= 1000)
        {
            // 只在主界面显示时间
            if (Current_State == UI_STATE_START && Home_Screen != NULL)
            {
                // 获取LVGL互斥锁
                if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
                {
                    // 如果时间标签尚未创建，则创建它
                    if (time_label == NULL)
                    {
                        time_label = lv_label_create(Home_Screen);
                        lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, -10, 10);
                        lv_obj_set_style_text_color(time_label, lv_color_hex(0x0000FF), 0);
                    }
                    
                    // 读取并显示时间
                    timeNow_t currentTime = DS1302_Read_Time();
                    char time_text[32];
                    snprintf(time_text, sizeof(time_text), 
                             "20%02d-%02d-%02d %02d:%02d:%02d",
                             currentTime.year, currentTime.month, currentTime.day,
                             currentTime.hour, currentTime.minute, currentTime.second);
                    lv_label_set_text(time_label, time_text);
                    
                    xSemaphoreGive(lvgl_mutex);
                }
            }
            
            last_update_time = current_time;
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // 100ms检查一次
    }
}
void vApplicationTickHook(void)
{
    lv_tick_inc(1);
}

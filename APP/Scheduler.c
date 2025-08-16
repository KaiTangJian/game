#include "FreeRTOS.h"
#include "task.h"
#include "mydefine.h"
#include "queue.h"
#include "semphr.h"
#include "Bright_APP.h"
#include "adc.h"
#include "Lcd.h"
#include "UI_Manager.h"
// å¯åŠ¨ä»»åŠ¡é…ç½®
#define START_TASK_STACK_SIZE 130
#define START_TASK_PRIORITY 4
TaskHandle_t Start_Task_Handle;
void Start_Task(void *pvParameters);

// ä»»åŠ¡LvHandler_Taské…ç½®
#define START_LvHandler_Task_STACK_SIZE 1024
#define START_LvHandler_Task_PRIORITY 4
TaskHandle_t Start_LvHandler_Task_Handle;
void LvHandler_Task(void *pvParameters);
//// ä»»åŠ¡2é…ç½®
#define START_TASK2_STACK_SIZE 512
#define START_TASK2_PRIORITY 3
TaskHandle_t Start_Task2_Handle;
void Task2(void *pvParameters);

// ä»»åŠ¡3é…ç½®
#define START_TASK3_STACK_SIZE 256
#define START_TASK3_PRIORITY 1
TaskHandle_t Start_Task3_Handle;
void Task3(void *pvParameters);

//æ¸¸æˆé€»è¾‘ä»»åŠ¡
#define GAME_LOGIC_TASK_STACK_SIZE 512
#define GAME_LOGIC_TASK_PRIORITY 2
TaskHandle_t Game_Logic_Task_Handle;
void Game_Logic_Task(void *pvParameters);

//è¾“å…¥å¤„ç†ä»»åŠ¡
#define INPUT_TASK_STACK_SIZE 256
#define INPUT_TASK_PRIORITY 3
TaskHandle_t Input_Task_Handle;
void Input_Task(void *pvParameters);

// å¯åŠ¨
QueueHandle_t queue1;
QueueHandle_t queue2;
QueueHandle_t lvgl_mutex;
uint8_t tx_buf[8];
uint8_t rx_buf[8];
uint8_t Mode = 0;
uint8_t Tx_cnt;
uint8_t playerId;
float accX;
float accY;
float accZ;
float gyroX;
float gyroY;
float gyroZ;

extern GameState_t current_game_state;     // çŽ°åœ¨çš„æ¸¸æˆçŠ¶æ€?
extern const Level_t *current_level_data;  // å…³å¡æ•?
extern GamePlayer_t current_player1_state; // å†°äººçŠ¶æ€?
extern GamePlayer_t current_player2_state; // ç«äººçŠ¶æ€?
extern uint32_t current_game_score;        // æ¸¸æˆåˆ†æ•°
extern uint32_t remaining_game_time_sec;   // å‰©ä½™æ¸¸æˆæ—¶é—´
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
//     // éŽºÑƒåŸ—å®¸ï¹€å½¸ç»‰è¯²å§©
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
        // åˆ›å»ºå¤±è´¥ï¼Œæ‰“å°æ—¥å¿—æˆ–å¤„ç†é”™è¯¯
        my_printf(&huart1, "lvgl_mutex åˆ›å»ºå¤±è´¥ï¼\r\n");
    }
    // åˆ›å»ºå¯åŠ¨ä»»åŠ¡
    xTaskCreate((TaskFunction_t)Start_Task,
                (char *)"Start_Task",
                (configSTACK_DEPTH_TYPE)START_TASK_STACK_SIZE,
                (void *)NULL,
                (UBaseType_t)START_TASK_PRIORITY,
                (TaskHandle_t *)&Start_Task_Handle);
    // å¯åŠ¨è°ƒåº¦å™? è‡ªåŠ¨åˆ›å»ºç©ºé—²ä»»åŠ¡
    vTaskStartScheduler();
}

void Start_Task(void *pvParameters)
{
    // åˆ›å»ºä»»åŠ¡1
    taskENTER_CRITICAL();
    xTaskCreate((TaskFunction_t)LvHandler_Task,
                (char *)"LvHandler_Task",
                (configSTACK_DEPTH_TYPE)START_LvHandler_Task_STACK_SIZE,
                (void *)NULL,
                (UBaseType_t)START_LvHandler_Task_PRIORITY,
                (TaskHandle_t *)&Start_LvHandler_Task_Handle);

    // åˆ›å»ºä»»åŠ¡2
    xTaskCreate((TaskFunction_t)Task2,
                (char *)"Task2",
                (configSTACK_DEPTH_TYPE)START_TASK2_STACK_SIZE,
                (void *)NULL,
                (UBaseType_t)START_TASK2_PRIORITY,
                (TaskHandle_t *)&Start_Task2_Handle);

    // åˆ›å»ºä»»åŠ¡3
    xTaskCreate((TaskFunction_t)Task3,
                (char *)"Task3",
                (configSTACK_DEPTH_TYPE)START_TASK3_STACK_SIZE,
                (void *)NULL,
                (UBaseType_t)START_TASK3_PRIORITY,
                (TaskHandle_t *)&Start_Task3_Handle);

        // åˆ›å»ºæ¸¸æˆé€»è¾‘ä»»åŠ¡
    xTaskCreate((TaskFunction_t)Game_Logic_Task,
                (char *)"Game_Logic_Task",
                (configSTACK_DEPTH_TYPE)GAME_LOGIC_TASK_STACK_SIZE,
                (void *)NULL,
                (UBaseType_t)GAME_LOGIC_TASK_PRIORITY,
                (TaskHandle_t *)&Game_Logic_Task_Handle);
    
        // åˆ›å»ºè¾“å…¥å¤„ç†ä»»åŠ¡
    xTaskCreate((TaskFunction_t)Input_Task,
                (char *)"Input_Task",
                (configSTACK_DEPTH_TYPE)INPUT_TASK_STACK_SIZE,
                (void *)NULL,
                (UBaseType_t)INPUT_TASK_PRIORITY,
                (TaskHandle_t *)&Input_Task_Handle);

    // åˆ é™¤å¯åŠ¨ä»»åŠ¡(åªè¦æ‰§è¡Œä¸€æ¬¡)
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
//        static bool game_initialized = false;
//                // æ·»åŠ ä¸´æ—¶æµ‹è¯•ä»£ç æ¥åˆ‡æ¢ç•Œé?
//         static uint8_t test_state = 0;
//        if (++test_state % 100 == 0) {  // æ¯?500msåˆ‡æ¢ä¸€æ¬?
//             if (test_state/100 % 3 == 0)
//                 lv_disp_load_scr(Home_Screen);
//             else if (test_state/100 % 3 == 1)
//                 lv_disp_load_scr(game_win_screen);
// --- ÆÁÄ»Ë¢ÐÂÂß¼­£ºÖ»ÓÐµ±UI×´Ì¬·¢Éú±ä»¯Ê±²ÅÖØÐÂ´´½¨ÆÁÄ» ---
    // Õâ¸ö²¿·Ö±ØÐëÔÚËùÓÐ¿ÉÄÜÐÞ¸Ä Current_State µÄÂß¼­Ö®ºóÖ´ÐÐ
    if (Current_State != last_state)
    {
        

        switch (Current_State)
        {
            case UI_STATE_START:
                create_home_screen(); // È·±£ home_screen ¶ÔÏóÒÑ´´½¨
                lv_disp_load_scr(Home_Screen); // ¼ÓÔØÖ÷Ò³Ãæ
                break;
            case UI_STATE_SELECT:
                create_select_screen(); // È·±£ select_screen ¶ÔÏóÒÑ´´½¨²¢ÄÚÈÝÒÑ¸üÐÂ
                lv_disp_load_scr(Select_Screen); // ¼ÓÔØÑ¡Ôñ½çÃæ
                break;
            case UI_STATE_IN_GAMME:
                create_game_play_screen(); // È·±£ game_play_screen ¶ÔÏóÒÑ´´½¨
                lv_disp_load_scr(game_play_screen); // ¼ÓÔØÓÎÏ·½çÃæ
                break;
            // È·±£´Ë´¦¸²¸ÇËùÓÐUI_STATE_tÃ¶¾ÙÖµ£¬ÒÔ±ÜÃâÎ´Öª×´Ì¬µ¼ÖÂÆÁÄ»¿Õ°×
        }
    }
     if (Current_State == UI_STATE_IN_GAMME)
        {
            xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
            // ¸üÐÂ¶¯Ì¬ÔªËØ£¨Íæ¼ÒÎ»ÖÃ¡¢·ÖÊýµÈ£©¡£ÕâÐ©º¯ÊýÖ±½Ó²Ù×÷ÓÎÏ·ÆÁÄ»ÉÏµÄLVGL¶ÔÏó¡£
            game_screen_update_dynamic_elements(&current_player1_state, &current_player2_state);
            // ¸üÐÂUI¸²¸Ç²ã£¨ÀýÈç·ÖÊý¡¢ÉúÃüÖµÏÔÊ¾£©
            game_screen_update_ui_overlay(current_game_score,
                                          current_player1_state.health,
                                          current_player2_state.health,
                                          remaining_game_time_sec);
            xSemaphoreGive(lvgl_mutex);
        }
    
    
    last_state = Current_State;
    
    // LVGL²Ù×÷Íê³ÉºóÊÍ·Å»¥³âÁ¿
    xSemaphoreGive(lvgl_mutex);
}

// ÎÞÂÛ×´Ì¬ÊÇ·ñ¸Ä±ä£¬Èç¹ûµ±Ç°ÊÇÓÎÏ·×´Ì¬£¬¶¼ÐèÒª¸üÐÂÓÎÏ·ÄÚ¶¯Ì¬ÔªËØ
        if (Current_State == UI_STATE_IN_GAMME)
    {
    // ÔÚ¸üÐÂ¶¯Ì¬ÔªËØÇ°»ñÈ¡»¥³âÁ¿
    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);

    // ¸üÐÂ¶¯Ì¬ÔªËØ£¨Íæ¼ÒÎ»ÖÃ¡¢·ÖÊýµÈ£©¡£ÕâÐ©º¯ÊýÓ¦¸ÃÖ±½Ó²Ù×÷ÓÎÏ·ÆÁÄ»ÉÏµÄLVGL¶ÔÏó¡£
    game_screen_update_dynamic_elements(&current_player1_state, &current_player2_state);
    
    // ¸üÐÂUI¸²¸Ç²ã
    game_screen_update_ui_overlay(current_game_score, 
                                  current_player1_state.health, 
                                  current_player2_state.health, 
                                  remaining_game_time_sec);
    
    // ¸üÐÂÍê³ÉºóÊÍ·Å»¥³âÁ¿
    xSemaphoreGive(lvgl_mutex);
     }
}

void Task2(void *pvParameters)
{
    my_printf(&huart1, "OK111\r\n");
//    MPU6050_Init();
//    if (MPU6050_Init() != HAL_OK)
//    {
//        my_printf(&huart1, "MPU6050 Init Failed!\r\n");
//    }
    while (1)
    {

		
//			MPU6050_Read_All();
//        // éŽµæ’³åµƒæˆæ’³åš­é’é¢è¦†é™ï¿?
//        my_printf(&huart1, "Acc [g]: %.2f, %.2f, %.2f\t", Ax, Ay, Az);
//        my_printf(&huart1, "Gyro [deg/s]: %.2f, %.2f, %.2f\t", Gx, Gy, Gz);
//        my_printf(&huart1, "Temp: %.2f C\r\n", Temperature);
//        //					handlePlayerMovement(playerId, accX, accY, accZ, gyroX, gyroY, gyroZ);//æ©æ„¬å§©é’ã‚†æŸ‡
//        //			        if(NRF24L01_RxPacket(rx_buf)==0X00)  //NRF24L01å¦¯â€³æ½¡éŽºãƒ¦æ•¹éç‰ˆåµéªžè·ºåž½é‚î…Ÿæ§¸éšï¸½å¸´é€èˆµåžšé??
//        //						{
//        //								my_printf(&huart1,"GZ");
//        //						}
//        //						else
//        //						{
//        //							my_printf(&huart1,"GG");
        //						}
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
    if (Game_LoadLevel(1)) 
    {
        // åŠ è½½æˆåŠŸåŽç»˜åˆ¶åœ°å›?
        game_screen_draw_map(current_level_data);
        
        // æ›´æ–°çŽ©å®¶æ˜¾ç¤º
        game_screen_update_dynamic_elements(&current_player1_state, &current_player2_state);
    }
    while (1)
    {
        Game_Update();
        // æ›´æ–°UI
        game_screen_update_dynamic_elements(&current_player1_state, &current_player2_state);
        game_screen_update_ui_overlay(current_game_score, 
                                      current_player1_state.health, 
                                      current_player2_state.health, 
                                      remaining_game_time_sec);
        
        vTaskDelay(pdMS_TO_TICKS(100)); // 100msæ›´æ–°ä¸€æ¬?
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

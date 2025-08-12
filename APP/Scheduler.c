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

// 启动

char *buffer1[128] = {"qwudhuqhdhqwudhqhdu"};
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
    // 启动调度器 自动创建空闲任务
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

    // 删除启动任务(只要执行一次)
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();
}

void LvHandler_Task(void *pvParameters)
{
	vTaskDelay(pdMS_TO_TICKS(100));
    create_home_screen();
    create_game_win_screen();
    xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
    lv_disp_load_scr(Home_Screen);
    Current_State = UI_STATE_START;
    create_game_play_screen();
    //lv_disp_load_scr(game_play_screen);
    xSemaphoreGive(lvgl_mutex);
		while (1)
    {
        xSemaphoreTake(lvgl_mutex, portMAX_DELAY);
        lv_task_handler();
        xSemaphoreGive(lvgl_mutex);
        vTaskDelay(pdMS_TO_TICKS(5));
                // 添加临时测试代码来切换界面
         static uint8_t test_state = 0;
        if (++test_state % 100 == 0) {  // 每500ms切换一次
             if (test_state/100 % 3 == 0)
                 lv_disp_load_scr(Home_Screen);
             else if (test_state/100 % 3 == 1)
                 lv_disp_load_scr(game_win_screen);

         }
    }
}

void Task2(void *pvParameters)
{
    my_printf(&huart1, "OK111\r\n");
    MPU6050_Init();
    if (MPU6050_Init() != HAL_OK)
    {
        my_printf(&huart1, "MPU6050 Init Failed!\r\n");
    }
    while (1)
    {

        MPU6050_Read_All();
        // 鎵撳嵃杈撳嚭鍒颁覆鍙�
        my_printf(&huart1, "Acc [g]: %.2f, %.2f, %.2f\t", Ax, Ay, Az);
        my_printf(&huart1, "Gyro [deg/s]: %.2f, %.2f, %.2f\t", Gx, Gy, Gz);
        my_printf(&huart1, "Temp: %.2f C\r\n", Temperature);
        //					handlePlayerMovement(playerId, accX, accY, accZ, gyroX, gyroY, gyroZ);//杩愬姩鍒ゆ柇
        //			        if(NRF24L01_RxPacket(rx_buf)==0X00)  //NRF24L01妯″潡鎺ユ敹鏁版嵁骞跺垽鏂槸鍚︽帴鏀舵垚鍔?
        //						{
        //								my_printf(&huart1,"GZ");
        //						}
        //						else
        //						{
        //							my_printf(&huart1,"GG");
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

void vApplicationTickHook(void)
{
    lv_tick_inc(1);
}

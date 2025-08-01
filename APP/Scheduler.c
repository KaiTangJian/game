#include "FreeRTOS.h"
#include "task.h"
#include "mydefine.h"
#include "queue.h"
//启动任务配置
#define START_TASK_STACK_SIZE 130
#define START_TASK_PRIORITY 1
TaskHandle_t Start_Task_Handle;
void Start_Task(void * pvParameters);

//任务1配置
#define START_TASK1_STACK_SIZE 1024
#define START_TASK1_PRIORITY 4
TaskHandle_t Start_Task1_Handle;
void Task1(void * pvParameters);
//任务2配置
#define START_TASK2_STACK_SIZE 512
#define START_TASK2_PRIORITY 2
TaskHandle_t Start_Task2_Handle;
void Task2(void * pvParameters);

//任务3配置
#define START_TASK3_STACK_SIZE 256
#define START_TASK3_PRIORITY 1
TaskHandle_t Start_Task3_Handle;
void Task3(void * pvParameters);

//启动

char* buffer1[128] ={"qwudhuqhdhqwudhqhdu"};
QueueHandle_t queue1;
QueueHandle_t queue2;
void FreeRTOS_Start()
{
    queue1 = xQueueCreate(2,sizeof(uint8_t));
    queue2 = xQueueCreate(1,sizeof(char*));
    //创建启动任务
    xTaskCreate((TaskFunction_t)Start_Task, 
         (char *) "Start_Task", 
        (configSTACK_DEPTH_TYPE) START_TASK_STACK_SIZE, 
        (void*)NULL, 
        (UBaseType_t) START_TASK_PRIORITY , 
        (TaskHandle_t*)  &Start_Task_Handle); 
    //启动调度器 自动创建空闲任务
    vTaskStartScheduler();

}

void Start_Task(void * pvParameters)
{
    //创建任务1
    taskENTER_CRITICAL();
    xTaskCreate((TaskFunction_t)Task1, 
         (char *) "Task1", 
        (configSTACK_DEPTH_TYPE) START_TASK1_STACK_SIZE, 
        (void*)NULL, 
        (UBaseType_t) START_TASK1_PRIORITY , 
        (TaskHandle_t*)  &Start_Task1_Handle); 

    //创建任务2
    xTaskCreate((TaskFunction_t)Task2, 
         (char *) "Task2", 
        (configSTACK_DEPTH_TYPE) START_TASK2_STACK_SIZE, 
        (void*)NULL, 
        (UBaseType_t) START_TASK2_PRIORITY , 
        (TaskHandle_t*)  &Start_Task2_Handle); 

    //创建任务3
    xTaskCreate((TaskFunction_t)Task3, 
         (char *) "Task3", 
       (configSTACK_DEPTH_TYPE) START_TASK3_STACK_SIZE, 
        (void*)NULL, 
        (UBaseType_t) START_TASK3_PRIORITY , 
       (TaskHandle_t*)  &Start_Task3_Handle); 

    //删除启动任务(只要执行一次)
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();  

}

void Task1(void * pvParameters)
{
        uint8_t data = 12;
        BaseType_t result1 = pdFALSE;
        BaseType_t result2 = pdFALSE;
    while(1)
    {
        //LCD_Test();
        // 向队列1发送数据
        result1 = xQueueSend(queue1, &data, portMAX_DELAY);

        //char *p = buffer1; // 新增
        result2 = xQueueSend(queue2, buffer1, portMAX_DELAY); // 修改

        if(result1 == pdTRUE)
        {
            my_printf(&huart1, "task1向队列1发送数据成功\r\n");
        }
        else
        {
            my_printf(&huart1, "task1向队列1发送数据失败\r\n");
        }
        if(result2 == pdTRUE)
        {
            my_printf(&huart1, "task1向队列2发送数据成功\r\n");
        }
        else
        {
            my_printf(&huart1, "task1向队列2发送数据失败\r\n");
        }
        HAL_Delay(50);
    }
}

void Task2(void * pvParameters)
{
    char* queue2_result = 0;
    BaseType_t result = pdFALSE;

    while(1)
    {
        //从队列2接收数据
        BaseType_t result = xQueueReceive(queue2, &queue2_result, portMAX_DELAY);
        if(result == pdTRUE)
        {
            my_printf(&huart1, "task2接收到的数据=%s..\r\n", queue2_result);
        }
        else
        {
            my_printf(&huart1, "task2接收数据失败..\r\n");
        }
        HAL_Delay(50);
    
    }
}

void Task3(void * pvParameters)
{
    uint8_t queue1_result = 0;
    BaseType_t result = pdFALSE;
    while(1)
    {
        //从队列1接收数据
        result = xQueueReceive(queue1, &queue1_result, portMAX_DELAY);
        if(result == pdTRUE)
        {
            my_printf(&huart1, "task3接收到的数据=%d..\r\n", queue1_result);
        }
        else
        {
            my_printf(&huart1, "task3接收数据失败..\r\n");
        }
    }
}


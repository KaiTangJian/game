#include "FreeRTOS.h"
#include "task.h"
#include "mydefine.h"
//启动任务配置
#define START_TASK_STACK_SIZE 130
#define START_TASK_PRIORITY 1
TaskHandle_t Start_Task_Handle;
void Start_Task(void * pvParameters);

//任务1配置
#define START_TASK1_STACK_SIZE 256
#define START_TASK1_PRIORITY 2
TaskHandle_t Start_Task1_Handle;
void Task1(void * pvParameters);
//任务2配置
#define START_TASK2_STACK_SIZE 256
#define START_TASK2_PRIORITY 3
TaskHandle_t Start_Task2_Handle;
void Task2(void * pvParameters);

//任务3配置
#define START_TASK3_STACK_SIZE 256
#define START_TASK3_PRIORITY 4
TaskHandle_t Start_Task3_Handle;
void Task3(void * pvParameters);

//启动

void FreeRTOS_Start()
{
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
    while(1)
    {
    my_printf(&huart1,"Task1 is running\r\n");
    vTaskDelay(500); //延时1秒
    }
}

void Task2(void * pvParameters)
{
    while(1)
    {

    my_printf(&huart1,"Task2 is running\r\n");
    vTaskDelay(500); //延时1秒
    }
}

void Task3(void * pvParameters)
{
    while(1)
    {
    my_printf(&huart1,"Task3 is running\r\n");
    vTaskDelay(500);
    }
}


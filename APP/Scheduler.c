#include "FreeRTOS.h"
#include "task.h"
#include "mydefine.h"
#include "queue.h"
//������������
#define START_TASK_STACK_SIZE 130
#define START_TASK_PRIORITY 1
TaskHandle_t Start_Task_Handle;
void Start_Task(void * pvParameters);

//����1����
#define START_TASK1_STACK_SIZE 1024
#define START_TASK1_PRIORITY 4
TaskHandle_t Start_Task1_Handle;
void Task1(void * pvParameters);
//����2����
#define START_TASK2_STACK_SIZE 512
#define START_TASK2_PRIORITY 2
TaskHandle_t Start_Task2_Handle;
void Task2(void * pvParameters);

//����3����
#define START_TASK3_STACK_SIZE 256
#define START_TASK3_PRIORITY 1
TaskHandle_t Start_Task3_Handle;
void Task3(void * pvParameters);

//����

char* buffer1[128] ={"qwudhuqhdhqwudhqhdu"};
QueueHandle_t queue1;
QueueHandle_t queue2;
void FreeRTOS_Start()
{
    queue1 = xQueueCreate(2,sizeof(uint8_t));
    queue2 = xQueueCreate(1,sizeof(char*));
    //������������
    xTaskCreate((TaskFunction_t)Start_Task, 
         (char *) "Start_Task", 
        (configSTACK_DEPTH_TYPE) START_TASK_STACK_SIZE, 
        (void*)NULL, 
        (UBaseType_t) START_TASK_PRIORITY , 
        (TaskHandle_t*)  &Start_Task_Handle); 
    //���������� �Զ�������������
    vTaskStartScheduler();

}

void Start_Task(void * pvParameters)
{
    //��������1
    taskENTER_CRITICAL();
    xTaskCreate((TaskFunction_t)Task1, 
         (char *) "Task1", 
        (configSTACK_DEPTH_TYPE) START_TASK1_STACK_SIZE, 
        (void*)NULL, 
        (UBaseType_t) START_TASK1_PRIORITY , 
        (TaskHandle_t*)  &Start_Task1_Handle); 

    //��������2
    xTaskCreate((TaskFunction_t)Task2, 
         (char *) "Task2", 
        (configSTACK_DEPTH_TYPE) START_TASK2_STACK_SIZE, 
        (void*)NULL, 
        (UBaseType_t) START_TASK2_PRIORITY , 
        (TaskHandle_t*)  &Start_Task2_Handle); 

    //��������3
    xTaskCreate((TaskFunction_t)Task3, 
         (char *) "Task3", 
       (configSTACK_DEPTH_TYPE) START_TASK3_STACK_SIZE, 
        (void*)NULL, 
        (UBaseType_t) START_TASK3_PRIORITY , 
       (TaskHandle_t*)  &Start_Task3_Handle); 

    //ɾ����������(ֻҪִ��һ��)
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
        // �����1��������
        result1 = xQueueSend(queue1, &data, portMAX_DELAY);

        //char *p = buffer1; // ����
        result2 = xQueueSend(queue2, buffer1, portMAX_DELAY); // �޸�

        if(result1 == pdTRUE)
        {
            my_printf(&huart1, "task1�����1�������ݳɹ�\r\n");
        }
        else
        {
            my_printf(&huart1, "task1�����1��������ʧ��\r\n");
        }
        if(result2 == pdTRUE)
        {
            my_printf(&huart1, "task1�����2�������ݳɹ�\r\n");
        }
        else
        {
            my_printf(&huart1, "task1�����2��������ʧ��\r\n");
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
        //�Ӷ���2��������
        BaseType_t result = xQueueReceive(queue2, &queue2_result, portMAX_DELAY);
        if(result == pdTRUE)
        {
            my_printf(&huart1, "task2���յ�������=%s..\r\n", queue2_result);
        }
        else
        {
            my_printf(&huart1, "task2��������ʧ��..\r\n");
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
        //�Ӷ���1��������
        result = xQueueReceive(queue1, &queue1_result, portMAX_DELAY);
        if(result == pdTRUE)
        {
            my_printf(&huart1, "task3���յ�������=%d..\r\n", queue1_result);
        }
        else
        {
            my_printf(&huart1, "task3��������ʧ��..\r\n");
        }
    }
}



#include "UI_Manager.h"
#include "Bright_APP.h"
#include "queue.h"
uint8_t key_val, key_down, key_old, key_up;
extern void password_input_digit(int digit);
extern void password_confirm(void);
extern void password_backspace(void);
extern int Select_Number;
extern void update_level_labels_highlight(void);
extern QueueHandle_t app_msg_queue;
extern QueueHandle_t ui_request_queue;
extern QueueHandle_t ui_state_queue;
// 消息类型定义
typedef enum
{
    MSG_USER_ACTIVITY = 1,
    MSG_SCREEN_OFF,
    MSG_SCREEN_ON,
    MSG_GAME_STATE_CHANGE,
    MSG_VOLUME_CHANGE,
    MSG_WAKEUP,
    MSG_TIME_UPDATE
} AppMsgType_t;

typedef struct
{
    AppMsgType_t type;
    uint32_t data;
} AppMessage_t;

static UI_STATE_t local_ui_state = UI_STATE_PASSWORD;
uint8_t key_read()
{
    uint8_t temp = 0;

    if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_3) == RESET)
        temp = 1;
    if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_4) == RESET)
        temp = 2;
    if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_5) == RESET)
        temp = 3;
    if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6) == RESET)
        temp = 4;
    return temp;
}
void key_proc()
{
    key_val = key_read();
    key_down = key_val & (key_old ^ key_val); // 检测按键按下事
    key_up = ~key_val & (key_old ^ key_val);  // 检测按键释放事
    key_old = key_val;                        // 更新旧按键状
    AppMessage_t state_msg;
    while (xQueueReceive(ui_state_queue, &state_msg, 0) == pdPASS)
    {
        if (state_msg.type == MSG_GAME_STATE_CHANGE)
        {
            local_ui_state = (UI_STATE_t)state_msg.data;
        }
    }

    if (Screen_On && key_down != 0) // 只有当有按键按下时才执行以下逻辑
    {
        AppMessage_t req_msg;
        AppMessage_t msg = {MSG_USER_ACTIVITY, HAL_GetTick()};
        xQueueSend(app_msg_queue, &msg, 0);
        req_msg.type = MSG_GAME_STATE_CHANGE;
        switch (local_ui_state)
        {
        case UI_STATE_START:   // 当前在开始界
            if (key_down == 3) // 在开始界面按下按
            {
                req_msg.data = UI_STATE_SELECT;
                xQueueSend(ui_request_queue, &req_msg, 0);
            }
                else if (key_down == 4) // 在开始界面按下按键4
            {
                req_msg.data = UI_STATE_PASSWORD;
                xQueueSend(ui_request_queue, &req_msg, 0);
            }
            break;

        case UI_STATE_SELECT:  // 当前在选择界面
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
                req_msg.data = UI_STATE_IN_GAMME;
                xQueueSend(ui_request_queue, &req_msg, 0);
            }
            else if (key_down == 4) // 按键4: 从选择界面退出到主页
            {
                req_msg.data = UI_STATE_START;
                xQueueSend(ui_request_queue, &req_msg, 0);
            }
            break;

        case UI_STATE_IN_GAMME: // 当前在游戏界
            if (key_down == 4)  // 按键4: 从游戏界面退出到主页
            {
                req_msg.data = UI_STATE_START;
                xQueueSend(ui_request_queue, &req_msg, 0);
            }

            break;
        case UI_STATE_WON:
            if (key_down == 4) // 按键4: 退出到主页
            {
                req_msg.data = UI_STATE_START;
                xQueueSend(ui_request_queue, &req_msg, 0);
            }
            break;
        case UI_STATE_LOSE:
            if (key_down == 4) // 按键4: 退出到主页
            {
                req_msg.data = UI_STATE_START;
                xQueueSend(ui_request_queue, &req_msg, 0);
            }
            break;

        case UI_STATE_PASSWORD:
            if (key_down == 1) // 按键1输入数字1
            {
                password_input_digit(1);
            }
            else if (key_down == 2) // 按键2输入数字2
            {
                password_input_digit(2);
            }
            else if (key_down == 3) // 按键3确认密码
            {
                password_confirm();
            }
            else if (key_down == 4) // 按键4删除一个输入的字符
            {
                password_backspace();
            }
            break;
        }
    }
}

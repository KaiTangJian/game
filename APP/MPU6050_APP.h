#ifndef __MPU6050_APP_H
#define __MPU6050_APP_H
#include "I2C.h"
#include "stdio.h"
#include "stdbool.h"
#include "stdarg.h"
#include "Game_Manager.h"
#include "USART_APP.h"
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart1;
// 定义两个MPU6050的I2C地址
#define MPU6050_PLAYER1_ADDR (0x68<<1)  // 玩家1传感器地址
#define MPU6050_PLAYER2_ADDR (0x69<<1)  // 玩家2传感器地址（AD0接VCC）
/* MPU6050 I2C璁惧鍦板潃瀹氫箟 */
#define WHO_AM_I_REG 0x75        // WHO_AM_I瀵勫瓨鍣ㄥ湴鍧€锛岄粯璁ゅ€�0x68
#define PWR_MGMT_1_REG 0x6B      // 鐢垫簮绠＄悊瀵勫瓨鍣�1
#define SMPLRT_DIV_REG 0x19      // 閲囨牱鐜囧垎棰戝瘎瀛樺櫒
#define CONFIG_REG 0x1A          // 閰嶇疆瀵勫瓨鍣紙鍚獶LPF璁剧疆锛�
#define GYRO_CONFIG_REG 0x1B     // 闄€铻轰华閰嶇疆瀵勫瓨鍣�
#define ACCEL_CONFIG_REG 0x1C    // 鍔犻€熷害璁￠厤缃瘎瀛樺櫒
#define ACCEL_XOUT_H_REG     0x3B
// 控制阈值定义
#define MOVE_THRESHOLD_GYRO  30.0f     // 左右移动阈值（陀螺仪度/秒）
#define JUMP_THRESHOLD_ACCEL 1.5f      // 跳跃检测阈值（加速度g）
#define JUMP_COOLDOWN        500       // 跳跃冷却时间（毫秒）
#define MOVE_INTERVAL        200       // 连续移动间隔（毫秒）
// 双传感器数据结构
typedef struct {
    // 原始数据
    int16_t Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW;
    int16_t Gyro_X_RAW, Gyro_Y_RAW, Gyro_Z_RAW;
    int16_t Temp_RAW;
    
    // 处理后的数据
    float Ax, Ay, Az;
    float Gx, Gy, Gz;
    float Temperature;
    
    // 控制相关
    uint32_t last_jump_time;
    uint32_t last_move_time;
} MPU6050_PlayerData;

// 函数声明
HAL_StatusTypeDef MPU6050_Dual_Init(void);
HAL_StatusTypeDef MPU6050_Init_Single(uint16_t device_addr);
void MPU6050_Read_Player_Data(uint8_t player_id);
void MPU6050_Process_Input(void);
void MPU6050_Read_All_Single(uint16_t device_addr, MPU6050_PlayerData* data);

// 全局数据变量
extern MPU6050_PlayerData player1_data;
extern MPU6050_PlayerData player2_data;

#endif






//#ifndef __MPU6050_APP_H
//#define __MPU6050_APP_H
//#include "I2C.h"
//extern I2C_HandleTypeDef hi2c2;
///* MPU6050 I2C璁惧鍦板潃瀹氫箟 */
//#define MPU6050_ADDR (0x68 << 1) // 鑻D0鎺ュ湴锛�7浣嶅湴鍧€0x68锛屽乏绉�1浣嶅緱鍒�8浣嶅湴鍧€0xD0
//#define WHO_AM_I_REG 0x75        // WHO_AM_I瀵勫瓨鍣ㄥ湴鍧€锛岄粯璁ゅ€�0x68
//#define PWR_MGMT_1_REG 0x6B      // 鐢垫簮绠＄悊瀵勫瓨鍣�1
//#define SMPLRT_DIV_REG 0x19      // 閲囨牱鐜囧垎棰戝瘎瀛樺櫒
//#define CONFIG_REG 0x1A          // 閰嶇疆瀵勫瓨鍣紙鍚獶LPF璁剧疆锛�
//#define GYRO_CONFIG_REG 0x1B     // 闄€铻轰华閰嶇疆瀵勫瓨鍣�
//#define ACCEL_CONFIG_REG 0x1C    // 鍔犻€熷害璁￠厤缃瘎瀛樺櫒

//HAL_StatusTypeDef MPU6050_Init(void);
//void MPU6050_Read_All(void);

//#endif

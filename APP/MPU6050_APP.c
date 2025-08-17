#include "MPU6050_APP.h"
#include "gpio.h"
#include "nrf24L01.h"
int16_t Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW;
int16_t Gyro_X_RAW, Gyro_Y_RAW, Gyro_Z_RAW;
int16_t Temp_RAW;

float Ax, Ay, Az;
float Gx, Gy, Gz;
float Temperature;
MPU6050_PlayerData player1_data;
MPU6050_PlayerData player2_data;
HAL_StatusTypeDef MPU6050_Init_Single(uint16_t device_addr)
{
    uint8_t check = 0, data;
    
    //  WHO_AM_I
    if (HAL_I2C_Mem_Read(&hi2c2, device_addr, WHO_AM_I_REG, 1, &check, 1, 100) != HAL_OK)
    {
        return HAL_ERROR;
    }
    if(device_addr == MPU6050_PLAYER1_ADDR)
		{
				if (check != 0x68)
			{
        return HAL_ERROR; 
			}
		}
		else
		{
				if (check != 0x70)//新的mpu报0x70
			{
        return HAL_ERROR; 
			}
		}
   
    // 配置电源管理
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c2, device_addr, PWR_MGMT_1_REG, 1, &data, 1, 100);
    HAL_Delay(10);
    
    // 配置采样�?
    data = 0x07;
    HAL_I2C_Mem_Write(&hi2c2, device_addr, SMPLRT_DIV_REG, 1, &data, 1, 100);
    
    // 配置滤波�?
    data = 0x03;
    HAL_I2C_Mem_Write(&hi2c2, device_addr, CONFIG_REG, 1, &data, 1, 100);
    
    // 配置陀螺仪量程
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c2, device_addr, GYRO_CONFIG_REG, 1, &data, 1, 100);
    
    // 配置加速度计量�?
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c2, device_addr, ACCEL_CONFIG_REG, 1, &data, 1, 100);
    
    return HAL_OK;
}

///* 初始化单个 MPU6050 传感器 */
//HAL_StatusTypeDef MPU6050_Init_Single(uint16_t device_addr)
//{
//    uint8_t check = 0, data;
//    
//    // 检查 WHO_AM_I
//    if (HAL_I2C_Mem_Read(&hi2c2, device_addr, WHO_AM_I_REG, 1, &check, 1, 100) != HAL_OK)
//    {
//        my_printf(&huart1, "I2C read failed for device at address 0x%X\r\n", device_addr);
//        return HAL_ERROR;
//    }
//    
//    my_printf(&huart1, "Device at address 0x%X returned WHO_AM_I: 0x%X\r\n", device_addr, check);
//    
//    if (check != 0x68)
//    {
//        my_printf(&huart1, "Device ID mismatch for address 0x%X. Expected: 0x68, Got: 0x%X\r\n", device_addr, check);
//        return HAL_ERROR; 
//    }
//   
//    // 配置电源管理
//    data = 0x00;
//    if (HAL_I2C_Mem_Write(&hi2c2, device_addr, PWR_MGMT_1_REG, 1, &data, 1, 100) != HAL_OK)
//    {
//        my_printf(&huart1, "Failed to configure power management for device 0x%X\r\n", device_addr);
//        return HAL_ERROR;
//    }
//    HAL_Delay(10);
//    
//    // 配置采样率
//    data = 0x07;
//    HAL_I2C_Mem_Write(&hi2c2, device_addr, SMPLRT_DIV_REG, 1, &data, 1, 100);
//    
//    // 配置滤波器
//    data = 0x03;
//    HAL_I2C_Mem_Write(&hi2c2, device_addr, CONFIG_REG, 1, &data, 1, 100);
//    
//    // 配置陀螺仪量程
//    data = 0x00;
//    HAL_I2C_Mem_Write(&hi2c2, device_addr, GYRO_CONFIG_REG, 1, &data, 1, 100);
//    
//    // 配置加速度计量程
//    data = 0x00;
//    HAL_I2C_Mem_Write(&hi2c2, device_addr, ACCEL_CONFIG_REG, 1, &data, 1, 100);
//    
//    my_printf(&huart1, "MPU6050 at address 0x%X initialized successfully\r\n", device_addr);
//    return HAL_OK;
//}
/* 璇诲彇鎵€鏈変紶鎰熷櫒鏁版嵁 */
 //初始化双MPU6050传感�?
HAL_StatusTypeDef MPU6050_Dual_Init(void)
{
    HAL_StatusTypeDef status1, status2;
    
    // 初始化玩1的MPU6050
    status1 = MPU6050_Init_Single(MPU6050_PLAYER1_ADDR);
    HAL_Delay(50);
    
    // 初始化玩2的MPU6050
    status2 = MPU6050_Init_Single(MPU6050_PLAYER2_ADDR);
    HAL_Delay(50);
    
    // 初始化时间戳
    player1_data.last_jump_time = 0;
    player1_data.last_move_time = 0;
    player2_data.last_jump_time = 0;
    player2_data.last_move_time = 0;
    
    // 如果任一传感器初始化失败，返回错�?
    if (status1 != HAL_OK || status2 != HAL_OK)
    {
        return HAL_ERROR;
    }
    
    return HAL_OK;
}
// 读取单个传感器的所有数
void MPU6050_Read_All_Single(uint16_t device_addr, MPU6050_PlayerData* data)
{
    uint8_t buf[14];
    
    // 读取所有传感器数据
    HAL_I2C_Mem_Read(&hi2c2, device_addr, ACCEL_XOUT_H_REG, 1, buf, 14, 100);
    
    // 解析原始数据
    data->Accel_X_RAW = (int16_t)(buf[0] << 8 | buf[1]);
    data->Accel_Y_RAW = (int16_t)(buf[2] << 8 | buf[3]);
    data->Accel_Z_RAW = (int16_t)(buf[4] << 8 | buf[5]);
    
    data->Temp_RAW = (int16_t)(buf[6] << 8 | buf[7]);
    
    data->Gyro_X_RAW = (int16_t)(buf[8] << 8 | buf[9]);
    data->Gyro_Y_RAW = (int16_t)(buf[10] << 8 | buf[11]);
    data->Gyro_Z_RAW = (int16_t)(buf[12] << 8 | buf[13]);
    
    // 转换为实际物理量（根据您的校准参数调整）
    data->Ax = data->Accel_X_RAW / 16384.0f + 0.01f;
    data->Ay = data->Accel_Y_RAW / 16384.0f + 0.01f;
    data->Az = data->Accel_Z_RAW / 16384.0f - 1.04f;
    
    data->Gx = data->Gyro_X_RAW / 131.0f + 1.89f;
    data->Gy = data->Gyro_Y_RAW / 131.0f + 0.56f;
    data->Gz = data->Gyro_Z_RAW / 131.0f - 0.83f;
    
    data->Temperature = data->Temp_RAW / 340.0f + 36.53f;
}

 //读取指定玩家的传感器数据
void MPU6050_Read_Player_Data(uint8_t player_id)
{
    if (player_id == 1)
    {
        MPU6050_Read_All_Single(MPU6050_PLAYER1_ADDR, &player1_data);
    }
    else if (player_id == 2)
    {
        MPU6050_Read_All_Single(MPU6050_PLAYER2_ADDR, &player2_data);
    }
}

 //处理单个玩家的输�?
static void Process_Single_Player_Input(uint8_t player_id, MPU6050_PlayerData* data)
{
    uint32_t current_time = HAL_GetTick();
    int8_t dx = 0;
    bool should_move = false;
    
    // 左右移动检测（基于Y轴陀螺仪�?
    if (data->Gy > MOVE_THRESHOLD_GYRO)
    {
        dx = 1;  // 向右移动
        should_move = true;
    }
    else if (data->Gy < -MOVE_THRESHOLD_GYRO)
    {
        dx = -1; // 向左移动
        should_move = true;
    }
    
    // 检查是否可以进行连续移�?
    if (should_move && (current_time - data->last_move_time) > MOVE_INTERVAL)
    {
        Game_HandleInput(player_id, dx, 0);
        data->last_move_time = current_time;
    }
    
    // 跳跃检测（基于Z轴加速度变化�?
    // 检测向上的加速度突变（表示开始跳跃）
    if (data->Az > JUMP_THRESHOLD_ACCEL && 
        (current_time - data->last_jump_time) > JUMP_COOLDOWN)
    {
        Game_HandleInput(player_id, 0, -1); // 向上跳跃
        data->last_jump_time = current_time;
    }
}

 //处理双传感器输入
void MPU6050_Process_Input(void)
{
    // 读取两个玩家的传感器数据
    MPU6050_Read_Player_Data(1);
    MPU6050_Read_Player_Data(2);
    
    // 处理玩家1的输�?
    Process_Single_Player_Input(1, &player1_data);
    
    // 处理玩家2的输�?
    Process_Single_Player_Input(2, &player2_data);
}

 //兼容原有接口的函数（读取玩家1数据�?
void MPU6050_Read_All(void)
{
    MPU6050_Read_Player_Data(1);
    
    // 更新全局变量以保持兼容�?
    Accel_X_RAW = player1_data.Accel_X_RAW;
    Accel_Y_RAW = player1_data.Accel_Y_RAW;
    Accel_Z_RAW = player1_data.Accel_Z_RAW;
    
    Gyro_X_RAW = player1_data.Gyro_X_RAW;
    Gyro_Y_RAW = player1_data.Gyro_Y_RAW;
    Gyro_Z_RAW = player1_data.Gyro_Z_RAW;
    
    Temp_RAW = player1_data.Temp_RAW;
    
    Ax = player1_data.Ax;
    Ay = player1_data.Ay;
    Az = player1_data.Az;
    
    Gx = player1_data.Gx;
    Gy = player1_data.Gy;
    Gz = player1_data.Gz;
    
    Temperature = player1_data.Temperature;
}



//#include "MPU6050_APP.h"
//#include "gpio.h"
//#include "nrf24L01.h"
//int16_t Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW;
//int16_t Gyro_X_RAW, Gyro_Y_RAW, Gyro_Z_RAW;
//int16_t Temp_RAW;

///* 杞崲鍚庣殑鐗╃悊閲忓叏灞€鍙橀噺 */
//float Ax, Ay, Az;
//float Gx, Gy, Gz;
//float Temperature;

///* 鍒濆鍖� MPU6050 浼犳劅鍣� */
//HAL_StatusTypeDef MPU6050_Init(void)
//{
//    uint8_t check = 0, data;
//    // 妫€鏌� WHO_AM_I
//    if (HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, 100) != HAL_OK)
//    {
//        return HAL_ERROR;
//    }
//    if (check != 0x68)
//    {
//        return HAL_ERROR; // 璁惧ID涓嶅尮閰�
//    }
//    // 鍞ら啋浼犳劅鍣�
//    data = 0x00;
//    HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &data, 1, 100);
//    HAL_Delay(10);
//    // 璁剧疆閲囨牱鐜� 1kHz
//    data = 0x07;
//    HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &data, 1, 100);
//    // 閰嶇疆 DLPF (44Hz)
//    data = 0x03;
//    HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, CONFIG_REG, 1, &data, 1, 100);
//    // 闄€铻轰华 卤250掳/s
//    data = 0x00;
//    HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &data, 1, 100);
//    // 鍔犻€熷害璁� 卤2g
//    data = 0x00;
//    HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &data, 1, 100);
//    return HAL_OK;
//}

///* 璇诲彇鎵€鏈変紶鎰熷櫒鏁版嵁 */
//void MPU6050_Read_All(void)
//{
//    uint8_t buf[14];
//    // 涓€娆℃€ц鍙� 鍔犻€熷害(6)+娓╁害(2)+闄€铻轰华(6) 鍏�14瀛楄妭
//    HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, 0x3B, 1, buf, 14, 100);
//    // 鍔犻€熷害
//    Accel_X_RAW = (int16_t)(buf[0] << 8 | buf[1]);
//    Accel_Y_RAW = (int16_t)(buf[2] << 8 | buf[3]);
//    Accel_Z_RAW = (int16_t)(buf[4] << 8 | buf[5]);
//    // 娓╁害
//    Temp_RAW = (int16_t)(buf[6] << 8 | buf[7]);
//    // 闄€铻轰华
//    Gyro_X_RAW = (int16_t)(buf[8] << 8 | buf[9]);
//    Gyro_Y_RAW = (int16_t)(buf[10] << 8 | buf[11]);
//    Gyro_Z_RAW = (int16_t)(buf[12] << 8 | buf[13]);
//    // 杞崲涓虹墿鐞嗛噺
//    Ax = Accel_X_RAW / 16384.0f + 0.01;
//    Ay = Accel_Y_RAW / 16384.0f + 0.01;
//    Az = Accel_Z_RAW / 16384.0f - 1.04;
//    Gx = Gyro_X_RAW / 131.0f + 1.89;
//    Gy = Gyro_Y_RAW / 131.0f + 0.56;
//    Gz = Gyro_Z_RAW / 131.0f - 0.83;
//    Temperature = Temp_RAW / 340.0f + 36.53f;
//}

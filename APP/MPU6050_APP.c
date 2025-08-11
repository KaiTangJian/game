#include "MPU6050_APP.h"
#include "gpio.h"
#include "nrf24L01.h"
int16_t Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW;
int16_t Gyro_X_RAW, Gyro_Y_RAW, Gyro_Z_RAW;
int16_t Temp_RAW;

/* 杞崲鍚庣殑鐗╃悊閲忓叏灞€鍙橀噺 */
float Ax, Ay, Az;
float Gx, Gy, Gz;
float Temperature;

/* 鍒濆鍖� MPU6050 浼犳劅鍣� */
HAL_StatusTypeDef MPU6050_Init(void)
{
    uint8_t check = 0, data;
    // 妫€鏌� WHO_AM_I
    if (HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, 100) != HAL_OK)
    {
        return HAL_ERROR;
    }
    if (check != 0x68)
    {
        return HAL_ERROR; // 璁惧ID涓嶅尮閰�
    }
    // 鍞ら啋浼犳劅鍣�
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &data, 1, 100);
    HAL_Delay(10);
    // 璁剧疆閲囨牱鐜� 1kHz
    data = 0x07;
    HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &data, 1, 100);
    // 閰嶇疆 DLPF (44Hz)
    data = 0x03;
    HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, CONFIG_REG, 1, &data, 1, 100);
    // 闄€铻轰华 卤250掳/s
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &data, 1, 100);
    // 鍔犻€熷害璁� 卤2g
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &data, 1, 100);
    return HAL_OK;
}

/* 璇诲彇鎵€鏈変紶鎰熷櫒鏁版嵁 */
void MPU6050_Read_All(void)
{
    uint8_t buf[14];
    // 涓€娆℃€ц鍙� 鍔犻€熷害(6)+娓╁害(2)+闄€铻轰华(6) 鍏�14瀛楄妭
    HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, 0x3B, 1, buf, 14, 100);
    // 鍔犻€熷害
    Accel_X_RAW = (int16_t)(buf[0] << 8 | buf[1]);
    Accel_Y_RAW = (int16_t)(buf[2] << 8 | buf[3]);
    Accel_Z_RAW = (int16_t)(buf[4] << 8 | buf[5]);
    // 娓╁害
    Temp_RAW = (int16_t)(buf[6] << 8 | buf[7]);
    // 闄€铻轰华
    Gyro_X_RAW = (int16_t)(buf[8] << 8 | buf[9]);
    Gyro_Y_RAW = (int16_t)(buf[10] << 8 | buf[11]);
    Gyro_Z_RAW = (int16_t)(buf[12] << 8 | buf[13]);
    // 杞崲涓虹墿鐞嗛噺
    Ax = Accel_X_RAW / 16384.0f + 0.01;
    Ay = Accel_Y_RAW / 16384.0f + 0.01;
    Az = Accel_Z_RAW / 16384.0f - 1.04;
    Gx = Gyro_X_RAW / 131.0f + 1.89;
    Gy = Gyro_Y_RAW / 131.0f + 0.56;
    Gz = Gyro_Z_RAW / 131.0f - 0.83;
    Temperature = Temp_RAW / 340.0f + 36.53f;
}

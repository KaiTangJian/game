#ifndef __MPU6050_APP_H
#define __MPU6050_APP_H
#include "I2C.h"
extern I2C_HandleTypeDef hi2c2;
/* MPU6050 I2C璁惧鍦板潃瀹氫箟 */
#define MPU6050_ADDR (0x68 << 1) // 鑻D0鎺ュ湴锛�7浣嶅湴鍧€0x68锛屽乏绉�1浣嶅緱鍒�8浣嶅湴鍧€0xD0
#define WHO_AM_I_REG 0x75        // WHO_AM_I瀵勫瓨鍣ㄥ湴鍧€锛岄粯璁ゅ€�0x68
#define PWR_MGMT_1_REG 0x6B      // 鐢垫簮绠＄悊瀵勫瓨鍣�1
#define SMPLRT_DIV_REG 0x19      // 閲囨牱鐜囧垎棰戝瘎瀛樺櫒
#define CONFIG_REG 0x1A          // 閰嶇疆瀵勫瓨鍣紙鍚獶LPF璁剧疆锛�
#define GYRO_CONFIG_REG 0x1B     // 闄€铻轰华閰嶇疆瀵勫瓨鍣�
#define ACCEL_CONFIG_REG 0x1C    // 鍔犻€熷害璁￠厤缃瘎瀛樺櫒

HAL_StatusTypeDef MPU6050_Init(void);
void MPU6050_Read_All(void);

#endif

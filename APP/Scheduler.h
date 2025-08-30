#ifndef __Scheduler_H__
#define __Scheduler_H__

#include "mydefine.h"
#include "MPU6050_APP.h"
#include "Buzzer_APP.h"
#include "DS1302_APP.h"
extern float Ax, Ay, Az;
extern float Gx, Gy, Gz;
extern float Temperature;
void FreeRTOS_Start(void);

#endif

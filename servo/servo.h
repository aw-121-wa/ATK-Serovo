#ifndef __SERVO_H
#define __SERVO_H

#include "IMU.h"

void Servo1_SetByAccX(void);  // 舵机1: acc_x控制，±90°
void Servo2_SetByAccY(void);  // 舵机2: acc_y控制，±30°

#endif

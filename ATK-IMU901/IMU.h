#ifndef __IMU_H
#define __IMU_H

#include "string.h"
#include "stdio.h"
#include "main.h"

// ATK-IMU901M帧头定义
#define IMU_FRAME_HEAD_L        0x55
#define IMU_FRAME_HEAD_H        0x55

#define IMU_FRAME_ID_ATTITUDE   0x01    // 姿态角
#define IMU_FRAME_ID_QUAT       0x02    // 四元数
#define IMU_FRAME_ID_GYRO_ACCE  0x03    // 陀螺仪+加速度计
#define IMU_FRAME_ID_MAG        0x04    // 磁力计
#define IMU_FRAME_ID_BARO       0x05    // 气压计
#define IMU_FRAME_ID_PORT       0x06    // 端口

#define IMU_FRAME_DAT_MAX_SIZE  28

typedef struct {
    float roll, pitch, yaw;              // 欧拉角 (°)
    float acc_x, acc_y, acc_z;           // 加速度 (g)
    float gyro_x, gyro_y, gyro_z;        // 角速度 (°/s)
    int16_t mag_x, mag_y, mag_z;         // 磁场
    float temperature;                    // 温度 (℃)
} IMU_Data_t;

extern IMU_Data_t IMU_Data;
extern uint8_t imu_rx_byte;

void IMU_Init(UART_HandleTypeDef *huart);
void IMU_UART_RxCallback(void);
void IMU_DisplayOnOLED(void);

#endif

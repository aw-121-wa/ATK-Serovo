#include "servo.h"
#include "tim.h"

// 舵机PWM参数 (50Hz, 周期2000)

#define SERVO_PWM_MIN   100  // -90° (1ms)
#define SERVO_PWM_MID   150  // 0°   (1.5ms)
#define SERVO_PWM_MAX   200  // +90° (2ms)

// 一阶低通滤波系数
#define FILTER_ALPHA    0.4f

static float filtered_acc_x = 0.0f;
static float filtered_acc_y = 0.0f;

// 一阶低通滤波: output = alpha * input + (1 - alpha) * last_output
static float LowPassFilter(float input, float *last_output, float alpha)
{
    *last_output = alpha * input + (1.0f - alpha) * (*last_output);
    return *last_output;
}

void Servo1_SetByAccX(void)
{
    float acc_x = IMU_Data.acc_x;
    float angle;
    float duty;

    if (acc_x > 1.0f) acc_x = 1.0f;
    if (acc_x < -1.0f) acc_x = -1.0f;

    acc_x = LowPassFilter(acc_x, &filtered_acc_x, FILTER_ALPHA);

    angle = acc_x * 90.0f;

    duty = SERVO_PWM_MID + (angle / 90.0f) * (SERVO_PWM_MAX - SERVO_PWM_MID);

    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, (uint32_t)duty);
}

void Servo2_SetByAccY(void)
{
    float acc_y = IMU_Data.acc_y;
    float angle;
    float duty;

    if (acc_y > 1.0f) acc_y = 1.0f;
    if (acc_y < -1.0f) acc_y = -1.0f;

    acc_y = LowPassFilter(acc_y, &filtered_acc_y, FILTER_ALPHA);

    angle = acc_y * 60.0f;

    duty = SERVO_PWM_MID + (angle / 90.0f) * (SERVO_PWM_MAX - SERVO_PWM_MID);

    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, (uint32_t)duty);
}

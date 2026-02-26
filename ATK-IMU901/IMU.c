#include "IMU.h"
#include "oled.h"
#include "usart.h"

IMU_Data_t IMU_Data = {0};
uint8_t imu_rx_byte;

static UART_HandleTypeDef *imu_huart = NULL;

typedef enum {
    WAIT_HEAD_L = 0,
    WAIT_HEAD_H,
    WAIT_ID,
    WAIT_LEN,
    WAIT_DAT,
    WAIT_SUM
} IMU_RxState_t;

static IMU_RxState_t rx_state = WAIT_HEAD_L;
static uint8_t frame_id = 0;
static uint8_t frame_len = 0;
static uint8_t frame_dat[IMU_FRAME_DAT_MAX_SIZE];
static uint8_t dat_index = 0;
static uint8_t check_sum = 0;

// 发送配置命令，设置回传内容
// 寄存器0x08: 回传内容设置
// bit0: 姿态角, bit1: 四元数, bit2: 陀螺仪+加速度计, bit3: 磁力计, bit4: 气压计, bit5: 端口
static void IMU_ConfigReturnSet(uint8_t config)
{
    uint8_t buf[6];
    buf[0] = 0x55;          // 帧头L
    buf[1] = 0xAF;          // 帧头H (应答帧)
    buf[2] = 0x08;          // 寄存器ID: RETURNSET
    buf[3] = 0x01;          // 数据长度
    buf[4] = config;        // 配置值
    buf[5] = buf[0] + buf[1] + buf[2] + buf[3] + buf[4];  // 校验和
    HAL_UART_Transmit(imu_huart, buf, 6, 100);
}

void IMU_Init(UART_HandleTypeDef *huart)
{
    imu_huart = huart;
    memset(&IMU_Data, 0, sizeof(IMU_Data));
    rx_state = WAIT_HEAD_L;

    // 配置回传内容: 姿态角(bit0) + 陀螺仪加速度计(bit2) = 0x05
    HAL_Delay(100);
    IMU_ConfigReturnSet(0x05);
    HAL_Delay(10);

    HAL_UART_Receive_IT(imu_huart, &imu_rx_byte, 1);
}

void IMU_UART_RxCallback(void)
{
    if (imu_huart == NULL) return;

    switch (rx_state)
    {
        case WAIT_HEAD_L:
            if (imu_rx_byte == IMU_FRAME_HEAD_L)
            {
                check_sum = imu_rx_byte;
                rx_state = WAIT_HEAD_H;
            }
            break;

        case WAIT_HEAD_H:
            if (imu_rx_byte == IMU_FRAME_HEAD_H)
            {
                check_sum += imu_rx_byte;
                rx_state = WAIT_ID;
            }
            else
            {
                rx_state = WAIT_HEAD_L;
            }
            break;

        case WAIT_ID:
            frame_id = imu_rx_byte;
            check_sum += imu_rx_byte;
            rx_state = WAIT_LEN;
            break;

        case WAIT_LEN:
            if (imu_rx_byte <= IMU_FRAME_DAT_MAX_SIZE)
            {
                frame_len = imu_rx_byte;
                check_sum += imu_rx_byte;
                dat_index = 0;
                if (frame_len == 0)
                {
                    rx_state = WAIT_SUM;
                }
                else
                {
                    rx_state = WAIT_DAT;
                }
            }
            else
            {
                rx_state = WAIT_HEAD_L;
            }
            break;

        case WAIT_DAT:
            frame_dat[dat_index++] = imu_rx_byte;
            check_sum += imu_rx_byte;
            if (dat_index >= frame_len)
            {
                rx_state = WAIT_SUM;
            }
            break;

        case WAIT_SUM:
            if (imu_rx_byte == check_sum)
            {
                int16_t *raw = (int16_t*)frame_dat;

                switch (frame_id)
                {
                    case IMU_FRAME_ID_ATTITUDE:  // 姿态角
                        IMU_Data.roll  = (float)raw[0] / 32768.0f * 180.0f;
                        IMU_Data.pitch = (float)raw[1] / 32768.0f * 180.0f;
                        IMU_Data.yaw   = (float)raw[2] / 32768.0f * 180.0f;
                        break;

                    case IMU_FRAME_ID_GYRO_ACCE:  // 加速度计+陀螺仪
                        // 字节0-5: 加速度计, 字节6-11: 陀螺仪
                        IMU_Data.acc_x  = (float)raw[0] / 32768.0f * 16.0f;
                        IMU_Data.acc_y  = (float)raw[1] / 32768.0f * 16.0f;
                        IMU_Data.acc_z  = (float)raw[2] / 32768.0f * 16.0f;
                        IMU_Data.gyro_x = (float)raw[3] / 32768.0f * 2000.0f;
                        IMU_Data.gyro_y = (float)raw[4] / 32768.0f * 2000.0f;
                        IMU_Data.gyro_z = (float)raw[5] / 32768.0f * 2000.0f;
                        break;

                    case IMU_FRAME_ID_MAG:  // 磁力计
                        IMU_Data.mag_x = raw[0];
                        IMU_Data.mag_y = raw[1];
                        IMU_Data.mag_z = raw[2];
                        break;
                }
            }
            rx_state = WAIT_HEAD_L;
            break;

        default:
            rx_state = WAIT_HEAD_L;
            break;
    }

    HAL_UART_Receive_IT(imu_huart, &imu_rx_byte, 1);
}

void IMU_DisplayOnOLED(void)
{
    char str[32];

    OLED_ShowString(0, 0, "ATK-MS901M", 16);

    sprintf(str, "R:%7.2f", IMU_Data.roll);
    OLED_ShowString(0, 2, str, 16);

    sprintf(str, "P:%7.2f", IMU_Data.pitch);
    OLED_ShowString(0, 4, str, 16);

    sprintf(str, "Y:%7.2f", IMU_Data.yaw);
    OLED_ShowString(0, 6, str, 16);

    sprintf(str, "aX:%5.2f", IMU_Data.acc_x);
    OLED_ShowString(64, 2, str, 16);

    sprintf(str, "aY:%5.2f", IMU_Data.acc_y);
    OLED_ShowString(64, 4, str, 16);

    sprintf(str, "aZ:%5.2f", IMU_Data.acc_z);
    OLED_ShowString(64, 6, str, 16);
}

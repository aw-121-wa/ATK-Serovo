#include "oled.h"
#include "oledfont.h"
#include "i2c.h"

// 函数声明
static uint32_t oled_pow(uint8_t m, uint8_t n);

// OLED显存缓冲区
static uint8_t OLED_GRAM[128][8];

// 写命令
static void OLED_WriteCmd(uint8_t cmd)
{
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDRESS, OLED_CMD, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0xFF);
}

// 写数据
static void OLED_WriteData(uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDRESS, OLED_DATA, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xFF);
}

// 设置坐标
void OLED_SetPos(uint8_t x, uint8_t y)
{
    OLED_WriteCmd(0xB0 + y);
    OLED_WriteCmd(((x & 0xF0) >> 4) | 0x10);
    OLED_WriteCmd(x & 0x0F);
}

// 初始化OLED
void OLED_Init(void)
{
    HAL_Delay(100);

    OLED_WriteCmd(0xAE); // 关闭显示
    OLED_WriteCmd(0x20); // 设置内存地址模式
    OLED_WriteCmd(0x10); // 页地址模式
    OLED_WriteCmd(0xB0); // 设置页起始地址
    OLED_WriteCmd(0xC8); // 设置COM扫描方向
    OLED_WriteCmd(0x00); // 设置低列地址
    OLED_WriteCmd(0x10); // 设置高列地址
    OLED_WriteCmd(0x40); // 设置起始行地址
    OLED_WriteCmd(0x81); // 设置对比度
    OLED_WriteCmd(0xFF);
    OLED_WriteCmd(0xA1); // 设置段重映射
    OLED_WriteCmd(0xA6); // 正常显示
    OLED_WriteCmd(0xA8); // 设置多路复用比
    OLED_WriteCmd(0x3F);
    OLED_WriteCmd(0xA4); // 全局显示开启
    OLED_WriteCmd(0xD3); // 设置显示偏移
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xD5); // 设置时钟分频
    OLED_WriteCmd(0xF0);
    OLED_WriteCmd(0xD9); // 设置预充电周期
    OLED_WriteCmd(0x22);
    OLED_WriteCmd(0xDA); // 设置COM引脚配置
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xDB); // 设置VCOMH电压倍率
    OLED_WriteCmd(0x20);
    OLED_WriteCmd(0x8D); // 使能充电泵
    OLED_WriteCmd(0x14);
    OLED_WriteCmd(0xAF); // 开启显示

    OLED_Clear();
}

// 清屏
void OLED_Clear(void)
{
    uint8_t i, j;
    for(i = 0; i < 8; i++)
    {
        for(j = 0; j < 128; j++)
        {
            OLED_GRAM[j][i] = 0x00;
        }
    }
    OLED_Display();
}

// 更新显示
void OLED_Display(void)
{
    uint8_t i, j;
    for(i = 0; i < 8; i++)
    {
        OLED_SetPos(0, i);
        for(j = 0; j < 128; j++)
        {
            OLED_WriteData(OLED_GRAM[j][i]);
        }
    }
}

// 画点
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t mode)
{
    uint8_t pos, bx, temp = 0;
    if(x > 127 || y > 63) return;

    pos = y / 8;
    bx = y % 8;
    temp = 1 << bx;

    if(mode) OLED_GRAM[x][pos] |= temp;
    else OLED_GRAM[x][pos] &= ~temp;
}

// 填充区域
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode)
{
    uint8_t x, y;
    for(x = x1; x <= x2; x++)
    {
        for(y = y1; y <= y2; y++)
        {
            OLED_DrawPoint(x, y, mode);
        }
    }
    OLED_Display();
}

// 显示字符
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size)
{
    uint8_t c = 0, i = 0;
    c = chr - ' ';

    if(x > OLED_WIDTH - 1) {
        x = 0;
        y = y + 2;
    }

    if(size == 16)
    {
        OLED_SetPos(x, y);
        for(i = 0; i < 8; i++)
            OLED_WriteData(F8X16[c * 16 + i]);
        OLED_SetPos(x, y + 1);
        for(i = 0; i < 8; i++)
            OLED_WriteData(F8X16[c * 16 + i + 8]);
    }
    else
    {
        OLED_SetPos(x, y);
        for(i = 0; i < 6; i++)
            OLED_WriteData(F6x8[c][i]);
    }
}

// 显示字符串
void OLED_ShowString(uint8_t x, uint8_t y, char *str, uint8_t size)
{
    uint8_t j = 0;
    while(str[j])
    {
        OLED_ShowChar(x, y, str[j], size);
        x += 8;
        if(x > 120) {
            x = 0;
            y += 2;
        }
        j++;
    }
}

// 显示数字
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for(t = 0; t < len; t++)
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                OLED_ShowChar(x + (size / 2) * t, y, ' ', size);
                continue;
            }
            else enshow = 1;
        }
        OLED_ShowChar(x + (size / 2) * t, y, temp + '0', size);
    }
}

// 计算m^n
static uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while(n--) result *= m;
    return result;
}

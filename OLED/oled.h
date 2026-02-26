#ifndef __OLED_H
#define __OLED_H

#include "main.h"

// OLED屏幕参数
#define OLED_WIDTH  128
#define OLED_HEIGHT 64

// I2C地址 (根据硬件可能是0x78或0x7A)
#define OLED_ADDRESS 0x78

// 命令/数据选择
#define OLED_CMD  0x00
#define OLED_DATA 0x40

// 函数声明
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Display(void);
void OLED_SetPos(uint8_t x, uint8_t y);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size);
void OLED_ShowString(uint8_t x, uint8_t y, char *str, uint8_t size);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t mode);
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode);

#endif

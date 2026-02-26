# OLED驱动使用教程

## 硬件连接

SSD1306 OLED屏幕（128x64）通过I2C接口连接：

- VCC -> 3.3V/5V
- GND -> GND
- SCL -> I2C时钟线（如PB6）
- SDA -> I2C数据线（如PB7）

## CubeMX配置

1. 在CubeMX中启用I2C1（或其他I2C外设）
2. 配置I2C速度：标准模式100kHz或快速模式400kHz
3. 生成代码

## 代码集成

### 1. 添加文件到项目

将OLED文件夹中的文件添加到Keil/IAR项目：
- oled.c
- oled.h
- oledfont.c
- oledfont.h

### 2. 修改I2C句柄

在 `oled.c` 中，根据你的I2C配置修改：

```c
// 如果使用I2C2，修改为：
HAL_I2C_Mem_Write(&hi2c2, OLED_ADDRESS, ...);
```

### 3. 修改I2C地址（如需要）

在 `oled.h` 中，根据硬件修改地址：

```c
#define OLED_ADDRESS 0x78  // 或 0x7A
```

## 使用示例

在 `main.c` 中：

```c
#include "oled.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();

    // 初始化OLED
    OLED_Init();

    // 显示字符串
    OLED_ShowString(0, 0, "Hello STM32!", 16);
    OLED_ShowString(0, 2, "OLED Test", 16);

    // 显示数字
    OLED_ShowNum(0, 4, 12345, 5, 16);

    while(1)
    {
        // 你的代码
    }
}
```

## API函数说明

### 初始化和显示控制

- `OLED_Init()` - 初始化OLED屏幕
- `OLED_Clear()` - 清空屏幕
- `OLED_Display()` - 刷新显示（使用GRAM缓冲时）

### 文本显示

- `OLED_ShowChar(x, y, chr, size)` - 显示单个字符
  - x: 列坐标 (0-127)
  - y: 页坐标 (0-7)
  - chr: 字符
  - size: 字体大小 (8或16)

- `OLED_ShowString(x, y, str, size)` - 显示字符串
  - str: 字符串指针

- `OLED_ShowNum(x, y, num, len, size)` - 显示数字
  - num: 要显示的数字
  - len: 数字长度

### 图形绘制

- `OLED_DrawPoint(x, y, mode)` - 画点
  - mode: 1=点亮, 0=熄灭

- `OLED_Fill(x1, y1, x2, y2, mode)` - 填充矩形区域

## 坐标系统

- X轴：0-127（列）
- Y轴：0-7（页，每页8个像素点）
- 对于像素级操作：Y轴0-63

## 常见问题

1. **屏幕无显示**
   - 检查I2C地址是否正确（0x78或0x7A）
   - 检查硬件连接
   - 用示波器/逻辑分析仪检查I2C信号

2. **显示乱码**
   - 确认oledfont.c已正确添加到项目
   - 检查字体数组是否完整

3. **显示不完整**
   - 增加I2C超时时间
   - 降低I2C速度到100kHz

## 注意事项

- 首次使用必须调用 `OLED_Init()`
- 使用GRAM缓冲的函数后需调用 `OLED_Display()` 刷新
- 直接写入函数（ShowChar/ShowString）会立即显示

#ifndef __LCD_H
#define __LCD_H
#include "main.h"
#include "lv_port_disp.h"
#include "lvgl.h"
extern DMA_HandleTypeDef hdma_spi1_tx;
// LCD参数结构体
typedef struct
{
    uint16_t width;   // LCD宽度
    uint16_t height;  // LCD高度
    uint16_t id;      // LCD ID
    uint8_t dir;      // 横屏还是竖屏控制：0，竖屏；1，横屏
    uint16_t wramcmd; // 开始写gram指令
    uint16_t setxcmd; // 设置x坐标指令
    uint16_t setycmd; // 设置y坐标指令
}_lcd_dev;

#define LCD_CS_CLR()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET)
#define LCD_CS_SET()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET)
#define LCD_RS_CLR()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)
#define LCD_RS_SET()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)
#define LCD_RST_CLR()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)
#define LCD_RST_SET()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define LCD_BL_CLR()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET)
#define LCD_BL_SET()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET)

// 画笔颜色
#define WHITE         0xFFFF
#define BLACK         0x0000	  
#define BLUE          0x001F  
#define BRED          0XF81F  
#define GRED          0XFFE0    
#define GBLUE         0X07FF
#define RED           0xF800
#define MAGENTA       0xF81F
#define GREEN         0x07E0
#define CYAN          0x7FFF
#define YELLOW        0xFFE0
#define BROWN         0XBC40
#define BRRED         0XFC07
#define GRAY          0X8430

// 常用画笔颜色
#define DARKBLUE      0X01CF
#define LIGHTBLUE     0X7D7C
#define GRAYBLUE      0X5458
 
#define LIGHTGREEN    0X841F
#define LIGHTGRAY     0XEF5B
#define LGRAY         0XC618

#define LGRAYBLUE     0XA651
#define LBBLUE        0X2B12

// 扫描方向定义
#define L2R_U2D  0 // 从左到右,从上到下
#define L2R_D2U  1 // 从左到右,从下到上
#define R2L_U2D  2 // 从右到左,从上到下
#define R2L_D2U  3 // 从右到左,从下到上
#define U2D_L2R  4 // 从上到下,从左到右
#define U2D_R2L  5 // 从上到下,从右到左
#define D2U_L2R  6 // 从下到上,从左到右
#define D2U_R2L  7 // 从下到上,从右到左

// 屏幕方向定义
#define DFT_SCAN_DIR  L2R_U2D  // 默认的扫描方向


// 函数声明
void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(uint16_t Color);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_DrawPoint(uint16_t x, uint16_t y);
void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
uint16_t LCD_ReadPoint(uint16_t x, uint16_t y);
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode);
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size);
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode);
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p);
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue);
uint16_t LCD_ReadReg(uint16_t LCD_Reg);
void LCD_Display_Dir(uint8_t dir);
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);
void LCD_Test(void);
void LCD_Fill_Array(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, lv_color_t *color_p);


// LCD基础通信函数
void LCD_WR_REG(uint8_t cmd);
void LCD_WR_DATA(uint8_t dat);
void LCD_WR_DATA_16Bit(uint16_t dat);

// 局部变量
extern _lcd_dev lcddev;
extern uint16_t POINT_COLOR;
extern uint16_t BACK_COLOR;

#endif

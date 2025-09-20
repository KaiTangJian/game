#ifndef __LCD_H
#define __LCD_H
#include "main.h"
#include "lv_port_disp.h"
#include "lvgl.h"
extern DMA_HandleTypeDef hdma_spi1_tx;
// LCD�����ṹ��
typedef struct
{
    uint16_t width;   // LCD���
    uint16_t height;  // LCD�߶�
    uint16_t id;      // LCD ID
    uint8_t dir;      // ���������������ƣ�0��������1������
    uint16_t wramcmd; // ��ʼдgramָ��
    uint16_t setxcmd; // ����x����ָ��
    uint16_t setycmd; // ����y����ָ��
}_lcd_dev;

#define LCD_CS_CLR()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET)
#define LCD_CS_SET()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET)
#define LCD_RS_CLR()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)
#define LCD_RS_SET()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)
#define LCD_RST_CLR()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)
#define LCD_RST_SET()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define LCD_BL_CLR()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET)
#define LCD_BL_SET()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET)

// ������ɫ
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

// ���û�����ɫ
#define DARKBLUE      0X01CF
#define LIGHTBLUE     0X7D7C
#define GRAYBLUE      0X5458
 
#define LIGHTGREEN    0X841F
#define LIGHTGRAY     0XEF5B
#define LGRAY         0XC618

#define LGRAYBLUE     0XA651
#define LBBLUE        0X2B12

// ɨ�跽����
#define L2R_U2D  0 // ������,���ϵ���
#define L2R_D2U  1 // ������,���µ���
#define R2L_U2D  2 // ���ҵ���,���ϵ���
#define R2L_D2U  3 // ���ҵ���,���µ���
#define U2D_L2R  4 // ���ϵ���,������
#define U2D_R2L  5 // ���ϵ���,���ҵ���
#define D2U_L2R  6 // ���µ���,������
#define D2U_R2L  7 // ���µ���,���ҵ���

// ��Ļ������
#define DFT_SCAN_DIR  L2R_U2D  // Ĭ�ϵ�ɨ�跽��


// ��������
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


// LCD����ͨ�ź���
void LCD_WR_REG(uint8_t cmd);
void LCD_WR_DATA(uint8_t dat);
void LCD_WR_DATA_16Bit(uint16_t dat);

// �ֲ�����
extern _lcd_dev lcddev;
extern uint16_t POINT_COLOR;
extern uint16_t BACK_COLOR;

#endif

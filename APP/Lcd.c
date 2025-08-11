#include "lcd.h"
#include "font.h"
#include "spi.h"
#include "main.h"

// ����LCD��Ҫ����
_lcd_dev lcddev;
// Ĭ����ɫ
uint16_t POINT_COLOR = 0x0000; // ������ɫ
uint16_t BACK_COLOR  = 0xFFFF; // ������ɫ

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) {
        LCD_CS_SET();
    }
}

/******************************************************************************
  ����˵����LCDд������
  ������ݣ�cmd ����
  ����ֵ��  ��
******************************************************************************/
void LCD_WR_REG(uint8_t cmd)
{
    LCD_CS_CLR();
    LCD_RS_CLR(); // ����ģʽ
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 100); 
    LCD_CS_SET();
}

/******************************************************************************
  ����˵����LCDд������
  ������ݣ�dat ����
  ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA(uint8_t dat)
{
    LCD_RS_SET(); // ����ģʽ
    LCD_CS_CLR();
    HAL_SPI_Transmit(&hspi1, &dat, 1, 100); 
    LCD_CS_SET();
}

/******************************************************************************
  ����˵����LCDд��16λ����
  ������ݣ�dat ����
  ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA_16Bit(uint16_t dat)
{
    uint8_t data[2];
    data[0] = dat >> 8;
    data[1] = dat & 0xFF;

    LCD_CS_CLR();
    LCD_RS_SET();
   // HAL_SPI_Transmit_DMA(&hspi1, data, 2);
    HAL_SPI_Transmit(&hspi1, data, 2, 1000);
    LCD_CS_SET();
}

/******************************************************************************
  ����˵����д�Ĵ���
  ������ݣ�LCD_Reg �Ĵ�����ַ
            LCD_RegValue �Ĵ���ֵ
  ����ֵ��  ��
******************************************************************************/
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

/******************************************************************************
  ����˵����������ʼ�ͽ�����ַ
  ������ݣ�x1,y1 ���
            x2,y2 �յ�
  ����ֵ��  ��
******************************************************************************/
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(x1 >> 8);
    LCD_WR_DATA(x1 & 0xFF);
    LCD_WR_DATA(x2 >> 8);
    LCD_WR_DATA(x2 & 0xFF);

    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(y1 >> 8);
    LCD_WR_DATA(y1 & 0xFF);
    LCD_WR_DATA(y2 >> 8);
    LCD_WR_DATA(y2 & 0xFF);

    LCD_WR_REG(lcddev.wramcmd);
}

/******************************************************************************
  ����˵����LCD��ʼ��
******************************************************************************/
void LCD_Init(void)
{
    LCD_RST_CLR();
    HAL_Delay(300);
    LCD_RST_SET();
    HAL_Delay(300);

    LCD_BL_SET();

    // �� ST7796S Ϊ����ʼ��
    LCD_WR_REG(0x11); HAL_Delay(200);
    LCD_WR_REG(0xF0); LCD_WR_DATA(0xC3);
    LCD_WR_REG(0xF0); LCD_WR_DATA(0x96);
    LCD_WR_REG(0x36); LCD_WR_DATA(0x28);
    LCD_WR_REG(0x3A); LCD_WR_DATA(0x55);
    LCD_WR_REG(0xB4); LCD_WR_DATA(0x01);
    LCD_WR_REG(0xB7); LCD_WR_DATA(0xC6);

    LCD_WR_REG(0xE8);
    LCD_WR_DATA(0x40); LCD_WR_DATA(0x8A);
    LCD_WR_DATA(0x00); LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x29); LCD_WR_DATA(0x19);
    LCD_WR_DATA(0xA5); LCD_WR_DATA(0x33);

    LCD_WR_REG(0xC1); LCD_WR_DATA(0x06);
    LCD_WR_REG(0xC2); LCD_WR_DATA(0xA7);
    LCD_WR_REG(0xC5); LCD_WR_DATA(0x18);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0xF0); LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x0B); LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x04); LCD_WR_DATA(0x15);
    LCD_WR_DATA(0x2F); LCD_WR_DATA(0x54);
    LCD_WR_DATA(0x42); LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x17); LCD_WR_DATA(0x14);
    LCD_WR_DATA(0x18); LCD_WR_DATA(0x1B);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA(0xF0); LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x0B); LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x04); LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x2D); LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x42); LCD_WR_DATA(0x3B);
    LCD_WR_DATA(0x16); LCD_WR_DATA(0x14);
    LCD_WR_DATA(0x17); LCD_WR_DATA(0x1B);

    LCD_WR_REG(0xF0); LCD_WR_DATA(0x3C);
    LCD_WR_REG(0xF0); LCD_WR_DATA(0x69);

    LCD_WR_REG(0x29); HAL_Delay(200);

    lcddev.width   = 320;
    lcddev.height  = 480;
    lcddev.id      = 0x7796;
    lcddev.dir     = 0;
    lcddev.wramcmd = 0x2C;
    lcddev.setxcmd = 0x2A;
    lcddev.setycmd = 0x2B;

    LCD_Clear(WHITE);

}

/******************************************************************************
  ����˵��������/�ر���ʾ
******************************************************************************/
void LCD_DisplayOn(void)  { LCD_WR_REG(0x29); }
void LCD_DisplayOff(void) { LCD_WR_REG(0x28); }

/******************************************************************************
  ����˵�������ù��
******************************************************************************/
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(Xpos >> 8);
    LCD_WR_DATA(Xpos & 0xFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(Ypos >> 8);
    LCD_WR_DATA(Ypos & 0xFF);
}

/******************************************************************************
  ����˵��������
******************************************************************************/
void LCD_Clear(uint16_t Color)
{
 /*  uint32_t total = lcddev.width * lcddev.height;
    LCD_Address_Set(0, 0, lcddev.width - 1, lcddev.height - 1);
    LCD_CS_CLR();
    LCD_RS_SET();
    for (uint32_t i = 0; i < total; i++) {
        LCD_WR_DATA_16Bit(Color);
    }
    LCD_CS_SET();
    */
    static uint16_t linebuf[320]; // �����������Ϊ320
    for (int i = 0; i < 320; i++) linebuf[i] = Color;

    for (int y = 0; y < lcddev.height; y++) {
        LCD_Address_Set(0, y, lcddev.width - 1, y);
        HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)linebuf, lcddev.width * 2);
        while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
    }
        
}

/******************************************************************************
  ����˵�������ٻ���
******************************************************************************/
void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_Address_Set(x, y, x, y);
    LCD_WR_DATA_16Bit(color);
}

/******************************************************************************
  ����˵��������
******************************************************************************/
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    int xerr = 0, yerr = 0;
    int dx = x2 - x1, dy = y2 - y1;
    int incx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
    int incy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0));
    dx = abs(dx); dy = abs(dy);
    int dist = (dx > dy ? dx : dy);
    int x = x1, y = y1;

    for (int t = 0; t <= dist; t++) {
        LCD_Fast_DrawPoint(x, y, POINT_COLOR);
        xerr += dx; yerr += dy;
        if (xerr > dist) { xerr -= dist; x += incx; }
        if (yerr > dist) { yerr -= dist; y += incy; }
    }
}

/******************************************************************************
  ����˵����������
******************************************************************************/
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}

/******************************************************************************
  ����˵������Բ
******************************************************************************/
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r)
{
    int a = 0, b = r, di = 3 - (r << 1);
    while (a <= b) {
        LCD_Fast_DrawPoint(x0 - b, y0 - a, POINT_COLOR);
        LCD_Fast_DrawPoint(x0 + b, y0 - a, POINT_COLOR);
        LCD_Fast_DrawPoint(x0 - a, y0 + b, POINT_COLOR);
        LCD_Fast_DrawPoint(x0 + a, y0 + b, POINT_COLOR);
        LCD_Fast_DrawPoint(x0 - b, y0 + a, POINT_COLOR);
        LCD_Fast_DrawPoint(x0 + b, y0 + a, POINT_COLOR);
        LCD_Fast_DrawPoint(x0 - a, y0 - b, POINT_COLOR);
        LCD_Fast_DrawPoint(x0 + a, y0 - b, POINT_COLOR);

        a++;
        if (di < 0) {
            di += 4 * a + 6;
        } else {
            di += 10 + 4 * (a - b);
            b--;
        }
        LCD_Fast_DrawPoint(x0 + b, y0 + a, POINT_COLOR);
    }
}

/******************************************************************************
  ����˵�����������
******************************************************************************/
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint16_t w = ex - sx + 1;
    uint16_t h = ey - sy + 1;
    LCD_Address_Set(sx, sy, ex, ey);
    for (uint16_t i = 0; i < h; i++) {
        for (uint16_t j = 0; j < w; j++) {
            LCD_WR_DATA_16Bit(color);
        }
    }
}

/******************************************************************************
  ����˵������ʾ�ַ�
******************************************************************************/
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t c, uint8_t size, uint8_t mode)
{
    uint8_t temp, t, t1;
    uint16_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    c = c - ' ';
    for (t = 0; t < csize; t++) {
        if (size == 12) temp = asc2_1206[c][t];
        else if (size == 16) temp = asc2_1608[c][t];
        else return;
        for (t1 = 0; t1 < 8; t1++) {
            if (temp & 0x80)
                LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if (mode == 0)
                LCD_Fast_DrawPoint(x, y, BACK_COLOR);
            temp <<= 1;
            y++;
            if ((y - y0) == size) {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/******************************************************************************
  ����˵������ʾ����
******************************************************************************/
uint32_t mypow(uint8_t m, uint8_t n)
{
    uint32_t r = 1;
    while (n--) r *= m;
    return r;
}
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp, en = 0;
    for (t = 0; t < len; t++) {
        temp = (num / mypow(10, len - t - 1)) % 10;
        if (en == 0 && t < (len - 1)) {
            if (temp == 0) {
                LCD_ShowChar(x + (size/2)*t, y, ' ', size, 0);
                continue;
            } else en = 1;
        }
        LCD_ShowChar(x + (size/2)*t, y, temp + '0', size, 0);
    }
}

/******************************************************************************
  ����˵����������������ʾ
******************************************************************************/
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode)
{
    uint8_t t, temp, en = 0;
    for (t = 0; t < len; t++) {
        temp = (num / mypow(10, len - t - 1)) % 10;
        if (en == 0 && t < (len - 1)) {
            if (temp == 0) {
                if (mode & 0x80)
                    LCD_ShowChar(x + (size/2)*t, y, '0', size, mode & 0x01);
                else
                    LCD_ShowChar(x + (size/2)*t, y, ' ', size, mode & 0x01);
                continue;
            } else en = 1;
        }
        LCD_ShowChar(x + (size/2)*t, y, temp + '0', size, mode & 0x01);
    }
}

/******************************************************************************
  ����˵������ʾ�ַ���
******************************************************************************/
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t size, uint8_t *p)
{
    uint16_t x0 = x;
    w += x; h += y;
    while ((*p >= ' ') && (*p <= '~')) {
        if (x >= w) { x = x0; y += size; }
        if (y >= h) break;
        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}

/******************************************************************************
  ����˵�������ô���
******************************************************************************/
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t ex = sx + width - 1;
    uint16_t ey = sy + height - 1;
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(sx >> 8); LCD_WR_DATA(sx & 0xFF);
    LCD_WR_DATA(ex >> 8); LCD_WR_DATA(ex & 0xFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(sy >> 8); LCD_WR_DATA(sy & 0xFF);
    LCD_WR_DATA(ey >> 8); LCD_WR_DATA(ey & 0xFF);
}

/******************************************************************************
  ����˵����������ʾ����
******************************************************************************/
void LCD_Display_Dir(uint8_t dir)
{
    if (dir == 0) {
        lcddev.dir = 0;
        lcddev.width = 320; lcddev.height = 480;
        LCD_WR_REG(0x36); LCD_WR_DATA(0x48);
    } else {
        lcddev.dir = 1;
        lcddev.width = 480; lcddev.height = 320;
        LCD_WR_REG(0x36); LCD_WR_DATA(0x28);
    }
}

/******************************************************************************
  ����˵����LCD����
******************************************************************************/
void LCD_Test(void)
{
    LCD_Clear(WHITE);
    LCD_DrawLine(0, 0, lcddev.width-1, lcddev.height-1);
    LCD_DrawLine(lcddev.width-1, 0, 0, lcddev.height-1);
    LCD_DrawRectangle(20, 120, 200, 160);
    LCD_Draw_Circle(120, 230, 40);

    HAL_Delay(1000); 
    LCD_Clear(RED);
    HAL_Delay(1000); 
    LCD_Clear(GREEN);
    HAL_Delay(1000); 
    LCD_Clear(BLUE);
    HAL_Delay(1000); LCD_Clear(BLACK);
}

void LCD_Fill_Array(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, lv_color_t *color_p)
{
    uint16_t w = ex - sx + 1;
    uint16_t h = ey - sy + 1;
    uint32_t size = w * h;
    
    LCD_Address_Set(sx, sy, ex, ey);
    LCD_CS_CLR();
    LCD_RS_SET();
    
    HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)color_p, size * 2);
    
    // �ȴ��������
    while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) 
    {
        // ����������ִ����������
    }
    
    LCD_CS_SET();
}

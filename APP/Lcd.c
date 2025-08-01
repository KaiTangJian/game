#include "lcd.h"
#include "font.h"
#include "spi.h"
#include "main.h"

// ����LCD��Ҫ����
_lcd_dev lcddev;
// Ĭ����ɫ
uint16_t POINT_COLOR = 0x0000; // ������ɫ
uint16_t BACK_COLOR = 0xFFFF;  // ������ɫ

// LCD��SPIʵ����������CubeMX������һ��
extern SPI_HandleTypeDef hspi1;  // ʹ��SPI1������ʵ�����ÿ�����Ҫ�޸�Ϊhspi2��

/******************************************************************************
      ����˵����LCDд������
      ������ݣ�cmd ����
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_REG(uint8_t cmd)
{
    LCD_CS_CLR();
    LCD_RS_CLR(); // ����ģʽ
    HAL_Delay(1);  // ��Ӷ���ʱ
    // ʹ��HAL�ⷢ������
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    
    LCD_CS_SET();
     HAL_Delay(1);  // ���CS�ߵ�ƽ��ʱ
}

/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat ����
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA(uint8_t dat)
{
    LCD_CS_CLR();
    LCD_RS_SET(); // ����ģʽ
    
    // ʹ��HAL�ⷢ������
    HAL_SPI_Transmit(&hspi1, &dat, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
    
    LCD_CS_SET();
    HAL_Delay(1);
}

/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat ����
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA_16Bit(uint16_t dat)
{
    uint8_t data[2];
    data[0] = dat >> 8;
    data[1] = dat;
    
    LCD_CS_CLR();
    LCD_RS_SET();
    
    // ʹ��HAL�ⷢ��16λ����
    HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
    
    LCD_CS_SET();
}

/******************************************************************************
      ����˵����LCDд�Ĵ���
      ������ݣ�LCD_Reg �Ĵ�����ַ
                LCD_RegValue �Ĵ�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

/******************************************************************************
      ����˵����LCD���Ĵ���
      ������ݣ�LCD_Reg �Ĵ�����ַ
      ����ֵ��  ����������
******************************************************************************/
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);
    
    uint8_t data = 0;
    // ʹ��HAL��������ݣ�ע����Ҫ��ȷ����SPIΪȫ˫��ģʽ
    HAL_SPI_Receive(&hspi1, &data, 1, HAL_MAX_DELAY);
    
    return data;
}

/******************************************************************************
      ����˵����������ʼ�ͽ�����ַ
      ������ݣ�x1,x2 �����е���ʼ�ͽ�����ַ
                y1,y2 �����е���ʼ�ͽ�����ַ
      ����ֵ��  ��
******************************************************************************/
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(x1 >> 8);
    LCD_WR_DATA(x1);
    LCD_WR_DATA(x2 >> 8);
    LCD_WR_DATA(x2);
    
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(y1 >> 8);
    LCD_WR_DATA(y1);
    LCD_WR_DATA(y2 >> 8);
    LCD_WR_DATA(y2);
    
    LCD_WR_REG(lcddev.wramcmd);
}

/******************************************************************************
      ����˵����LCD��ʼ������
      ������ݣ���
      ����ֵ��  ��
******************************************************************************/
void LCD_Init(void)
{
    // ��ʼ��LCD
    LCD_RST_CLR();
    HAL_Delay(300);
    LCD_RST_SET();
    HAL_Delay(300);
    
    // �򿪱���
    LCD_BL_SET();
    
    
   // ��ʼ���Ĵ��� (��ST7796SΪ��)
   LCD_WR_REG(0x11);     // �˳�˯��
    HAL_Delay(200);
    
    LCD_WR_REG(0xF0);     // ���
    LCD_WR_DATA(0xC3);
    
    LCD_WR_REG(0xF0);
    LCD_WR_DATA(0x96);
    
    LCD_WR_REG(0x36);     // �ڴ����ݷ��ʿ���
    LCD_WR_DATA(0x48);    // ��������
    
    LCD_WR_REG(0x3A);     // �ӿ����ظ�ʽ
    LCD_WR_DATA(0x55);    // 16bit 565 RGB
    
    LCD_WR_REG(0xB4);     // �з�ת
    LCD_WR_DATA(0x01);
    
    LCD_WR_REG(0xB7);     // Դ�����������
    LCD_WR_DATA(0xC6);
    
    LCD_WR_REG(0xE8);     // ��ʾ�������
    LCD_WR_DATA(0x40);
    LCD_WR_DATA(0x8A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0x19);
    LCD_WR_DATA(0xA5);
    LCD_WR_DATA(0x33);
    
    LCD_WR_REG(0xC1);     // ��Դ����2
    LCD_WR_DATA(0x06);
    
    LCD_WR_REG(0xC2);     // VGH��ѹ
    LCD_WR_DATA(0xA7);
    
    LCD_WR_REG(0xC5);     // VCOM����
    LCD_WR_DATA(0x18);
    
    LCD_WR_REG(0xE0);     // ������Gamma����
    LCD_WR_DATA(0xF0);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x15);
    LCD_WR_DATA(0x2F);
    LCD_WR_DATA(0x54);
    LCD_WR_DATA(0x42);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x14);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x1B);
    
    LCD_WR_REG(0xE1);     // ������Gamma����
    LCD_WR_DATA(0xF0);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x2D);
    LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x42);
    LCD_WR_DATA(0x3B);
    LCD_WR_DATA(0x16);
    LCD_WR_DATA(0x14);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x1B);
    
    LCD_WR_REG(0xF0);     // ���
    LCD_WR_DATA(0x3C);
    
    LCD_WR_REG(0xF0);
    LCD_WR_DATA(0x69);
    
    LCD_WR_REG(0x29);    // ������ʾ 
    HAL_Delay(200);
    
    // ����LCD����
    lcddev.width = 320;   // ST7796S���
    lcddev.height = 480;  // ST7796S�߶�
    lcddev.id = 0x7796;   // LCD ID
    lcddev.dir = 0;       // ����
    lcddev.wramcmd = 0x2C;// д���Դ�����
    lcddev.setxcmd = 0x2A;// ����x����
    lcddev.setycmd = 0x2B;// ����y����

    // ����Ϊ��ɫ
    LCD_Clear(WHITE);
}

/******************************************************************************
      ����˵����LCD������ʾ
      ������ݣ���
      ����ֵ��  ��
******************************************************************************/
void LCD_DisplayOn(void)
{
    LCD_WR_REG(0x29); // ������ʾ
}

/******************************************************************************
      ����˵����LCD�ر���ʾ
      ������ݣ���
      ����ֵ��  ��
******************************************************************************/
void LCD_DisplayOff(void)
{
    LCD_WR_REG(0x28); // �ر���ʾ
}

/******************************************************************************
      ����˵����LCD���ù��λ��
      ������ݣ�x,y ����
      ����ֵ��  ��
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
      ����˵����LCD��������
      ������ݣ�Color Ҫ��������ɫ
      ����ֵ��  ��
******************************************************************************/
void LCD_Clear(uint16_t Color)
{
    uint32_t i;
    uint32_t totalPoints = lcddev.width * lcddev.height;
    
    LCD_Address_Set(0, 0, lcddev.width-1, lcddev.height-1);
    
    LCD_CS_CLR();
    LCD_RS_SET();
    
    for(i=0; i<totalPoints; i++)
    {
        LCD_WR_DATA_16Bit(Color);
    }
    
    LCD_CS_SET();
}

/******************************************************************************
      ����˵����LCD����
      ������ݣ�x,y ��������
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawPoint(uint16_t x, uint16_t y)
{
    LCD_Address_Set(x, y, x, y); // ���ù��λ��
    LCD_WR_DATA_16Bit(POINT_COLOR);
}

/******************************************************************************
      ����˵����LCD���ٻ���
      ������ݣ�x,y ����
                color ��ɫ
      ����ֵ��  ��
******************************************************************************/
void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_Address_Set(x, y, x, y);
    LCD_WR_DATA_16Bit(color);
}

/******************************************************************************
      ����˵����LCD����
      ������ݣ�x,y ����
      ����ֵ��  ��ɫֵ
******************************************************************************/
uint16_t LCD_ReadPoint(uint16_t x, uint16_t y)
{
    uint8_t r=0, g=0, b=0;
    
    if(x >= lcddev.width || y >= lcddev.height)
        return 0; // ������Χ
    
    LCD_SetCursor(x, y);
    LCD_WR_REG(0x2E); // ��GRAMָ��
    
    // �ն�
    r = LCD_ReadReg(0x00);
    
    // ��ȡRGB����
    r = LCD_ReadReg(0x00);
    g = LCD_ReadReg(0x00);
    b = LCD_ReadReg(0x00);
    
    return ((r<<11) | (g<<5) | b);
}

/******************************************************************************
      ����˵����LCD����
      ������ݣ�x1,y1 �������
                x2,y2 �յ�����
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t t; 
    int xerr = 0, yerr = 0, delta_x, delta_y, distance; 
    int incx, incy, row, col; 
    
    delta_x = x2 - x1; // ������������ 
    delta_y = y2 - y1; 
    row = x1; 
    col = y1; 
    
    if(delta_x > 0) incx = 1; // ���õ������� 
    else if(delta_x == 0) incx = 0; // ��ֱ�� 
    else {incx = -1; delta_x = -delta_x;} 
    
    if(delta_y > 0) incy = 1; 
    else if(delta_y == 0) incy = 0; // ˮƽ�� 
    else {incy = -1; delta_y = -delta_y;} 
    
    if(delta_x > delta_y) distance = delta_x; // ѡȡ�������������� 
    else distance = delta_y; 
    
    for(t=0; t<=distance+1; t++) // ������� 
    {  
        LCD_DrawPoint(row, col); // ���� 
        xerr += delta_x; 
        yerr += delta_y; 
        
        if(xerr > distance) 
        { 
            xerr -= distance; 
            row += incx; 
        } 
        if(yerr > distance) 
        { 
            yerr -= distance; 
            col += incy; 
        } 
    }
}

/******************************************************************************
      ����˵����LCD������
      ������ݣ�x1,y1 �������
                x2,y2 �յ�����
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}

/******************************************************************************
      ����˵����LCD��Բ
      ������ݣ�x0,y0 Բ������
                r �뾶
      ����ֵ��  ��
******************************************************************************/
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r)
{
    int a, b;
    int di;
    a = 0; b = r;    
    di = 3 - (r << 1);       
    
    while(a <= b)
    {
        LCD_DrawPoint(x0-b, y0-a);  // 3           
        LCD_DrawPoint(x0+b, y0-a);  // 0           
        LCD_DrawPoint(x0-a, y0+b);  // 1                
        LCD_DrawPoint(x0-b, y0-a);  // 7                
        LCD_DrawPoint(x0-a, y0-b);  // 2                
        LCD_DrawPoint(x0+b, y0+a);  // 4               
        LCD_DrawPoint(x0+a, y0-b);  // 5
        LCD_DrawPoint(x0+a, y0+b);  // 6 
        LCD_DrawPoint(x0-b, y0+a);                       
        a++;
        
        // ʹ��Bresenham�㷨��Բ     
        if(di < 0)
            di += 4*a+6;   
        else
        {
            di += 10+4*(a-b);   
            b--;
        } 
        LCD_DrawPoint(x0+a, y0+b);
    }
}

/******************************************************************************
      ����˵����LCD�������
      ������ݣ�x1,y1 �������
                x2,y2 �յ�����
                color �����ɫ
      ����ֵ��  ��
******************************************************************************/
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint16_t i, j;
    uint16_t width = ex - sx + 1;      // �õ����Ŀ��
    uint16_t height = ey - sy + 1;     // �߶�
    
    LCD_Address_Set(sx, sy, ex, ey);
    
    for(i=0; i<height; i++)
    {
        for(j=0; j<width; j++)
            LCD_WR_DATA_16Bit(color); // д������
    }
}

/******************************************************************************
      ����˵����LCD��ʾһ���ַ�
      ������ݣ�x,y �������
                num Ҫ��ʾ���ַ�
                size �����С
                mode ���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = (size/8+((size%8)?1:0))*(size/2);  // �õ�����һ���ַ���Ӧ������ռ���ֽ���
    
    num = num - ' '; // �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
    
    for(t=0; t<csize; t++)
    {
        if(size == 12) temp = asc2_1206[num][t];        // ����1206����
        else if(size == 16) temp = asc2_1608[num][t];   // ����1608����
//        else if(size == 24) temp = asc2_2412[num][t];   // ����2412����
        else return;                                    // û�е��ֿ�
        
        for(t1=0; t1<8; t1++)
        {
            if(temp & 0x80) LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if(mode == 0) LCD_Fast_DrawPoint(x, y, BACK_COLOR);
            
            temp <<= 1;
            y++;
            if(y >= lcddev.height) return;      // ��������
            if((y - y0) == size)
            {
                y = y0;
                x++;
                if(x >= lcddev.width) return;   // ��������
                break;
            }
        }
    }
}

/******************************************************************************
      ����˵��������m��n�η�
      ������ݣ�m,n
      ����ֵ��  m^n��ֵ
******************************************************************************/
uint32_t mypow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while(n--) result *= m;
    return result;
}

/******************************************************************************
      ����˵������ʾ����
      ������ݣ�x,y �������
                num Ҫ��ʾ������
                len ���ֵ�λ��
                size �����С
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    
    for(t=0; t<len; t++)
    {
        temp = (num/mypow(10,len-t-1))%10;
        if(enshow == 0 && t < (len-1))
        {
            if(temp == 0)
            {
                LCD_ShowChar(x+(size/2)*t, y, ' ', size, 0);
                continue;
            }else enshow = 1;
        }
        LCD_ShowChar(x+(size/2)*t, y, temp+'0', size, 0);
    }
}

/******************************************************************************
      ����˵������ʾ����(������)
      ������ݣ�x,y �������
                num Ҫ��ʾ������
                len ���ֵ�λ��
                size �����С
                mode �Ƿ����
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    
    for(t=0; t<len; t++)
    {
        temp = (num/mypow(10, len-t-1))%10;
        if(enshow == 0 && t < (len-1))
        {
            if(temp == 0)
            {
                if(mode & 0X80) LCD_ShowChar(x+(size/2)*t, y, '0', size, mode&0X01);
                else LCD_ShowChar(x+(size/2)*t, y, ' ', size, mode&0X01);
                continue;
            }else enshow = 1;
        }
        LCD_ShowChar(x+(size/2)*t, y, temp+'0', size, mode&0X01);
    }
}

/******************************************************************************
      ����˵������ʾ�ַ���
      ������ݣ�x,y �������
                *p �ַ�����ʼ��ַ
                size �����С
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p)
{
    uint8_t x0 = x;
    width += x;
    height += y;
    
    while((*p <= '~') && (*p >= ' ')) // �ж��ǲ��ǷǷ��ַ�!
    {
        if(x >= width)
        {
            x = x0;
            y += size;
        }
        if(y >= height) break; // �˳�
        LCD_ShowChar(x, y, *p, size, 0);
        x += size/2;
        p++;
    }
}

/******************************************************************************
      ����˵�������ô���
      ������ݣ�sx,sy �����������
                width,height ���ڴ�С
      ����ֵ��  ��
******************************************************************************/
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t ex = sx + width - 1;
    uint16_t ey = sy + height - 1;
    
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(sx >> 8);
    LCD_WR_DATA(sx & 0xFF);
    LCD_WR_DATA(ex >> 8);
    LCD_WR_DATA(ex & 0xFF);
    
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(sy >> 8);
    LCD_WR_DATA(sy & 0xFF);
    LCD_WR_DATA(ey >> 8);
    LCD_WR_DATA(ey & 0xFF);
}

/******************************************************************************
      ����˵����������ʾ����
      ������ݣ�dir ��ʾ����
      ����ֵ��  ��
******************************************************************************/
void LCD_Display_Dir(uint8_t dir)
{
    if(dir == 0) // ����
    {
        lcddev.dir = 0;
        lcddev.width = 320;
        lcddev.height = 480;
        
        LCD_WR_REG(0x36);
        LCD_WR_DATA(0x48);
    }
    else // ����
    {
        lcddev.dir = 1;
        lcddev.width = 480;
        lcddev.height = 320;
        
        LCD_WR_REG(0x36);
        LCD_WR_DATA(0x28);
    }
}

/******************************************************************************
      ����˵����LCD���Ժ���
      ������ݣ���
      ����ֵ��  ��
******************************************************************************/
void LCD_Test(void)
{
    // ������ʾ����

    LCD_Clear(WHITE);

    // ���߲���
    LCD_DrawLine(0, 0, lcddev.width-1, lcddev.height-1);
    LCD_DrawLine(lcddev.width-1, 0, 0, lcddev.height-1);
    
    // �����β���
    LCD_DrawRectangle(20, 120, 200, 160);
    
    // ��Բ����
    LCD_Draw_Circle(120, 230, 40);
    
    // �����ɫ����
    HAL_Delay(1000);
    LCD_Clear(RED);
    HAL_Delay(1000);
    LCD_Clear(GREEN);
    HAL_Delay(1000);
    LCD_Clear(BLUE);
    HAL_Delay(1000);
    LCD_Clear(BLACK);
}
uint16_t LCD_ReadID(void)
{
    uint16_t id = 0;
    
    LCD_WR_REG(0x04);  // ��׼��ȡID����
    
    // �ն�����
    LCD_ReadReg(0x00);
    LCD_ReadReg(0x00);
    
    id = LCD_ReadReg(0x00) << 8;  // ��ȡ��8λ
    id |= LCD_ReadReg(0x00);      // ��ȡ��8λ
    
    return id;
}


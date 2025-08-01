#include "lcd.h"
#include "font.h"
#include "spi.h"
#include "main.h"

// 管理LCD重要参数
_lcd_dev lcddev;
// 默认颜色
uint16_t POINT_COLOR = 0x0000; // 画笔颜色
uint16_t BACK_COLOR = 0xFFFF;  // 背景颜色

// LCD的SPI实例，必须与CubeMX中配置一致
extern SPI_HandleTypeDef hspi1;  // 使用SPI1，根据实际配置可能需要修改为hspi2等

/******************************************************************************
      函数说明：LCD写入命令
      入口数据：cmd 命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(uint8_t cmd)
{
    LCD_CS_CLR();
    LCD_RS_CLR(); // 命令模式
    HAL_Delay(1);  // 添加短延时
    // 使用HAL库发送命令
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    
    LCD_CS_SET();
     HAL_Delay(1);  // 添加CS高电平延时
}

/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(uint8_t dat)
{
    LCD_CS_CLR();
    LCD_RS_SET(); // 数据模式
    
    // 使用HAL库发送数据
    HAL_SPI_Transmit(&hspi1, &dat, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
    
    LCD_CS_SET();
    HAL_Delay(1);
}

/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA_16Bit(uint16_t dat)
{
    uint8_t data[2];
    data[0] = dat >> 8;
    data[1] = dat;
    
    LCD_CS_CLR();
    LCD_RS_SET();
    
    // 使用HAL库发送16位数据
    HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
    
    LCD_CS_SET();
}

/******************************************************************************
      函数说明：LCD写寄存器
      入口数据：LCD_Reg 寄存器地址
                LCD_RegValue 寄存器数据
      返回值：  无
******************************************************************************/
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

/******************************************************************************
      函数说明：LCD读寄存器
      入口数据：LCD_Reg 寄存器地址
      返回值：  读到的数据
******************************************************************************/
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);
    
    uint8_t data = 0;
    // 使用HAL库接收数据，注意需要正确配置SPI为全双工模式
    HAL_SPI_Receive(&hspi1, &data, 1, HAL_MAX_DELAY);
    
    return data;
}

/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
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
      函数说明：LCD初始化函数
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_Init(void)
{
    // 初始化LCD
    LCD_RST_CLR();
    HAL_Delay(300);
    LCD_RST_SET();
    HAL_Delay(300);
    
    // 打开背光
    LCD_BL_SET();
    
    
   // 初始化寄存器 (以ST7796S为例)
   LCD_WR_REG(0x11);     // 退出睡眠
    HAL_Delay(200);
    
    LCD_WR_REG(0xF0);     // 命令集
    LCD_WR_DATA(0xC3);
    
    LCD_WR_REG(0xF0);
    LCD_WR_DATA(0x96);
    
    LCD_WR_REG(0x36);     // 内存数据访问控制
    LCD_WR_DATA(0x48);    // 正常方向
    
    LCD_WR_REG(0x3A);     // 接口像素格式
    LCD_WR_DATA(0x55);    // 16bit 565 RGB
    
    LCD_WR_REG(0xB4);     // 列反转
    LCD_WR_DATA(0x01);
    
    LCD_WR_REG(0xB7);     // 源驱动方向控制
    LCD_WR_DATA(0xC6);
    
    LCD_WR_REG(0xE8);     // 显示输出控制
    LCD_WR_DATA(0x40);
    LCD_WR_DATA(0x8A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0x19);
    LCD_WR_DATA(0xA5);
    LCD_WR_DATA(0x33);
    
    LCD_WR_REG(0xC1);     // 电源控制2
    LCD_WR_DATA(0x06);
    
    LCD_WR_REG(0xC2);     // VGH电压
    LCD_WR_DATA(0xA7);
    
    LCD_WR_REG(0xC5);     // VCOM控制
    LCD_WR_DATA(0x18);
    
    LCD_WR_REG(0xE0);     // 正极性Gamma控制
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
    
    LCD_WR_REG(0xE1);     // 负极性Gamma控制
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
    
    LCD_WR_REG(0xF0);     // 命令集
    LCD_WR_DATA(0x3C);
    
    LCD_WR_REG(0xF0);
    LCD_WR_DATA(0x69);
    
    LCD_WR_REG(0x29);    // 开启显示 
    HAL_Delay(200);
    
    // 设置LCD参数
    lcddev.width = 320;   // ST7796S宽度
    lcddev.height = 480;  // ST7796S高度
    lcddev.id = 0x7796;   // LCD ID
    lcddev.dir = 0;       // 竖屏
    lcddev.wramcmd = 0x2C;// 写入显存命令
    lcddev.setxcmd = 0x2A;// 设置x命令
    lcddev.setycmd = 0x2B;// 设置y命令

    // 清屏为白色
    LCD_Clear(WHITE);
}

/******************************************************************************
      函数说明：LCD开启显示
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_DisplayOn(void)
{
    LCD_WR_REG(0x29); // 开启显示
}

/******************************************************************************
      函数说明：LCD关闭显示
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_DisplayOff(void)
{
    LCD_WR_REG(0x28); // 关闭显示
}

/******************************************************************************
      函数说明：LCD设置光标位置
      入口数据：x,y 坐标
      返回值：  无
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
      函数说明：LCD清屏函数
      入口数据：Color 要清屏的颜色
      返回值：  无
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
      函数说明：LCD画点
      入口数据：x,y 画点坐标
      返回值：  无
******************************************************************************/
void LCD_DrawPoint(uint16_t x, uint16_t y)
{
    LCD_Address_Set(x, y, x, y); // 设置光标位置
    LCD_WR_DATA_16Bit(POINT_COLOR);
}

/******************************************************************************
      函数说明：LCD快速画点
      入口数据：x,y 坐标
                color 颜色
      返回值：  无
******************************************************************************/
void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_Address_Set(x, y, x, y);
    LCD_WR_DATA_16Bit(color);
}

/******************************************************************************
      函数说明：LCD读点
      入口数据：x,y 坐标
      返回值：  颜色值
******************************************************************************/
uint16_t LCD_ReadPoint(uint16_t x, uint16_t y)
{
    uint8_t r=0, g=0, b=0;
    
    if(x >= lcddev.width || y >= lcddev.height)
        return 0; // 超出范围
    
    LCD_SetCursor(x, y);
    LCD_WR_REG(0x2E); // 读GRAM指令
    
    // 空读
    r = LCD_ReadReg(0x00);
    
    // 获取RGB数据
    r = LCD_ReadReg(0x00);
    g = LCD_ReadReg(0x00);
    b = LCD_ReadReg(0x00);
    
    return ((r<<11) | (g<<5) | b);
}

/******************************************************************************
      函数说明：LCD画线
      入口数据：x1,y1 起点坐标
                x2,y2 终点坐标
      返回值：  无
******************************************************************************/
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t t; 
    int xerr = 0, yerr = 0, delta_x, delta_y, distance; 
    int incx, incy, row, col; 
    
    delta_x = x2 - x1; // 计算坐标增量 
    delta_y = y2 - y1; 
    row = x1; 
    col = y1; 
    
    if(delta_x > 0) incx = 1; // 设置单步方向 
    else if(delta_x == 0) incx = 0; // 垂直线 
    else {incx = -1; delta_x = -delta_x;} 
    
    if(delta_y > 0) incy = 1; 
    else if(delta_y == 0) incy = 0; // 水平线 
    else {incy = -1; delta_y = -delta_y;} 
    
    if(delta_x > delta_y) distance = delta_x; // 选取基本增量坐标轴 
    else distance = delta_y; 
    
    for(t=0; t<=distance+1; t++) // 画线输出 
    {  
        LCD_DrawPoint(row, col); // 画点 
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
      函数说明：LCD画矩形
      入口数据：x1,y1 起点坐标
                x2,y2 终点坐标
      返回值：  无
******************************************************************************/
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}

/******************************************************************************
      函数说明：LCD画圆
      入口数据：x0,y0 圆心坐标
                r 半径
      返回值：  无
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
        
        // 使用Bresenham算法画圆     
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
      函数说明：LCD填充区域
      入口数据：x1,y1 起点坐标
                x2,y2 终点坐标
                color 填充颜色
      返回值：  无
******************************************************************************/
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint16_t i, j;
    uint16_t width = ex - sx + 1;      // 得到填充的宽度
    uint16_t height = ey - sy + 1;     // 高度
    
    LCD_Address_Set(sx, sy, ex, ey);
    
    for(i=0; i<height; i++)
    {
        for(j=0; j<width; j++)
            LCD_WR_DATA_16Bit(color); // 写入数据
    }
}

/******************************************************************************
      函数说明：LCD显示一个字符
      入口数据：x,y 起点坐标
                num 要显示的字符
                size 字体大小
                mode 叠加方式(1)还是非叠加方式(0)
      返回值：  无
******************************************************************************/
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = (size/8+((size%8)?1:0))*(size/2);  // 得到字体一个字符对应点阵集所占的字节数
    
    num = num - ' '; // 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    
    for(t=0; t<csize; t++)
    {
        if(size == 12) temp = asc2_1206[num][t];        // 调用1206字体
        else if(size == 16) temp = asc2_1608[num][t];   // 调用1608字体
//        else if(size == 24) temp = asc2_2412[num][t];   // 调用2412字体
        else return;                                    // 没有的字库
        
        for(t1=0; t1<8; t1++)
        {
            if(temp & 0x80) LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if(mode == 0) LCD_Fast_DrawPoint(x, y, BACK_COLOR);
            
            temp <<= 1;
            y++;
            if(y >= lcddev.height) return;      // 超区域了
            if((y - y0) == size)
            {
                y = y0;
                x++;
                if(x >= lcddev.width) return;   // 超区域了
                break;
            }
        }
    }
}

/******************************************************************************
      函数说明：计算m的n次方
      入口数据：m,n
      返回值：  m^n的值
******************************************************************************/
uint32_t mypow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while(n--) result *= m;
    return result;
}

/******************************************************************************
      函数说明：显示数字
      入口数据：x,y 起点坐标
                num 要显示的数字
                len 数字的位数
                size 字体大小
      返回值：  无
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
      函数说明：显示数字(带符号)
      入口数据：x,y 起点坐标
                num 要显示的数字
                len 数字的位数
                size 字体大小
                mode 是否填充
      返回值：  无
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
      函数说明：显示字符串
      入口数据：x,y 起点坐标
                *p 字符串起始地址
                size 字体大小
      返回值：  无
******************************************************************************/
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p)
{
    uint8_t x0 = x;
    width += x;
    height += y;
    
    while((*p <= '~') && (*p >= ' ')) // 判断是不是非法字符!
    {
        if(x >= width)
        {
            x = x0;
            y += size;
        }
        if(y >= height) break; // 退出
        LCD_ShowChar(x, y, *p, size, 0);
        x += size/2;
        p++;
    }
}

/******************************************************************************
      函数说明：设置窗口
      入口数据：sx,sy 窗口起点坐标
                width,height 窗口大小
      返回值：  无
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
      函数说明：设置显示方向
      入口数据：dir 显示方向
      返回值：  无
******************************************************************************/
void LCD_Display_Dir(uint8_t dir)
{
    if(dir == 0) // 竖屏
    {
        lcddev.dir = 0;
        lcddev.width = 320;
        lcddev.height = 480;
        
        LCD_WR_REG(0x36);
        LCD_WR_DATA(0x48);
    }
    else // 横屏
    {
        lcddev.dir = 1;
        lcddev.width = 480;
        lcddev.height = 320;
        
        LCD_WR_REG(0x36);
        LCD_WR_DATA(0x28);
    }
}

/******************************************************************************
      函数说明：LCD测试函数
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_Test(void)
{
    // 横屏显示测试

    LCD_Clear(WHITE);

    // 画线测试
    LCD_DrawLine(0, 0, lcddev.width-1, lcddev.height-1);
    LCD_DrawLine(lcddev.width-1, 0, 0, lcddev.height-1);
    
    // 画矩形测试
    LCD_DrawRectangle(20, 120, 200, 160);
    
    // 画圆测试
    LCD_Draw_Circle(120, 230, 40);
    
    // 填充颜色测试
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
    
    LCD_WR_REG(0x04);  // 标准读取ID命令
    
    // 空读几次
    LCD_ReadReg(0x00);
    LCD_ReadReg(0x00);
    
    id = LCD_ReadReg(0x00) << 8;  // 读取高8位
    id |= LCD_ReadReg(0x00);      // 读取低8位
    
    return id;
}


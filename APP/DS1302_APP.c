#include "DS1302_APP.h"
static void DS1302_Data_Write_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	  /*Configure GPIO pin : PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
static void DS1302_Data_Read_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	  /*Configure GPIO pin : PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
void DS1302_Single_Byte_Write(uint8_t data)
{
	DS1302_Data_Write_Init();
	CLK_L;
	for(int i=0;i<8;i++)
	{
		CLK_L;
		if((data&0x01)==0x01)
			DATA_H;
		else
			DATA_L;
		data>>=0x01;
		CLK_H;
	}
}
void DS1302_Write_Register(uint8_t regAddress,uint8_t regData)
{
	RST_L;
	CLK_L;
	RST_H;
	DS1302_Single_Byte_Write(regAddress);
	DS1302_Single_Byte_Write(regData);
	RST_H;
}
uint8_t DS1302_Read_Register(uint8_t regAddress)
{
	RST_L;
	CLK_L;
	uint8_t regData=0x00;
	RST_H;
	DS1302_Single_Byte_Write(regAddress);
	DS1302_Data_Read_Init();
	for(int i=0;i<8;i++)
	{
		regData>>=0x01;
		CLK_H;
		CLK_L;
		HAL_Delay(1);
		if(DATA_READ==GPIO_PIN_SET)
			regData=regData|0x80;
	}
	RST_H;
	return regData;
}


void DS1302_Set_Time(uint8_t year,uint8_t week,uint8_t month,uint8_t day,uint8_t hour,uint8_t minute,uint8_t second)
{
	DS1302_Write_Register(W_CONTR,0x00);
	DS1302_Write_Register(W_YEAR,year);
	DS1302_Write_Register(W_WEEK,week);
	DS1302_Write_Register(W_MONTH,month);
	DS1302_Write_Register(W_DAY,day);
	DS1302_Write_Register(W_HOUR,hour);
	DS1302_Write_Register(W_MINUTE,minute);
	DS1302_Write_Register(W_SECOND,second);
	DS1302_Write_Register(W_CONTR,0x80);
}
timeNow_t DS1302_Read_Time(void)
{
	timeNow_t timeNow;
	timeNow.year=DS1302_Read_Register(R_YEAR);
	timeNow.week=DS1302_Read_Register(R_WEEK);
	timeNow.month=DS1302_Read_Register(R_MONTH);
	timeNow.day=DS1302_Read_Register(R_DAY);
	timeNow.hour=DS1302_Read_Register(R_HOUR);
	timeNow.minute=DS1302_Read_Register(R_MINUTE);
	timeNow.second=DS1302_Read_Register(R_SECOND);
	return timeNow;
}
void DS1302_Print_Time(void)
{
	timeNow_t timeNow=DS1302_Read_Time();
	my_printf(&huart1,"20%02X-%02X-%02X %02X:%02X:%02X\n",timeNow.year,timeNow.month,timeNow.day,timeNow.hour,timeNow.minute,timeNow.second);
	
}
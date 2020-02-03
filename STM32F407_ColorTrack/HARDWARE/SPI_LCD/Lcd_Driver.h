
#ifndef _LCD_DRIVER_H_
#define _LCD_DRIVER_H_

#include "stm32f4xx.h"
/////////////////////////////////////用户配置区///////////////////////////////////
//支持横竖屏快速定义切换
#define USE_HORIZONTAL          1   //定义是否使用横屏      0,不使用.1,使用.

//-----------------------------SPI 总线配置--------------------------------------//
#define USE_HARDWARE_SPI     1  //1:Enable Hardware SPI;0:USE Soft SPI

//-------------------------屏幕物理像素设置--------------------------------------//
#define LCD_X_SIZE          240
#define LCD_Y_SIZE          320

#if USE_HORIZONTAL//如果定义了横屏
#define X_MAX_PIXEL         LCD_Y_SIZE
#define Y_MAX_PIXEL         LCD_X_SIZE
#else
#define X_MAX_PIXEL         LCD_X_SIZE
#define Y_MAX_PIXEL         LCD_Y_SIZE
#endif
//////////////////////////////////////////////////////////////////////////////////







#define RED     0xf800
#define GREEN   0x07e0
#define BLUE    0x001f
#define WHITE   0xffff
#define BLACK   0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D      //灰色0 3165 00110 001011 00101
#define GRAY1   0x8410          //灰色1      00000 000000 00000
#define GRAY2   0x4208          //灰色2  1111111111011111




#define LCD_CTRL        GPIOB       //定义TFT数据端口
//#define LCD_LED           GPIO_Pin_9  //MCU_PB9--->>TFT --BL
#define LCD_RS          GPIO_Pin_7  //PB11--->>TFT --RS/DC
#define LCD_CS          GPIO_Pin_9 //MCU_PB11--->>TFT --CS/CE
#define LCD_RST             GPIO_Pin_8  //PB10--->>TFT --RST
#define LCD_SCL         GPIO_Pin_3  //PB13--->>TFT --SCL/SCK
#define LCD_SDA         GPIO_Pin_5  //PB15 MOSI--->>TFT --SDA/DIN
#define LCD_PIN    0x0380


//#define LCD_CS_SET(x) LCD_CTRL->ODR=(LCD_CTRL->ODR&~LCD_CS)|(x ? LCD_CS:0)

//液晶控制口置1操作语句宏定义
#define LCD_CS_SET      LCD_CTRL->BSRRL=LCD_CS
#define LCD_RS_SET      LCD_CTRL->BSRRL=LCD_RS
#define LCD_SDA_SET     LCD_CTRL->BSRRL=LCD_SDA
#define LCD_SCL_SET     LCD_CTRL->BSRRL=LCD_SCL
#define LCD_RST_SET     LCD_CTRL->BSRRL=LCD_RST
//#define   LCD_LED_SET     LCD_CTRL->BSRRL=LCD_LED

//液晶控制口置0操作语句宏定义
#define LCD_CS_CLR      LCD_CTRL->BSRRH=LCD_CS
#define LCD_RS_CLR      LCD_CTRL->BSRRH=LCD_RS
#define LCD_SDA_CLR     LCD_CTRL->BSRRH=LCD_SDA
#define LCD_SCL_CLR     LCD_CTRL->BSRRH=LCD_SCL
#define LCD_RST_CLR     LCD_CTRL->BSRRH=LCD_RST
//#define   LCD_LED_CLR     LCD_CTRL->BSRRH=LCD_LED


#define LCD_WR_DATA(data){\
        LCD_RS_SET;\
        LCD_CS_CLR;\
        LCD_DATAOUT(data);\
        LCD_WR_CLR;\
        LCD_WR_SET;\
        LCD_CS_SET;\
    }

void SPI_SetSpeed(SPI_TypeDef* SPIx, u8 SpeedSet);

void LCD_GPIO_Init(void);
void Lcd_WriteIndex(u8 Index);
void Lcd_WriteData(u8 Data);
void Lcd_WriteReg(u8 Index, u8 Data);
u16 Lcd_ReadReg(u8 LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(u16 Color);
void Lcd_SetXY(u16 x, u16 y);
void Gui_DrawPoint(u16 x, u16 y, u16 Data);
unsigned int Lcd_ReadPoint(u16 x, u16 y);
void Lcd_SetRegion(u16 xStar, u16 yStar, u16 xEnd, u16 yEnd);
void Lcd_WriteData_16Bit(u16 Data);

#endif

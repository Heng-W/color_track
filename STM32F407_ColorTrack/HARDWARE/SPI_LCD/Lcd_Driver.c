#include "SPI_LCD/Lcd_Driver.h"
#include "delay.h"

//---------------------------------function----------------------------------------------------//

/****************************************************************************
* 名    称：void LCD_GPIO_Init(void)
* 功    能：STM32_模拟SPI所用到的GPIO初始化
* 入口参数：无
* 出口参数：无
* 说    明：初始化模拟SPI所用的GPIO
****************************************************************************/
void LCD_GPIO_Init(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LCD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(LCD_CTRL, &GPIO_InitStructure);

}
/****************************************************************************
* 名    称：void  SPIv_WriteData(u8 Data)
* 功    能：STM32_模拟SPI写一个字节数据底层函数
* 入口参数：Data
* 出口参数：无
* 说    明：STM32_模拟SPI读写一个字节数据底层函数
****************************************************************************/
void  SPIv_WriteData(u8 Data)
{
    unsigned char i = 0;

    for (i = 8; i > 0; i--)
    {
        if (Data & 0x80)
            LCD_SDA_SET; //输出数据
        else LCD_SDA_CLR;

        LCD_SCL_CLR;
        LCD_SCL_SET;
        Data <<= 1;
    }
}

/****************************************************************************
* 名    称：u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
* 功    能：STM32_硬件SPI读写一个字节数据底层函数
* 入口参数：SPIx,Byte
* 出口参数：返回总线收到的数据
* 说    明：STM32_硬件SPI读写一个字节数据底层函数
****************************************************************************/
u8 SPI_WriteByte(SPI_TypeDef* SPIx, u8 Byte)
{
    while ((SPIx->SR & SPI_I2S_FLAG_TXE) == RESET);     //等待发送区空

    SPIx->DR = Byte;        //发送一个byte

    while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == RESET); //等待接收完一个byte

    return SPIx->DR;                 //返回收到的数据
}

/****************************************************************************
* 名    称：void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
* 功    能：设置SPI的速度
* 入口参数：SPIx,SpeedSet
* 出口参数：无
* 说    明：spi速度=fAPB2/2^(SpeedSet+1)
****************************************************************************/
void SPI_SetSpeed(SPI_TypeDef* SPIx, u8 SpeedSet)
{
    SpeedSet &= 0x07;
    SPIx->CR1 &= 0XFFC7;

    //SPIx->CR1|=SPI_BaudRatePrescaler_2;//Fsck=Fpclk/2

    SPIx->CR1 |= SpeedSet << 3; //Fsck=Fpclk/32
    SPIx->CR1 |= 1 << 6; //SPI设备使能
}


void SPI1_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//使能SPI1时钟

    //GPIOFB3,4,5初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; //PB3~5复用功能输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化


    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1); //PB3复用为 SPI1
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1); //PB4复用为 SPI1
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1); //PB5复用为 SPI1

    //这里只针对SPI口初始化
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE); //复位SPI1
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE); //停止复位SPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;       //设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;       //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;     //串行同步时钟的空闲状态为高电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;    //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;       //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;      //定义波特率预分频的值，设置为最高速率
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;    //CRC值计算的多项式
    SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI1, ENABLE); //使能SPI外设

    //SPI1_ReadWriteByte(0xff);//启动传输
}


/****************************************************************************
* 名    称：Lcd_WriteIndex(u8 Index)
* 功    能：向液晶屏写一个8位指令
* 入口参数：Index   寄存器地址
* 出口参数：无
* 说    明：调用前需先选中控制器，内部函数
****************************************************************************/
void Lcd_WriteIndex(u8 Index)
{
    LCD_CS_CLR;
    LCD_RS_CLR;
#if USE_HARDWARE_SPI
    SPI_WriteByte(SPI1, Index);
#else
    SPIv_WriteData(Index);
#endif
    LCD_CS_SET;
}

/****************************************************************************
* 名    称：Lcd_WriteData(u8 Data)
* 功    能：向液晶屏写一个8位数据
* 入口参数：dat     寄存器数据
* 出口参数：无
* 说    明：向控制器指定地址写入数据，内部函数
****************************************************************************/
void Lcd_WriteData(u8 Data)
{
    LCD_CS_CLR;
    LCD_RS_SET;
#if USE_HARDWARE_SPI
    SPI_WriteByte(SPI1, Data);
#else
    SPIv_WriteData(Data);
#endif
    LCD_CS_SET;
}

/****************************************************************************
* 名    称：void LCD_WriteReg(u8 Index,u16 Data)
* 功    能：写寄存器数据
* 入口参数：Index,Data
* 出口参数：无
* 说    明：本函数为组合函数，向Index地址的寄存器写入Data值
****************************************************************************/
void LCD_WriteReg(u8 Index, u16 Data)
{
    Lcd_WriteIndex(Index);
    Lcd_WriteData_16Bit(Data);
}

/****************************************************************************
* 名    称：void Lcd_WriteData_16Bit(u16 Data)
* 功    能：向液晶屏写一个16位数据
* 入口参数：Data
* 出口参数：无
* 说    明：向控制器指定地址写入一个16位数据
****************************************************************************/
void Lcd_WriteData_16Bit(u16 Data)
{
    Lcd_WriteData(Data >> 8);
    Lcd_WriteData(Data);
}

/****************************************************************************
* 名    称：void Lcd_Reset(void)
* 功    能：液晶硬复位函数
* 入口参数：无
* 出口参数：无
* 说    明：液晶初始化前需执行一次复位操作
****************************************************************************/
void Lcd_Reset(void)
{
    LCD_RST_CLR;
    delay_ms(100);
    LCD_RST_SET;
    delay_ms(50);
}



void Lcd_Init(void)
{
    SPI1_Init();
    LCD_GPIO_Init();

    Lcd_Reset();

    Lcd_WriteIndex(0xCB);

    Lcd_WriteData(0x39);
    Lcd_WriteData(0x2C);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x34);
    Lcd_WriteData(0x02);

    Lcd_WriteIndex(0xCF);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0XC1);
    Lcd_WriteData(0X30);

    Lcd_WriteIndex(0xE8);
    Lcd_WriteData(0x85);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x78);

    Lcd_WriteIndex(0xEA);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x00);

    Lcd_WriteIndex(0xED);
    Lcd_WriteData(0x64);
    Lcd_WriteData(0x03);
    Lcd_WriteData(0X12);
    Lcd_WriteData(0X81);

    Lcd_WriteIndex(0xF7);
    Lcd_WriteData(0x20);

    Lcd_WriteIndex(0xC0);   //Power control
    Lcd_WriteData(0x23);    //VRH[5:0]

    Lcd_WriteIndex(0xC1);   //Power control
    Lcd_WriteData(0x10);    //SAP[2:0];BT[3:0]

    Lcd_WriteIndex(0xC5);   //VCM control
    Lcd_WriteData(0x3e);    //对比度调节
    Lcd_WriteData(0x28);

    Lcd_WriteIndex(0xC7);   //VCM control2
    Lcd_WriteData(0x86);    //--

    Lcd_WriteIndex(0x36);   // Memory Access Control

    //C8//48 68竖屏//28 E8 横屏
#if USE_HORIZONTAL
    Lcd_WriteData(0xE8);//横屏参数
#else
    Lcd_WriteData(0x48);//竖屏参数
#endif

    Lcd_WriteIndex(0x3A);
    Lcd_WriteData(0x55);

    Lcd_WriteIndex(0xB1);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x18);

    Lcd_WriteIndex(0xB6);    // Display Function Control
    Lcd_WriteData(0x08);
    Lcd_WriteData(0x82);
    Lcd_WriteData(0x27);

    Lcd_WriteIndex(0xF2);    // 3Gamma Function Disable
    Lcd_WriteData(0x00);

    Lcd_WriteIndex(0x26);    //Gamma curve selected
    Lcd_WriteData(0x01);

    Lcd_WriteIndex(0xE0);    //Set Gamma
    Lcd_WriteData(0x0F);
    Lcd_WriteData(0x31);
    Lcd_WriteData(0x2B);
    Lcd_WriteData(0x0C);
    Lcd_WriteData(0x0E);
    Lcd_WriteData(0x08);
    Lcd_WriteData(0x4E);
    Lcd_WriteData(0xF1);
    Lcd_WriteData(0x37);
    Lcd_WriteData(0x07);
    Lcd_WriteData(0x10);
    Lcd_WriteData(0x03);
    Lcd_WriteData(0x0E);
    Lcd_WriteData(0x09);
    Lcd_WriteData(0x00);

    Lcd_WriteIndex(0XE1);    //Set Gamma
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x0E);
    Lcd_WriteData(0x14);
    Lcd_WriteData(0x03);
    Lcd_WriteData(0x11);
    Lcd_WriteData(0x07);
    Lcd_WriteData(0x31);
    Lcd_WriteData(0xC1);
    Lcd_WriteData(0x48);
    Lcd_WriteData(0x08);
    Lcd_WriteData(0x0F);
    Lcd_WriteData(0x0C);
    Lcd_WriteData(0x31);
    Lcd_WriteData(0x36);
    Lcd_WriteData(0x0F);

    Lcd_WriteIndex(0x11);    //Exit Sleep
    delay_ms(120);

    Lcd_WriteIndex(0x29);    //Display on
    Lcd_WriteIndex(0x2c);
}

/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void Lcd_SetXY(u16 Xpos, u16 Ypos)
{
    Lcd_WriteIndex(0x2A);
    Lcd_WriteData_16Bit(Xpos);
    Lcd_WriteIndex(0x2B);
    Lcd_WriteData_16Bit(Ypos);
    Lcd_WriteIndex(0x2c);
}
/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
//设置显示窗口
void Lcd_SetRegion(u16 xStar, u16 yStar, u16 xEnd, u16 yEnd)
{
    Lcd_WriteIndex(0x2A);
    Lcd_WriteData_16Bit(xStar);
    Lcd_WriteData_16Bit(xEnd);
    Lcd_WriteIndex(0x2B);
    Lcd_WriteData_16Bit(yStar);
    Lcd_WriteData_16Bit(yEnd);
    Lcd_WriteIndex(0x2c);
}


/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：xy坐标和颜色数据
返回值：无
*************************************************/
void Gui_DrawPoint(u16 x, u16 y, u16 Data)
{
    Lcd_SetXY(x, y);
    Lcd_WriteData_16Bit(Data);

}


/****************************************************************************
名    称：unsigned int Lcd_ReadPoint(u16 x,u16 y)
功    能：读TFT某一点的颜色
入口参数：x,y的坐标位置
出口参数：color  点颜色值
****************************************************************************/
unsigned int Lcd_ReadPoint(u16 x, u16 y)
{
    //  unsigned int Data;
    //  Lcd_SetXY(x,y);

    //  //Lcd_ReadData();//丢掉无用字节
    //  Data=Lcd_ReadData();
    //  //Lcd_WriteData(Data);
    //  return Data;


    u8 r, g, b;
    u16 R, G, B, Data;
    Lcd_SetXY(x, y);
    LCD_CS_CLR;
    Lcd_WriteIndex(0X2E);
    LCD_RS_SET;
    LCD_RS_CLR;
    SPI_WriteByte(SPI1, 0xff);
    r = SPI_WriteByte(SPI1, 0xff);
    g = SPI_WriteByte(SPI1, 0xff);
    b = SPI_WriteByte(SPI1, 0xff);
    LCD_CS_SET;
    R = (r << 1) & 0x00FF;
    G = g & 0x00FF;
    B = (b << 1) & 0x00FF;
    Data = (R << 8) | (G << 5) | (B >> 3);
    //LCD_CS_CLR;
    return Data;
}

/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_Clear(u16 Color)
{
    unsigned int i;
    Lcd_SetRegion(0, 0, X_MAX_PIXEL - 1, Y_MAX_PIXEL - 1);
    LCD_CS_CLR;
    LCD_RS_SET;

    for (i = 0; i < X_MAX_PIXEL * Y_MAX_PIXEL; i++)
    {
        //Lcd_WriteData_16Bit(Color);
#if USE_HARDWARE_SPI
        SPI_WriteByte(SPI1, Color >> 8);
        SPI_WriteByte(SPI1, Color);
#else
        SPIv_WriteData(Color >> 8);
        SPIv_WriteData(Color);
#endif
    }

    LCD_CS_SET;
}


void Lcd_DrawColor(u16 x, u16 y, u16 Color)
{
    unsigned int i, m;
    Lcd_SetRegion(x, y, X_MAX_PIXEL - 1, Y_MAX_PIXEL - 1);
    LCD_RS_SET;

    for (i = 0; i < Y_MAX_PIXEL - y; i++)
    {
        for (m = 0; m < X_MAX_PIXEL - x; m++)
        {
            SPI_WriteByte(SPI1, Color >> 8);
            SPI_WriteByte(SPI1, Color);
        }
    }
}
/****************************************************************************
名    称：Lcd_DrawColorRet
功    能：绘制方形色块
入口参数：填充颜色COLOR
出口参数：x，y-->绘制的色块的起始位置
          w，h-->色块的宽和高
          Color->色块的颜色
****************************************************************************/
void Lcd_DrawColorRet(u16 x, u16 y, u16 w, u16 h, u16 Color)
{
    unsigned int i, m;
    Lcd_SetRegion(x, y, x + w - 1, y + h - 1);
    LCD_RS_SET;

    for (i = y; i < h + y; i++)
    {
        for (m = x; m < w + x; m++)
        {
            SPI_WriteByte(SPI1, Color >> 8);
            SPI_WriteByte(SPI1, Color);
        }
    }

    Lcd_SetRegion(0, 0, X_MAX_PIXEL - 1, Y_MAX_PIXEL - 1);
    //LCD_RS_SET;
}

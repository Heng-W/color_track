#include "SPI_LCD/Lcd_Driver.h"
#include "delay.h"

//---------------------------------function----------------------------------------------------//

/****************************************************************************
* ��    �ƣ�void LCD_GPIO_Init(void)
* ��    �ܣ�STM32_ģ��SPI���õ���GPIO��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ������ʼ��ģ��SPI���õ�GPIO
****************************************************************************/
void LCD_GPIO_Init(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LCD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(LCD_CTRL, &GPIO_InitStructure);

}
/****************************************************************************
* ��    �ƣ�void  SPIv_WriteData(u8 Data)
* ��    �ܣ�STM32_ģ��SPIдһ���ֽ����ݵײ㺯��
* ��ڲ�����Data
* ���ڲ�������
* ˵    ����STM32_ģ��SPI��дһ���ֽ����ݵײ㺯��
****************************************************************************/
void  SPIv_WriteData(u8 Data)
{
    unsigned char i = 0;

    for (i = 8; i > 0; i--)
    {
        if (Data & 0x80)
            LCD_SDA_SET; //�������
        else LCD_SDA_CLR;

        LCD_SCL_CLR;
        LCD_SCL_SET;
        Data <<= 1;
    }
}

/****************************************************************************
* ��    �ƣ�u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
* ��    �ܣ�STM32_Ӳ��SPI��дһ���ֽ����ݵײ㺯��
* ��ڲ�����SPIx,Byte
* ���ڲ��������������յ�������
* ˵    ����STM32_Ӳ��SPI��дһ���ֽ����ݵײ㺯��
****************************************************************************/
u8 SPI_WriteByte(SPI_TypeDef* SPIx, u8 Byte)
{
    while ((SPIx->SR & SPI_I2S_FLAG_TXE) == RESET);     //�ȴ���������

    SPIx->DR = Byte;        //����һ��byte

    while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == RESET); //�ȴ�������һ��byte

    return SPIx->DR;                 //�����յ�������
}

/****************************************************************************
* ��    �ƣ�void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
* ��    �ܣ�����SPI���ٶ�
* ��ڲ�����SPIx,SpeedSet
* ���ڲ�������
* ˵    ����spi�ٶ�=fAPB2/2^(SpeedSet+1)
****************************************************************************/
void SPI_SetSpeed(SPI_TypeDef* SPIx, u8 SpeedSet)
{
    SpeedSet &= 0x07;
    SPIx->CR1 &= 0XFFC7;

    //SPIx->CR1|=SPI_BaudRatePrescaler_2;//Fsck=Fpclk/2

    SPIx->CR1 |= SpeedSet << 3; //Fsck=Fpclk/32
    SPIx->CR1 |= 1 << 6; //SPI�豸ʹ��
}


void SPI1_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//ʹ��SPI1ʱ��

    //GPIOFB3,4,5��ʼ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; //PB3~5���ù������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��


    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1); //PB3����Ϊ SPI1
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1); //PB4����Ϊ SPI1
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1); //PB5����Ϊ SPI1

    //����ֻ���SPI�ڳ�ʼ��
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE); //��λSPI1
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE); //ֹͣ��λSPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;       //����SPI����ģʽ:����Ϊ��SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;       //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;     //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;    //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;       //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;      //���岨����Ԥ��Ƶ��ֵ������Ϊ�������
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;    //CRCֵ����Ķ���ʽ
    SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

    SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����

    //SPI1_ReadWriteByte(0xff);//��������
}


/****************************************************************************
* ��    �ƣ�Lcd_WriteIndex(u8 Index)
* ��    �ܣ���Һ����дһ��8λָ��
* ��ڲ�����Index   �Ĵ�����ַ
* ���ڲ�������
* ˵    ��������ǰ����ѡ�п��������ڲ�����
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
* ��    �ƣ�Lcd_WriteData(u8 Data)
* ��    �ܣ���Һ����дһ��8λ����
* ��ڲ�����dat     �Ĵ�������
* ���ڲ�������
* ˵    �����������ָ����ַд�����ݣ��ڲ�����
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
* ��    �ƣ�void LCD_WriteReg(u8 Index,u16 Data)
* ��    �ܣ�д�Ĵ�������
* ��ڲ�����Index,Data
* ���ڲ�������
* ˵    ����������Ϊ��Ϻ�������Index��ַ�ļĴ���д��Dataֵ
****************************************************************************/
void LCD_WriteReg(u8 Index, u16 Data)
{
    Lcd_WriteIndex(Index);
    Lcd_WriteData_16Bit(Data);
}

/****************************************************************************
* ��    �ƣ�void Lcd_WriteData_16Bit(u16 Data)
* ��    �ܣ���Һ����дһ��16λ����
* ��ڲ�����Data
* ���ڲ�������
* ˵    �����������ָ����ַд��һ��16λ����
****************************************************************************/
void Lcd_WriteData_16Bit(u16 Data)
{
    Lcd_WriteData(Data >> 8);
    Lcd_WriteData(Data);
}

/****************************************************************************
* ��    �ƣ�void Lcd_Reset(void)
* ��    �ܣ�Һ��Ӳ��λ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����Һ����ʼ��ǰ��ִ��һ�θ�λ����
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
    Lcd_WriteData(0x3e);    //�Աȶȵ���
    Lcd_WriteData(0x28);

    Lcd_WriteIndex(0xC7);   //VCM control2
    Lcd_WriteData(0x86);    //--

    Lcd_WriteIndex(0x36);   // Memory Access Control

    //C8//48 68����//28 E8 ����
#if USE_HORIZONTAL
    Lcd_WriteData(0xE8);//��������
#else
    Lcd_WriteData(0x48);//��������
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
��������LCD_Set_XY
���ܣ�����lcd��ʾ��ʼ��
��ڲ�����xy����
����ֵ����
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
��������LCD_Set_Region
���ܣ�����lcd��ʾ�����ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
*************************************************/
//������ʾ����
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
��������LCD_DrawPoint
���ܣ���һ����
��ڲ�����xy�������ɫ����
����ֵ����
*************************************************/
void Gui_DrawPoint(u16 x, u16 y, u16 Data)
{
    Lcd_SetXY(x, y);
    Lcd_WriteData_16Bit(Data);

}


/****************************************************************************
��    �ƣ�unsigned int Lcd_ReadPoint(u16 x,u16 y)
��    �ܣ���TFTĳһ�����ɫ
��ڲ�����x,y������λ��
���ڲ�����color  ����ɫֵ
****************************************************************************/
unsigned int Lcd_ReadPoint(u16 x, u16 y)
{
    //  unsigned int Data;
    //  Lcd_SetXY(x,y);

    //  //Lcd_ReadData();//���������ֽ�
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
��������Lcd_Clear
���ܣ�ȫ����������
��ڲ����������ɫCOLOR
����ֵ����
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
��    �ƣ�Lcd_DrawColorRet
��    �ܣ����Ʒ���ɫ��
��ڲ����������ɫCOLOR
���ڲ�����x��y-->���Ƶ�ɫ�����ʼλ��
          w��h-->ɫ��Ŀ�͸�
          Color->ɫ�����ɫ
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

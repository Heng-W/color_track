#include "sys.h"
#include "delay.h"
#include "OV7670/ov7670.h"
//#include "OV7670/ov7670cfg.h"
#include "OV7670/sccb.h"
#include "EXTI/exti.h"
#include "SPI_LCD/Lcd_Driver.h"


//const u8*LMODE_TBL[6]={"Auto","Sunny","Cloudy","Office","Home","Night"};//6种光照模式
//const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};   //7种特效

void set_Cmos7670reg(void);


void GPIO_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(Cmos_RCC_CLK, ENABLE);//使能相关端口时钟

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉

    GPIO_InitStructure.GPIO_Pin  = Cmos_VSYNC_PIN;  //输入 上拉  Cmos_VSYNC
    GPIO_Init(Cmos_VSYNC_PORT, &GPIO_InitStructure);
    GPIO_SetBits(Cmos_VSYNC_PORT, Cmos_VSYNC_PIN);

    GPIO_InitStructure.GPIO_Pin  = OV7670_DATA_PIN; //数据输入 上拉
    GPIO_Init(OV7670_DATA_PORT, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉

    GPIO_InitStructure.GPIO_Pin = FIFO_PIN; // 端口配置  FIFO_WR、FIFO_RCK、FIFO_WRST、FIFO_RRST、FIFO_OE
    GPIO_Init(FIFO_PORT, &GPIO_InitStructure);
    GPIO_SetBits(FIFO_PORT, FIFO_PIN);

}

//初始化OV7670
//返回0:成功
//返回其他值:错误代码
u8 OV7670_Init(void)
{
    u8 temp;

    GPIO_Config();//设置IO

    SCCB_Init();                //初始化SCCB 的IO口

    if (SCCB_WR_Reg(0x12, 0x80)) return 1;  //复位SCCB

    delay_ms(50);

    //读取产品型号
    temp = SCCB_RD_Reg(0x0b);

    if (temp != 0x73)return 2;

    temp = SCCB_RD_Reg(0x0a);

    if (temp != 0x76)return 2;

    set_Cmos7670reg();//写寄存器
    return 0x00;    //ok
}
////////////////////////////////////////////////////////////////////////////
//OV7670功能设置
//白平衡设置
//0:自动
//1:太阳sunny
//2,阴天cloudy
//3,办公室office
//4,家里home
void OV7670_Light_Mode(u8 mode)
{
    switch (mode)
    {
        case 0://Auto，自动模式
            SCCB_WR_Reg(0X13, 0XE7); //COM8设置
            SCCB_WR_Reg(0X01, 0X00); //AWB蓝色通道增益
            SCCB_WR_Reg(0X02, 0X00); //AWB红色通道增益
            break;

        case 1://sunny
            SCCB_WR_Reg(0X13, 0XE5);
            SCCB_WR_Reg(0X01, 0X5A);
            SCCB_WR_Reg(0X02, 0X5C);
            break;

        case 2://cloudy
            SCCB_WR_Reg(0X13, 0XE5);
            SCCB_WR_Reg(0X01, 0X58);
            SCCB_WR_Reg(0X02, 0X60);
            break;

        case 3://office
            SCCB_WR_Reg(0X13, 0XE5);
            SCCB_WR_Reg(0X01, 0X84);
            SCCB_WR_Reg(0X02, 0X4c);
            break;

        case 4://home
            SCCB_WR_Reg(0X13, 0XE5);
            SCCB_WR_Reg(0X01, 0X96);
            SCCB_WR_Reg(0X02, 0X40);
            break;
    }
}
//色度设置
//sat:-2~+2
void OV7670_Color_Saturation(s8 sat)
{
    u8 reg4f5054val = 0X80;
    u8 reg52val = 0X22;
    u8 reg53val = 0X5E;

    switch (sat)
    {
        case -2:
            reg4f5054val = 0X40;
            reg52val = 0X11;
            reg53val = 0X2F;
            break;

        case -1:
            reg4f5054val = 0X66;
            reg52val = 0X1B;
            reg53val = 0X4B;
            break;

        case 0://默认就是sat=0,即不调节色度的设置
            reg4f5054val = 0X80;
            reg52val = 0X22;
            reg53val = 0X5E;
            break;

        case 1:
            reg4f5054val = 0X99;
            reg52val = 0X28;
            reg53val = 0X71;
            break;

        case 2:
            reg4f5054val = 0XC0;
            reg52val = 0X33;
            reg53val = 0X8D;
            break;
    }

    SCCB_WR_Reg(0X4F, reg4f5054val);    //色彩矩阵系数1
    SCCB_WR_Reg(0X50, reg4f5054val);    //色彩矩阵系数2
    SCCB_WR_Reg(0X51, 0X00);            //色彩矩阵系数3
    SCCB_WR_Reg(0X52, reg52val);        //色彩矩阵系数4
    SCCB_WR_Reg(0X53, reg53val);        //色彩矩阵系数5
    SCCB_WR_Reg(0X54, reg4f5054val);    //色彩矩阵系数6
    SCCB_WR_Reg(0X58, 0X9E);            //MTXS
}
//亮度设置
//bright：-2~+2
void OV7670_Brightness(s8 bright)
{
    u8 reg55val = 0X00; //默认就是bright=0

    switch (bright)
    {
        case -2://-2
            reg55val = 0XB0;
            break;

        case -1://-1
            reg55val = 0X98;
            break;

        case 0:
            reg55val = 0X00; //默认就是bright=0
            break;

        case 1://1
            reg55val = 0X18;
            break;

        case 2://2
            reg55val = 0X30;
            break;
    }

    SCCB_WR_Reg(0X55, reg55val);    //亮度调节
}
//对比度设置
//contrast：-2~+2
void OV7670_Contrast(s8 contrast)
{
    u8 reg56val = 0X40; //默认就是contrast=0

    switch (contrast)
    {
        case -2:
            reg56val = 0X30;
            break;

        case -1:
            reg56val = 0X38;
            break;

        case 0:
            reg56val = 0X40;
            break;

        case 1:
            reg56val = 0X50;
            break;

        case 2:
            reg56val = 0X60;
            break;
    }

    SCCB_WR_Reg(0X56, reg56val);    //对比度调节
}
//特效设置
//0:普通模式
//1,负片
//2,黑白
//3,偏红色
//4,偏绿色
//5,偏蓝色
//6,复古
void OV7670_Special_Effects(u8 eft)
{
    switch (eft)
    {
        case 0://正常
            SCCB_WR_Reg(0X3A, 0X04); //TSLB设置
            SCCB_WR_Reg(0X67, 0X80); //MANV,手动V值
            SCCB_WR_Reg(0X68, 0XC0); //MANU,手动U值
            break;

        case 1://负片
            SCCB_WR_Reg(0X3A, 0X24);
            SCCB_WR_Reg(0X67, 0X80);
            SCCB_WR_Reg(0X68, 0X80);
            break;

        case 2://黑白
            SCCB_WR_Reg(0X3A, 0X14);
            SCCB_WR_Reg(0X67, 0X80);
            SCCB_WR_Reg(0X68, 0X80);
            break;

        case 3://偏红色
            SCCB_WR_Reg(0X3A, 0X14);
            SCCB_WR_Reg(0X67, 0X80);
            SCCB_WR_Reg(0X68, 0Xc0);
            break;

        case 4://偏绿色
            SCCB_WR_Reg(0X3A, 0X14);
            SCCB_WR_Reg(0X67, 0X40);
            SCCB_WR_Reg(0X68, 0X40);
            break;

        case 5://偏蓝色
            SCCB_WR_Reg(0X3A, 0X14);
            SCCB_WR_Reg(0X67, 0XC0);
            SCCB_WR_Reg(0X68, 0X80);
            break;

        case 6://复古
            SCCB_WR_Reg(0X3A, 0X14);
            SCCB_WR_Reg(0X67, 0X40);
            SCCB_WR_Reg(0X68, 0XA0);
            break;
    }
}
//设置图像输出窗口
//对QVGA设置。
void OV7670_Window_Set(u16 sx, u16 sy, u16 width, u16 height)
{
    u16 endx;
    u16 endy;
    u8 temp;
    endx = sx + width * 2;  //V*2
    endy = sy + height * 2;

    if (endy > 784)endy -= 784;

    temp = SCCB_RD_Reg(0X03);               //读取Vref之前的值
    temp &= 0XF0;
    temp |= ((endx & 0X03) << 2) | (sx & 0X03);
    SCCB_WR_Reg(0X03, temp);                //设置Vref的start和end的最低2位
    SCCB_WR_Reg(0X19, sx >> 2);         //设置Vref的start高8位
    SCCB_WR_Reg(0X1A, endx >> 2);           //设置Vref的end的高8位

    temp = SCCB_RD_Reg(0X32);               //读取Href之前的值
    temp &= 0XC0;
    temp |= ((endy & 0X07) << 3) | (sy & 0X07);
    SCCB_WR_Reg(0X17, sy >> 3);         //设置Href的start高8位
    SCCB_WR_Reg(0X18, endy >> 3);           //设置Href的end的高8位
}


void set_Cmos7670reg(void)
{
    SCCB_WR_Reg(0x3a, 0x04);
    SCCB_WR_Reg(0x40, 0xd0);
    SCCB_WR_Reg(0x12, 0x14);  //输出格式为：QVGA(320*240) RGB
    //SCCB_WR_Reg(0x12, 0x08);  //输出格式为：QCIF:176?44,
    SCCB_WR_Reg(0x32, 0x80);
    SCCB_WR_Reg(0x17, 0x16);
    SCCB_WR_Reg(0x18, 0x04);
    SCCB_WR_Reg(0x19, 0x02);
    SCCB_WR_Reg(0x1a, 0x7b);
    SCCB_WR_Reg(0x03, 0x06);
    SCCB_WR_Reg(0x0c, 0x0c);
    SCCB_WR_Reg(0x15, 0x02);
    SCCB_WR_Reg(0x3e, 0x00);
    SCCB_WR_Reg(0x70, 0x3a);
    SCCB_WR_Reg(0x71, 0x01);
    SCCB_WR_Reg(0x72, 0x11);
    SCCB_WR_Reg(0x73, 0x00);
    SCCB_WR_Reg(0xa2, 0x02);
    SCCB_WR_Reg(0x11, 0x81);

    SCCB_WR_Reg(0x7a, 0x20);
    SCCB_WR_Reg(0x7b, 0x1c);
    SCCB_WR_Reg(0x7c, 0x28);
    SCCB_WR_Reg(0x7d, 0x3c);
    SCCB_WR_Reg(0x7e, 0x55);
    SCCB_WR_Reg(0x7f, 0x68);
    SCCB_WR_Reg(0x80, 0x76);
    SCCB_WR_Reg(0x81, 0x80);
    SCCB_WR_Reg(0x82, 0x88);
    SCCB_WR_Reg(0x83, 0x8f);
    SCCB_WR_Reg(0x84, 0x96);
    SCCB_WR_Reg(0x85, 0xa3);
    SCCB_WR_Reg(0x86, 0xaf);
    SCCB_WR_Reg(0x87, 0xc4);
    SCCB_WR_Reg(0x88, 0xd7);
    SCCB_WR_Reg(0x89, 0xe8);

    SCCB_WR_Reg(0x13, 0xe7);
    SCCB_WR_Reg(0x00, 0x00);
    SCCB_WR_Reg(0x10, 0x00);
    SCCB_WR_Reg(0x0d, 0x00);
    SCCB_WR_Reg(0x14, 0x20);
    SCCB_WR_Reg(0xa5, 0x05);
    SCCB_WR_Reg(0xab, 0x07);
    SCCB_WR_Reg(0x24, 0x75);
    SCCB_WR_Reg(0x25, 0x63);
    SCCB_WR_Reg(0x26, 0xA5);
    SCCB_WR_Reg(0x9f, 0x78);
    SCCB_WR_Reg(0xa0, 0x68);
    SCCB_WR_Reg(0xa1, 0x03);
    SCCB_WR_Reg(0xa6, 0xdf);
    SCCB_WR_Reg(0xa7, 0xdf);
    SCCB_WR_Reg(0xa8, 0xf0);
    SCCB_WR_Reg(0xa9, 0x90);
    SCCB_WR_Reg(0xaa, 0x94);//AEC公式选择：基于直方图的AEC算法；SCCB_WR_Reg(0xaa, 0x94)->基于平均值的AEC算法
    SCCB_WR_Reg(0x13, 0xe5);

    SCCB_WR_Reg(0x0e, 0x61);
    SCCB_WR_Reg(0x0f, 0x4b);
    SCCB_WR_Reg(0x16, 0x02);
    SCCB_WR_Reg(0x1e, 0x37);//翻转镜头图像SCCB_WR_Reg(0x1e, 0x27);上下颠倒SCCB_WR_Reg(0x1e, 0x01);
    SCCB_WR_Reg(0x21, 0x02);
    SCCB_WR_Reg(0x22, 0x91);
    SCCB_WR_Reg(0x29, 0x07);
    SCCB_WR_Reg(0x33, 0x0b);
    SCCB_WR_Reg(0x35, 0x0b);
    SCCB_WR_Reg(0x37, 0x1d);
    SCCB_WR_Reg(0x38, 0x71);
    SCCB_WR_Reg(0x39, 0x2a);
    SCCB_WR_Reg(0x3c, 0x78);
    SCCB_WR_Reg(0x4d, 0x40);
    SCCB_WR_Reg(0x4e, 0x20);
    SCCB_WR_Reg(0x69, 0x5d);//固定增益控制
    SCCB_WR_Reg(0x6b, 0x40);
    SCCB_WR_Reg(0x74, 0x19);
    SCCB_WR_Reg(0x8d, 0x4f);
    SCCB_WR_Reg(0x8e, 0x00);
    SCCB_WR_Reg(0x8f, 0x00);
    SCCB_WR_Reg(0x90, 0x00);
    SCCB_WR_Reg(0x91, 0x00);
    SCCB_WR_Reg(0x92, 0x00);
    SCCB_WR_Reg(0x96, 0x00);
    SCCB_WR_Reg(0x9a, 0x80);
    SCCB_WR_Reg(0xb0, 0x84);
    SCCB_WR_Reg(0xb1, 0x0c);
    SCCB_WR_Reg(0xb2, 0x0e);
    SCCB_WR_Reg(0xb3, 0x82);
    SCCB_WR_Reg(0xb8, 0x0a);

    SCCB_WR_Reg(0x43, 0x14);
    SCCB_WR_Reg(0x44, 0xf0);
    SCCB_WR_Reg(0x45, 0x34);
    SCCB_WR_Reg(0x46, 0x58);
    SCCB_WR_Reg(0x47, 0x28);
    SCCB_WR_Reg(0x48, 0x3a);
    SCCB_WR_Reg(0x59, 0x88);
    SCCB_WR_Reg(0x5a, 0x88);
    SCCB_WR_Reg(0x5b, 0x44);
    SCCB_WR_Reg(0x5c, 0x67);
    SCCB_WR_Reg(0x5d, 0x49);
    SCCB_WR_Reg(0x5e, 0x0e);
    SCCB_WR_Reg(0x64, 0x04);
    SCCB_WR_Reg(0x65, 0x20);
    SCCB_WR_Reg(0x66, 0x05);
    SCCB_WR_Reg(0x94, 0x04);
    SCCB_WR_Reg(0x95, 0x08);

    SCCB_WR_Reg(0x6c, 0x0a);
    SCCB_WR_Reg(0x6d, 0x55);

    SCCB_WR_Reg(0x4f, 0x80);//色彩矩阵系数
    SCCB_WR_Reg(0x50, 0x80);
    SCCB_WR_Reg(0x51, 0x00);
    SCCB_WR_Reg(0x52, 0x22);
    SCCB_WR_Reg(0x53, 0x5e);
    SCCB_WR_Reg(0x54, 0x80);

    SCCB_WR_Reg(0x6e, 0x11);
    SCCB_WR_Reg(0x6f, 0x9f);
    SCCB_WR_Reg(0x55, 0x00);
    SCCB_WR_Reg(0x56, 0x40);
    SCCB_WR_Reg(0x58, 0x9e);
}







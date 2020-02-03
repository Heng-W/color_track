#include "EXTI/exti.h"
#include "delay.h"
#include "led.h"
#include "usart.h"
#include "OV7670/ov7670.h"
#include "EasyTracer/EasyTracer.h"
#include "SPI_LCD/Lcd_Driver.h"
#include "SPI_LCD/GUI.h"
#include "MOTOR/motor.h"
//////////////////////////////////////////////////////////////////////////////////


u8 ov_sta;//帧中断标记
u16 ov_frame; //帧率

int find_flag;
u8 color_idx;
u8 free_def_flag;
u16 lcd_data[TRACK_H][TRACK_W] = {0};
RESULT Resu = {160, 120, DEFAULT_W, DEFAULT_H};

//预定义颜色的判断条件
TARGET_CONDI Condition[] =
{
    {0, 15, 90, 240, 80, 160, DEFAULT_W, DEFAULT_H, TRACK_W, TRACK_H}, //红
    {68, 108, 170, 240, 110, 170, DEFAULT_W, DEFAULT_H, TRACK_W, TRACK_H}, //绿
    {97, 137, 65, 145, 85, 145, DEFAULT_W, DEFAULT_H, TRACK_W, TRACK_H}, //蓝
    {7, 47, 110, 180, 100, 170, DEFAULT_W, DEFAULT_H, TRACK_W, TRACK_H}, //黄
    {0, 240, 0, 240, 0, 240, DEFAULT_W, DEFAULT_H, TRACK_W, TRACK_H}
};

#define COLOR_CNT (sizeof(Condition)/sizeof(Condition[0]))

//设定追踪物体最小宽高
void set_size(unsigned int w, unsigned int h)
{
    for (int i = 0; i < COLOR_CNT; i++)
    {
        Condition[i].WIDTH_MIN = w;
        Condition[i].HIGHT_MIN = h;
    }
}


void EXTI4_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line4) == SET)    //是4线的中断
    {
        EXTI_ClearITPendingBit(EXTI_Line4);  //清除EXTI4线路挂起位
        ov_sta++;       //帧中断加1

        if (ov_sta == 1)
        {
            delay_us(1);
            OV7670_WRST = 0;    //复位写指针
            delay_us(1);
            OV7670_WRST = 1;
            delay_us(1);
            OV7670_WREN = 1;    //允许写入FIFO
            delay_us(1);
        }

        if (ov_sta == 2)
        {
            OV7670_WRST = 0;
            EXTI->IMR &= ~(EXTI_Line4);//关中断
            OV7670_RRST = 0;                //开始复位读指针
            OV7670_RCK_L;
            OV7670_RCK_H;
            OV7670_RCK_L;
            OV7670_RCK_H;
            OV7670_RRST = 1;                //复位读指针结束

            OV7670_CS = 0;                  //使能

            Lcd_SetXY(0, 0);

            u16 j1, j2;
            u8 color_h, color_l;

            u16 track_startx = 160 - TRACK_W / 2;
            u16 track_endx = 160 + TRACK_W / 2;
            u16 track_starty = 120 - TRACK_H / 2;
            u16 track_endy = 120 + TRACK_H / 2;

            for (j1 = 0; j1 < 240; j1++)
            {
                for (j2 = 0; j2 < 320; j2++)
                {
                    OV7670_RCK_L;
                    OV7670_RCK_H;
                    color_h = OV7670_DATA;  //读数据

                    OV7670_RCK_L;
                    OV7670_RCK_H;
                    color_l = OV7670_DATA;  //读数据

                    Lcd_WriteData(color_h);
                    Lcd_WriteData(color_l);

                    //选取中间的240×240存储到RAM中，进行数据处理
                    if (j2 >= track_startx && j2 < track_endx && j1 >= track_starty && j1 < track_endy)
                    {
                        lcd_data[j1 - track_starty][j2 - track_startx] = (color_h << 8) | color_l;
                    }
                }
            }

            Lcd_SetXY(0, 0);

            RESULT Result;

            if (!free_def_flag)
                find_flag = Trace(&Condition[color_idx], &Result); //预定义颜色根据设定好的阈值判断
            else
                find_flag = Trace(&condi_set, &Result); //自定义颜色根据计算出的阈值判断

            if (find_flag)
            {
                Resu = Result;
                Gui_box(Resu.x - Resu.w / 2, Resu.y - Resu.h / 2, Resu.w, Resu.h, RED); //识别到目标则在LCD上标记出来
                LED_chg(0);
            }

            OV7670_CS = 1; //禁止FIFO
            ov_sta = 0;                 //清零帧中断标记
            ov_frame++;         //帧率计数值加一
            EXTI->IMR |= (EXTI_Line4);//打开外部中断
        }
    }
}


void EXTI4_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟


    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);
    //GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);//PA8对中断线8

    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);     //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;            //使能外部中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;   //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                  //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                             //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}



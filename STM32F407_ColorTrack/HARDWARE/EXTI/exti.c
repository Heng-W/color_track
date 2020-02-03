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


u8 ov_sta;//֡�жϱ��
u16 ov_frame; //֡��

int find_flag;
u8 color_idx;
u8 free_def_flag;
u16 lcd_data[TRACK_H][TRACK_W] = {0};
RESULT Resu = {160, 120, DEFAULT_W, DEFAULT_H};

//Ԥ������ɫ���ж�����
TARGET_CONDI Condition[] =
{
    {0, 15, 90, 240, 80, 160, DEFAULT_W, DEFAULT_H, TRACK_W, TRACK_H}, //��
    {68, 108, 170, 240, 110, 170, DEFAULT_W, DEFAULT_H, TRACK_W, TRACK_H}, //��
    {97, 137, 65, 145, 85, 145, DEFAULT_W, DEFAULT_H, TRACK_W, TRACK_H}, //��
    {7, 47, 110, 180, 100, 170, DEFAULT_W, DEFAULT_H, TRACK_W, TRACK_H}, //��
    {0, 240, 0, 240, 0, 240, DEFAULT_W, DEFAULT_H, TRACK_W, TRACK_H}
};

#define COLOR_CNT (sizeof(Condition)/sizeof(Condition[0]))

//�趨׷��������С���
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
    if (EXTI_GetITStatus(EXTI_Line4) == SET)    //��4�ߵ��ж�
    {
        EXTI_ClearITPendingBit(EXTI_Line4);  //���EXTI4��·����λ
        ov_sta++;       //֡�жϼ�1

        if (ov_sta == 1)
        {
            delay_us(1);
            OV7670_WRST = 0;    //��λдָ��
            delay_us(1);
            OV7670_WRST = 1;
            delay_us(1);
            OV7670_WREN = 1;    //����д��FIFO
            delay_us(1);
        }

        if (ov_sta == 2)
        {
            OV7670_WRST = 0;
            EXTI->IMR &= ~(EXTI_Line4);//���ж�
            OV7670_RRST = 0;                //��ʼ��λ��ָ��
            OV7670_RCK_L;
            OV7670_RCK_H;
            OV7670_RCK_L;
            OV7670_RCK_H;
            OV7670_RRST = 1;                //��λ��ָ�����

            OV7670_CS = 0;                  //ʹ��

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
                    color_h = OV7670_DATA;  //������

                    OV7670_RCK_L;
                    OV7670_RCK_H;
                    color_l = OV7670_DATA;  //������

                    Lcd_WriteData(color_h);
                    Lcd_WriteData(color_l);

                    //ѡȡ�м��240��240�洢��RAM�У��������ݴ���
                    if (j2 >= track_startx && j2 < track_endx && j1 >= track_starty && j1 < track_endy)
                    {
                        lcd_data[j1 - track_starty][j2 - track_startx] = (color_h << 8) | color_l;
                    }
                }
            }

            Lcd_SetXY(0, 0);

            RESULT Result;

            if (!free_def_flag)
                find_flag = Trace(&Condition[color_idx], &Result); //Ԥ������ɫ�����趨�õ���ֵ�ж�
            else
                find_flag = Trace(&condi_set, &Result); //�Զ�����ɫ���ݼ��������ֵ�ж�

            if (find_flag)
            {
                Resu = Result;
                Gui_box(Resu.x - Resu.w / 2, Resu.y - Resu.h / 2, Resu.w, Resu.h, RED); //ʶ��Ŀ������LCD�ϱ�ǳ���
                LED_chg(0);
            }

            OV7670_CS = 1; //��ֹFIFO
            ov_sta = 0;                 //����֡�жϱ��
            ov_frame++;         //֡�ʼ���ֵ��һ
            EXTI->IMR |= (EXTI_Line4);//���ⲿ�ж�
        }
    }
}


void EXTI4_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��


    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);
    //GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);//PA8���ж���8

    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);     //����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;            //ʹ���ⲿ�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;   //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                  //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                             //ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);       //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}



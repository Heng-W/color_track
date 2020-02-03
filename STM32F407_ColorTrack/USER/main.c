#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "OV7670/ov7670.h"
#include "EXTI/exti.h"
#include "TIMER/timer.h"
#include "MOTOR/motor.h"
#include "SPI_LCD/Lcd_Driver.h"
#include "SPI_LCD/GUI.h"


char dispbuf[30];

//����LCD��ʾ
void LCD_refresh(void)
{
    if (LCD_refresh_flag)
    {
        LCD_refresh_flag = 0;
        sprintf(dispbuf, "rate:%d", ov_frame);
        Gui_DrawFont_GBK32(50, 10, BLUE, WHITE, (u8*)dispbuf);
        LED_chg(1);
    }
}


int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
    delay_init(168);      //��ʼ����ʱ����
    uart_init(9600);        //��ʼ�����ڲ�����Ϊ9600

    LED_Init();
    Lcd_Init();

    Lcd_Clear(GRAY1);

    PWM_Init();             //��ʼ����·50Hz��PWM
    Motor_SetAngle(CHx_MOTOR_H, 90);  //�����ʼ��Ϊ90��
    Motor_SetAngle(CHx_MOTOR_V, 90);

    u8 ret = OV7670_Init(); //����ͷ��ʼ��������0��ɹ�

    if (ret == 0)
    {
        EXTI4_Init();       //����֡�жϼ��
    }
    else
    {
        sprintf(dispbuf, "ret %d", ret);
        Gui_DrawFont_GBK32(50, 50, BLUE, WHITE, (u8*)dispbuf);
    }

    Time2_init();//��ʱ�ж�1ms

    while (1)
    {
        //LCD_refresh();//lcd����
        usart_dispose();//�����¼�����
    }
}

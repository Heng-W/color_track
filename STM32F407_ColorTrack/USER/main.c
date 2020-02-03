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

//更新LCD显示
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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
    delay_init(168);      //初始化延时函数
    uart_init(9600);        //初始化串口波特率为9600

    LED_Init();
    Lcd_Init();

    Lcd_Clear(GRAY1);

    PWM_Init();             //初始化两路50Hz的PWM
    Motor_SetAngle(CHx_MOTOR_H, 90);  //舵机初始化为90度
    Motor_SetAngle(CHx_MOTOR_V, 90);

    u8 ret = OV7670_Init(); //摄像头初始化，返回0则成功

    if (ret == 0)
    {
        EXTI4_Init();       //开启帧中断检测
    }
    else
    {
        sprintf(dispbuf, "ret %d", ret);
        Gui_DrawFont_GBK32(50, 50, BLUE, WHITE, (u8*)dispbuf);
    }

    Time2_init();//定时中断1ms

    while (1)
    {
        //LCD_refresh();//lcd更新
        usart_dispose();//串口事件处理
    }
}

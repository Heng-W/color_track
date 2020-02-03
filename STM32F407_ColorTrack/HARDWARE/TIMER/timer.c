#include "TIMER/timer.h"
#include "led.h"
#include <stdio.h>
#include "MOTOR/motor.h"


u8 LCD_refresh_flag;
u8 start_flag = 0;
u16 fps;

u16 event_cnt;

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

        if (event_cnt % 50 == 0)
        {
            if (start_flag)
                Motor_Action();     //50ms舵机动作一次
        }

        if (event_cnt % 500 == 0)
        {
            send_message();//500ms向APP发送一次信息
        }

        if (event_cnt % 1000 == 0)
        {
            fps = ov_frame; //记录帧率
            ov_frame = 0;       //帧率计数清零
            //LCD_refresh_flag=1;//1s更新一次LCD
        }

        if (++event_cnt >= 1000) event_cnt = 0;//超过1s计数值清零
    }
}


/*
void TIM4_IRQHandler(void)   //10ms
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM4,TIM_IT_Update); //清标志
    key_scan();
    }
}*/



void Time2_init()

{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //打开相应的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);


    //设置定时器TIM_TimeBaseInit初始化定时器的时钟基数
    TIM_TimeBaseStructure.TIM_Period = TIM2_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = TIM2_PSC;       //T=(1+TIM_Prescaler )/84M)*(1+TIM_Period)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, & TIM_TimeBaseStructure);


    //使能指定的TIM中断
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    //使能TIM定时器
    TIM_Cmd(TIM2, ENABLE);


    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void Time4_init()

{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //打开相应的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);



    //设置定时器TIM_TimeBaseInit初始化定时器的时钟基数
    TIM_TimeBaseStructure.TIM_Period = TIM4_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler = TIM4_PSC;       //T=(1+TIM_Prescaler )/72M)*(1+TIM_Period)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, & TIM_TimeBaseStructure);


    //使能指定的TIM中断
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    //使能TIM定时器
    TIM_Cmd(TIM4, ENABLE);


    //设置NVIC
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}









#include "MOTOR/motor.h"


float g_angle_H = 90, g_angle_V = 90;

u32 CCR1_Val, CCR2_Val, CCR3_Val, CCR4_Val;

void PWM_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(GENERAL_TIM_GPIO_CLK, ENABLE);

    //GPIO_PinAFConfig(GENERAL_TIM_CH1_PORT,GENERAL_TIM_CH1_PIN_SOURCE,GENERAL_TIM_GPIO_AF);
    //GPIO_PinAFConfig(GENERAL_TIM_CH2_PORT,GENERAL_TIM_CH2_PIN_SOURCE,GENERAL_TIM_GPIO_AF);
    GPIO_PinAFConfig(GENERAL_TIM_CH3_PORT, GENERAL_TIM_CH3_PIN_SOURCE, GENERAL_TIM_GPIO_AF);
    GPIO_PinAFConfig(GENERAL_TIM_CH4_PORT, GENERAL_TIM_CH4_PIN_SOURCE, GENERAL_TIM_GPIO_AF);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉



    /*  // 输出比较通道1 GPIO 初始化
      GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH1_PIN;
      GPIO_Init(GENERAL_TIM_CH1_PORT, &GPIO_InitStructure);

        // 输出比较通道2 GPIO 初始化
      GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH2_PIN;
      GPIO_Init(GENERAL_TIM_CH2_PORT, &GPIO_InitStructure);*/

    // 输出比较通道3 GPIO 初始化
    GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH3_PIN;
    GPIO_Init(GENERAL_TIM_CH3_PORT, &GPIO_InitStructure);

    // 输出比较通道4 GPIO 初始化
    GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH4_PIN;
    GPIO_Init(GENERAL_TIM_CH4_PORT, &GPIO_InitStructure);
}



void PWM_Mode_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK, ENABLE);

    //  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3 , ENABLE);    //重映射

    TIM_TimeBaseStructure.TIM_Period = GENERAL_TIM_Period;
    TIM_TimeBaseStructure.TIM_Prescaler = GENERAL_TIM_Prescaler;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);

    /*--------------------输出比较结构体初始化-------------------*/

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//pwm1、high；pwm2、low

    /*  // 输出比较通道 1
        TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
        TIM_OC1Init(GENERAL_TIM, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);

        // 输出比较通道 2
        TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
        TIM_OC2Init(GENERAL_TIM, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);*/

    // 输出比较通道 3
    TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
    TIM_OC3Init(GENERAL_TIM, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);

    // 输出比较通道 4
    TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
    TIM_OC4Init(GENERAL_TIM, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(GENERAL_TIM, ENABLE); //ARPE使能，等待下次事件发生更新影子寄存器
    TIM_Cmd(GENERAL_TIM, ENABLE);// 使能计数器
}

void PWM_Init(void)
{
    PWM_GPIO_Config();
    PWM_Mode_Config();
}

//设置通道计数器比较值
void PWM_SetCCRX(u8 num, u32 val)
{
    switch (num)
    {
        case 1:
            GENERAL_TIM->CCR1 = val;
            break;

        case 2:
            GENERAL_TIM->CCR2 = val;
            break;

        case 3:
            GENERAL_TIM->CCR3 = val;
            break;

        case 4:
            GENERAL_TIM->CCR4 = val;
            break;

        default:
            break;
    }
}

//获得通道计数器比较值
u32 PWM_GetCCRX(u8 num)
{
    switch (num)
    {
        case 1:
            return GENERAL_TIM->CCR1;

        case 2:
            return GENERAL_TIM->CCR2;

        case 3:
            return GENERAL_TIM->CCR3;

        case 4:
            return GENERAL_TIM->CCR4;

        default:
            return 0;
    }
}


//设置舵机角度，num为通道
void Motor_SetAngle(u8 num, float angle)
{
    /*if (angle < 0) angle = 0;
    else if (angle > 180) angle = 180;  */

    double duty = ((double)angle) / 18 + 2.5;   //0-180度对应2.5-12.5占空比
    PWM_SetCCRX(num, duty * DUTY_TO_CCRX_RATIO);
}

//获得舵机角度
float Motor_GetAngle(u8 num)
{
    u32 ccrx = PWM_GetCCRX(num);

    double duty = 1.0 * ccrx / DUTY_TO_CCRX_RATIO;
    float angle = 18 * (duty - 2.5);

    if (angle < 0) angle = 0;
    else if (angle > 180) angle = 180;

    return angle;
}

//在原基础上增加或减少角度
void Motor_AddToAngle(u8 num, float add_angle)
{
    float angle = Motor_GetAngle(num) + add_angle;

    if (angle < 0) angle = 0;
    else if (angle > 180) angle = 180;

    Motor_SetAngle(num, angle);
}


#define BOUND 10

#define ANGLE_MIN 20    //舵机最小角度
#define ANGLE_MAX 160 //舵机最大角度

float Kp = 0.012, Kd = 0.001;

float last_err = 0;

//舵机PD控制
float update_pid(float setpoint, float real_val)
{
    float out = 0;
    float err = setpoint - real_val;

    if (err < BOUND && err > -BOUND)   //与中心点误差小于bound则舵机不运动
    {
        last_err = err;
        return 0;
    }

    out = Kp * err + Kd * (err - last_err);
    last_err = err;
    return out;
}

//舵机动作控制
void Motor_Action(void)
{
    if (!find_flag) return;//未识别到目标返回

    float angle;

    angle = g_angle_H + update_pid(160, Resu.x);

    //限幅
    if (angle < ANGLE_MIN) g_angle_H = ANGLE_MIN;
    else if (angle > ANGLE_MAX) g_angle_H = ANGLE_MAX;
    else g_angle_H = angle;

    //设置水平方向舵机角度
    Motor_SetAngle(CHx_MOTOR_H, g_angle_H);

    angle = g_angle_V - update_pid(120, Resu.y);

    if (angle < ANGLE_MIN) g_angle_V = ANGLE_MIN;
    else if (angle > ANGLE_MAX) g_angle_V = ANGLE_MAX;
    else g_angle_V = angle;

    //设置垂直方向舵机角度
    Motor_SetAngle(CHx_MOTOR_V, g_angle_V);

}

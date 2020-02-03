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
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����



    /*  // ����Ƚ�ͨ��1 GPIO ��ʼ��
      GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH1_PIN;
      GPIO_Init(GENERAL_TIM_CH1_PORT, &GPIO_InitStructure);

        // ����Ƚ�ͨ��2 GPIO ��ʼ��
      GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH2_PIN;
      GPIO_Init(GENERAL_TIM_CH2_PORT, &GPIO_InitStructure);*/

    // ����Ƚ�ͨ��3 GPIO ��ʼ��
    GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH3_PIN;
    GPIO_Init(GENERAL_TIM_CH3_PORT, &GPIO_InitStructure);

    // ����Ƚ�ͨ��4 GPIO ��ʼ��
    GPIO_InitStructure.GPIO_Pin =  GENERAL_TIM_CH4_PIN;
    GPIO_Init(GENERAL_TIM_CH4_PORT, &GPIO_InitStructure);
}



void PWM_Mode_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK, ENABLE);

    //  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3 , ENABLE);    //��ӳ��

    TIM_TimeBaseStructure.TIM_Period = GENERAL_TIM_Period;
    TIM_TimeBaseStructure.TIM_Prescaler = GENERAL_TIM_Prescaler;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);

    /*--------------------����ȽϽṹ���ʼ��-------------------*/

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//pwm1��high��pwm2��low

    /*  // ����Ƚ�ͨ�� 1
        TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
        TIM_OC1Init(GENERAL_TIM, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);

        // ����Ƚ�ͨ�� 2
        TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
        TIM_OC2Init(GENERAL_TIM, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);*/

    // ����Ƚ�ͨ�� 3
    TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
    TIM_OC3Init(GENERAL_TIM, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);

    // ����Ƚ�ͨ�� 4
    TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
    TIM_OC4Init(GENERAL_TIM, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(GENERAL_TIM, ENABLE); //ARPEʹ�ܣ��ȴ��´��¼���������Ӱ�ӼĴ���
    TIM_Cmd(GENERAL_TIM, ENABLE);// ʹ�ܼ�����
}

void PWM_Init(void)
{
    PWM_GPIO_Config();
    PWM_Mode_Config();
}

//����ͨ���������Ƚ�ֵ
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

//���ͨ���������Ƚ�ֵ
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


//���ö���Ƕȣ�numΪͨ��
void Motor_SetAngle(u8 num, float angle)
{
    /*if (angle < 0) angle = 0;
    else if (angle > 180) angle = 180;  */

    double duty = ((double)angle) / 18 + 2.5;   //0-180�ȶ�Ӧ2.5-12.5ռ�ձ�
    PWM_SetCCRX(num, duty * DUTY_TO_CCRX_RATIO);
}

//��ö���Ƕ�
float Motor_GetAngle(u8 num)
{
    u32 ccrx = PWM_GetCCRX(num);

    double duty = 1.0 * ccrx / DUTY_TO_CCRX_RATIO;
    float angle = 18 * (duty - 2.5);

    if (angle < 0) angle = 0;
    else if (angle > 180) angle = 180;

    return angle;
}

//��ԭ���������ӻ���ٽǶ�
void Motor_AddToAngle(u8 num, float add_angle)
{
    float angle = Motor_GetAngle(num) + add_angle;

    if (angle < 0) angle = 0;
    else if (angle > 180) angle = 180;

    Motor_SetAngle(num, angle);
}


#define BOUND 10

#define ANGLE_MIN 20    //�����С�Ƕ�
#define ANGLE_MAX 160 //������Ƕ�

float Kp = 0.012, Kd = 0.001;

float last_err = 0;

//���PD����
float update_pid(float setpoint, float real_val)
{
    float out = 0;
    float err = setpoint - real_val;

    if (err < BOUND && err > -BOUND)   //�����ĵ����С��bound�������˶�
    {
        last_err = err;
        return 0;
    }

    out = Kp * err + Kd * (err - last_err);
    last_err = err;
    return out;
}

//�����������
void Motor_Action(void)
{
    if (!find_flag) return;//δʶ��Ŀ�귵��

    float angle;

    angle = g_angle_H + update_pid(160, Resu.x);

    //�޷�
    if (angle < ANGLE_MIN) g_angle_H = ANGLE_MIN;
    else if (angle > ANGLE_MAX) g_angle_H = ANGLE_MAX;
    else g_angle_H = angle;

    //����ˮƽ�������Ƕ�
    Motor_SetAngle(CHx_MOTOR_H, g_angle_H);

    angle = g_angle_V - update_pid(120, Resu.y);

    if (angle < ANGLE_MIN) g_angle_V = ANGLE_MIN;
    else if (angle > ANGLE_MAX) g_angle_V = ANGLE_MAX;
    else g_angle_V = angle;

    //���ô�ֱ�������Ƕ�
    Motor_SetAngle(CHx_MOTOR_V, g_angle_V);

}

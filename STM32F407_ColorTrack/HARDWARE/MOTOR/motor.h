#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f4xx.h"
#include "easytracer/EasyTracer.h"

#define            GENERAL_TIM                   TIM3
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM3
#define            GENERAL_TIM_Period            (20000-1)
#define            GENERAL_TIM_Prescaler         (84-1) //计数频率1MHz，PWM频率50hz

#define                      GENERAL_TIM_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define            GENERAL_TIM_GPIO_AF               GPIO_AF_TIM3
//  输出比较通道1
#define            GENERAL_TIM_CH1_PORT          GPIOA
#define            GENERAL_TIM_CH1_PIN           GPIO_Pin_6
#define            GENERAL_TIM_CH1_PIN_SOURCE    GPIO_PinSource6
//  输出比较通道2
#define            GENERAL_TIM_CH2_PORT          GPIOA
#define            GENERAL_TIM_CH2_PIN           GPIO_Pin_7
#define            GENERAL_TIM_CH2_PIN_SOURCE    GPIO_PinSource7
//  输出比较通道3
#define            GENERAL_TIM_CH3_PORT          GPIOB
#define            GENERAL_TIM_CH3_PIN           GPIO_Pin_0
#define            GENERAL_TIM_CH3_PIN_SOURCE    GPIO_PinSource0
// 输出比较通道4
#define            GENERAL_TIM_CH4_PORT          GPIOB
#define            GENERAL_TIM_CH4_PIN           GPIO_Pin_1
#define            GENERAL_TIM_CH4_PIN_SOURCE    GPIO_PinSource1

#define  CHx_MOTOR_H 3  //水平方向舵机通道3
#define  CHx_MOTOR_V 4  //垂直方向舵机通道4


#define  DUTY_TO_CCRX_RATIO  ((GENERAL_TIM_Period+1)/100)

extern float g_angle_H, g_angle_V;//水平角和垂直角
extern int find_flag;
extern RESULT Resu;

void PWM_Init(void);
void Motor_SetAngle(u8 num, float angle);
void Motor_AddToAngle(u8 num, float add_angle);
void Motor_Action(void);

#endif

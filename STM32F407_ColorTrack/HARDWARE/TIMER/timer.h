#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include "stm32f4xx.h"
//TIM1¡¢8-11 168M (APB2Á½±¶)
//TIM2-7¡¢12-14 84M  (APB1Á½±¶)

//1ms
#define TIM2_PERIOD  (1000-1)
#define TIM2_PSC     (84-1)

//10ms
#define TIM4_PERIOD  (10000-1)
#define TIM4_PSC     (84-1)


extern u8 LCD_refresh_flag;
extern u16 ov_frame;
extern u16 fps;//Ö¡ÂÊ

void Time2_init(void);
void Time4_init(void);
void send_message(void);

#endif

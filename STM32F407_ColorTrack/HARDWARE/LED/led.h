#ifndef __LED_H
#define __LED_H
#include "sys.h"

//LED端口定义
#define LED0 PFout(9)   // DS0
#define LED1 PFout(10)  // DS1   

void LED_Init(void);//初始化
void LED_on(u8 num);
void LED_off(u8 num);
void LED_chg(u8 num);

#endif

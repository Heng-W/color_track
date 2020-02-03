#ifndef _KEY_H
#define _KEY_H

#include "headfile.h"

#define    KEY1_GPIO_CLK     RCC_AHB1Periph_GPIOA
#define    KEY1_GPIO_PORT    GPIOA
#define    KEY1_GPIO_PIN     GPIO_Pin_0

//#define    KEY2_GPIO_CLK     RCC_AHB1Periph_GPIOC
//#define    KEY2_GPIO_PORT    GPIOC
//#define    KEY2_GPIO_PIN     GPIO_Pin_13

#define    key1_val          0x01
#define    key2_val          0x02

#define    KEY_INTERVAL   30
#define    KEY_LONG_TIME  100


typedef enum
{
    KEY_DOWN = 0,
    KEY_UP = 1,
    KEY_HOLD,
    KEY_LONG,
    KEY_AUTO
} KeyState;


void key_init(void);
void key_scan(void);
void key_dispose(void);


#endif

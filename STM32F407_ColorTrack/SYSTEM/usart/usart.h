#ifndef __USART_H
#define __USART_H
#include <stdio.h>
#include "stm32f4xx_conf.h"
#include "sys.h"
#include "EasyTracer/EasyTracer.h"
#include "Color/color.h"
//////////////////////////////////////////////////////////////////////////////////


// 串口1-USART1
#define  USARTx                   USART1
#define  USART_CLK                RCC_APB2Periph_USART1
#define  USART_APBxClkCmd         RCC_APB2PeriphClockCmd

// USART GPIO 引脚宏定义
#define  USART_GPIO_CLK           (RCC_AHB1Periph_GPIOA)
#define  USART_GPIO_APBxClkCmd    RCC_AHB1PeriphClockCmd

#define  USART_TX_GPIO_PORT         GPIOA
#define  USART_TX_GPIO_PIN          GPIO_Pin_9
#define  USART_TX_GPIO_PIN_SOURCE       GPIO_PinSource9
#define  USART_RX_GPIO_PORT       GPIOA
#define  USART_RX_GPIO_PIN        GPIO_Pin_10
#define  USART_RX_GPIO_PIN_SOURCE       GPIO_PinSource10
#define  USART_GPIO_AF_USARTx           GPIO_AF_USART1

#define  USART_IRQ                USART1_IRQn
#define  USART_IRQHandler         USART1_IRQHandler


#define USART_REC_LEN           100     //定义最大接收字节数 
#define EN_USARTx_RX            1       //使能（1）/禁止（0）串口1接收


//指令类型
typedef enum
{
    start, stop, up, down, left, right, setsize, pick,
    setcolor, defcolor, start_monitor, stop_monitor,
    camera_reset, reset, exit, init, message
} Command;


extern u8 color_idx, start_flag;
extern u8 free_def_flag;
extern TARGET_CONDI condi_set;

void uart_init(u32 bound);
void usart_dispose(void);
void pick_avg(void);
void cal_condi(COLOR_HSL* hsl);


#endif



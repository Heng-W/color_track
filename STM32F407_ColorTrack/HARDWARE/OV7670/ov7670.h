#ifndef _OV7670_H
#define _OV7670_H
#include "sys.h"
#include "sccb.h"

//////////////////////////////////////////////////////////////////////////////////

#define Cmos_RCC_CLK  (RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE)

#define Cmos_VSYNC_PORT GPIOA
#define Cmos_VSYNC_PIN GPIO_Pin_4


#define FIFO_PORT GPIOB
#define FIFO_PIN  0x7c00

#define OV7670_WREN    PBout(11)    //д��FIFOʹ��
#define OV7670_WRST  PBout(13)  //дָ�븴λ
#define OV7670_RCK_H    GPIOB->BSRRL=1<<10//���ö�����ʱ�Ӹߵ�ƽ
#define OV7670_RCK_L GPIOB->BSRRH=1<<10 //���ö�����ʱ�ӵ͵�ƽ
#define OV7670_RRST  PBout(14)  //��ָ�븴λ
#define OV7670_CS      PBout(12)    //Ƭѡ�ź�(CS)

#define OV7670_DATA_PORT GPIOE
#define OV7670_DATA_PIN 0x7f80  //PE7-14

#define OV7670_DATA   ((OV7670_DATA_PORT->IDR & OV7670_DATA_PIN)>>7)


/////////////////////////////////////////


u8   OV7670_Init(void);
void OV7670_Light_Mode(u8 mode);
void OV7670_Color_Saturation(s8 sat);
void OV7670_Brightness(s8 bright);
void OV7670_Contrast(s8 contrast);
void OV7670_Special_Effects(u8 eft);
void OV7670_Window_Set(u16 sx, u16 sy, u16 width, u16 height);

#endif

#ifndef __SCCB_H
#define __SCCB_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////

#define SCCB_RCC_CLK RCC_AHB1Periph_GPIOC

#define SCCB_PORT GPIOC
#define SCCB_SCL_PIN GPIO_Pin_2
#define SCCB_SDA_PIN GPIO_Pin_3

#define SCCB_SDA_IN()  {GPIOC->MODER &= 0xFFFFFF3F;}
#define SCCB_SDA_OUT() {GPIOC->MODER &= 0xFFFFFF3F;GPIOC->MODER |= (u32)1<<6;}//GPIOB->OTYPER &= 0xffffff7f;}//通用推挽输出


//IO操作函数
#define SCCB_SCL            PCout(2)        //SCL
#define SCCB_SDA            PCout(3)        //SDA    

#define SCCB_READ_SDA       PCin(3)         //输入SDA    
#define SCCB_ID             0X42            //OV7670的ID

///////////////////////////////////////////
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
u8 SCCB_WR_Byte(u8 dat);
u8 SCCB_RD_Byte(void);
u8 SCCB_WR_Reg(u8 reg, u8 data);
u8 SCCB_RD_Reg(u8 reg);

#endif

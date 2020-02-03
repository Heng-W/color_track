#ifndef __EXTI_H
#define __EXTI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////

#define TRACK_W 240
#define TRACK_H 240

#define DEFAULT_W 30
#define DEFAULT_H 30


extern u8 ov_sta;//帧中断标记
extern u16 ov_frame; //帧率
extern u16 lcd_data[TRACK_H][TRACK_W];


void EXTI4_Init(void);
void set_size(unsigned int w, unsigned int h);

#endif

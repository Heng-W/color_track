#ifndef __GUI_H
#define __GUI_H

#include "stm32f4xx.h"

u16 LCD_BGR2RGB(u16 c);
void Gui_Circle(u16 X, u16 Y, u16 R, u16 fc);
void Gui_DrawLine(u16 x0, u16 y0, u16 x1, u16 y1, u16 Color);
void Gui_box(u16 x, u16 y, u16 w, u16 h, u16 bc);
void Gui_box2(u16 x, u16 y, u16 w, u16 h, u8 mode);
void DisplayButtonDown(u16 x1, u16 y1, u16 x2, u16 y2);
void DisplayButtonUp(u16 x1, u16 y1, u16 x2, u16 y2);
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8* s);
void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8* s);
void Gui_DrawFont_GBK32(u16 x, u16 y, u16 fc, u16 bc, u8* s);
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num);
void Gui_DrawFont_GBK64(u16 x, u16 y, u16 fc, u16 bc, u8* s);
void show_pic(const unsigned char* p);
//void Time_Disp_Main(struct rtc_time *tm,u16 background_light);
void Gui_DrawBluetooth_Logo(u16 x, u16 y, u16 fc, u16 bc);
void Gui_DrawBattery_Logo(u16 x, u16 y, u16 fc, u16 bc, u8 valu);
void disp_logo44(u16 x, u16 y, u16 fc, u16 bc, u8 valu);
void Painting_Form(uint8_t x, uint8_t y, uint8_t n);
//void Time_read(struct rtc_time *tm,struct data *save_data);
void disp_Battery(void);
#endif


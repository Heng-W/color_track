#ifndef __COLOR_H
#define __COLOR_H

#include "sys.h"


typedef struct
{
    u8 red, green, blue;
} COLOR_RGB;


typedef struct
{
    u8 hue, saturation, luminance;
} COLOR_HSL;


void get_rgb(u16 dat, COLOR_RGB* rgb);
void rgb_to_hsl(COLOR_RGB* rgb, COLOR_HSL* hsl);

#endif

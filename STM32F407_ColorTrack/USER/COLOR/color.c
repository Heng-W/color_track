#include "Color/color.h"

//RGB565格式，转RGB数值
void get_rgb(u16 dat, COLOR_RGB* rgb)
{
    rgb->red = (u8)((dat & 0xf800) >> 8); //5bit, 0-248(8)
    rgb->green = (u8)((dat & 0x07e0) >> 3); //6bit, 0-252(4)
    rgb->blue = (u8)((dat & 0x001f) << 3); //5bit, 0-248(8)
}


#define min3v(v1, v2, v3)   ((v1)>(v2)? ((v2)>(v3)?(v3):(v2)):((v1)>(v3)?(v3):(v1)))//比较最小值 
#define max3v(v1, v2, v3)   ((v1)<(v2)? ((v2)<(v3)?(v3):(v2)):((v1)<(v3)?(v3):(v1)))//比较最大值

//RGB转HSL
void rgb_to_hsl(COLOR_RGB* rgb, COLOR_HSL* hsl)
{
    int maxVal, minVal, difVal;
    int r = rgb->red;
    int g = rgb->green;
    int b = rgb->blue;
    int h, s2, l;
    maxVal = max3v(r, g, b);
    minVal = min3v(r, g, b);
    difVal = maxVal - minVal;
    //计算亮度
    l = (maxVal + minVal) * 240 / 255 / 2;

    if (maxVal == minVal)   //若r=g=b
    {
        h = 0;
        s2 = 0;
    }
    else
    {
        //计算色调
        if (maxVal == r)
        {
            if (g >= b)
            {
                h = 40 * (g - b) / (difVal);
            }
            else
            {
                h = 40 * (g - b) / (difVal) + 240;
            }
        }
        else if (maxVal == g)
        {
            h = 40 * (b - r) / (difVal) + 80;
        }
        else if (maxVal == b)
        {
            h = 40 * (r - g) / (difVal) + 160;
        }

        //计算饱和度
        if (l == 0)
        {
            s2 = 0;
        }
        else if (l <= 120)
        {
            s2 = (difVal) * 240 / (maxVal + minVal);
        }
        else
        {
            s2 = (difVal) * 240 / (511 - (maxVal + minVal));
        }
    }

    hsl->hue = (u8)(((h > 240) ? 240 : ((h < 0) ? 0 : h)));
    hsl->saturation = (u8)(((s2 > 240) ? 240 : ((s2 < 0) ? 0 : s2)));
    hsl->luminance = (u8)(((l > 240) ? 240 : ((l < 0) ? 0 : l)));
}

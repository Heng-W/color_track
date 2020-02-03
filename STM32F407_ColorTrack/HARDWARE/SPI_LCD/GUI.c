/**
******************************************************************************
* @file    GUI.c
* @author  wangyongqing
* @version V1.0
* @date    2015-08-08
* @brief
******************************************************************************
***/
#include "SPI_LCD/GUI.h"
#include "SPI_LCD/Lcd_Driver.h"
#include "SPI_LCD/font.h"
/****************************************************************************
* 名    称：u16 LCD_BGR2RGB(u16 c)
* 功    能：从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式，通过该函数转换
* 入口参数：c->GBR格式的颜色值
* 出口参数：RGB格式的颜色值
* 说    明：
****************************************************************************/
u16 LCD_BGR2RGB(u16 c)
{
    u16  r, g, b, rgb;
    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);
    return (rgb);
}

/****************************************************************************
* 名    称：void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc)
* 功    能：在屏幕上画圆
* 入口参数：x,y->圆的坐标，R->圆的半径,fc->颜色比如：红色为0xf800
* 出口参数：无
* 说    明：
****************************************************************************/
void Gui_Circle(u16 X, u16 Y, u16 R, u16 fc)
{
    //Bresenham算法
    unsigned short  a, b;
    int c;
    a = 0;
    b = R;
    c = 3 - 2 * R;

    while (a < b)
    {
        Gui_DrawPoint(X + a, Y + b, fc); //        7
        Gui_DrawPoint(X - a, Y + b, fc); //        6
        Gui_DrawPoint(X + a, Y - b, fc); //        2
        Gui_DrawPoint(X - a, Y - b, fc); //        3
        Gui_DrawPoint(X + b, Y + a, fc); //        8
        Gui_DrawPoint(X - b, Y + a, fc); //        5
        Gui_DrawPoint(X + b, Y - a, fc); //        1
        Gui_DrawPoint(X - b, Y - a, fc); //        4

        if (c < 0) c = c + 4 * a + 6;
        else
        {
            c = c + 4 * (a - b) + 10;
            b -= 1;
        }

        a += 1;
    }

    if (a == b)
    {
        Gui_DrawPoint(X + a, Y + b, fc);
        Gui_DrawPoint(X + a, Y + b, fc);
        Gui_DrawPoint(X + a, Y - b, fc);
        Gui_DrawPoint(X - a, Y - b, fc);
        Gui_DrawPoint(X + b, Y + a, fc);
        Gui_DrawPoint(X - b, Y + a, fc);
        Gui_DrawPoint(X + b, Y - a, fc);
        Gui_DrawPoint(X - b, Y - a, fc);
    }
}

/****************************************************************************
* 名    称：void Gui_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color)
* 功    能：画线函数，使用Bresenham 画线算法
* 入口参数：x0,y0->线起点坐标;x1,y1->线终点坐标;Color->线的颜色
* 出口参数：无
* 说    明：
****************************************************************************/
void Gui_DrawLine(u16 x0, u16 y0, u16 x1, u16 y1, u16 Color)
{
    int dx,         // difference in x's
        dy,             // difference in y's
        dx2,            // dx,dy * 2
        dy2,
        x_inc,          // amount in pixel space to move during drawing
        y_inc,          // amount in pixel space to move during drawing
        error,          // the discriminant i.e. error i.e. decision variable
        index;          // used for looping

    Lcd_SetXY(x0, y0);
    dx = x1 - x0; //计算x距离
    dy = y1 - y0; //计算y距离

    if (dx >= 0)
    {
        x_inc = 1;
    }
    else
    {
        x_inc = -1;
        dx    = -dx;
    }

    if (dy >= 0)
    {
        y_inc = 1;
    }
    else
    {
        y_inc = -1;
        dy    = -dy;
    }

    dx2 = dx << 1;
    dy2 = dy << 1;

    if (dx > dy)   //x距离大于y距离，那么每个x轴上只有一个点，每个y轴上有若干个点
    {
        //且线的点数等于x距离，以x轴递增画点
        // initialize error term
        error = dy2 - dx;

        // draw the line
        for (index = 0; index <= dx; index++)   //要画的点数不会超过x距离
        {
            //画点
            Gui_DrawPoint(x0, y0, Color);

            // test if error has overflowed
            if (error >= 0)   //是否需要增加y坐标值
            {
                error -= dx2;

                // move to next line
                y0 += y_inc; //增加y坐标值
            } // end if error overflowed

            // adjust the error term
            error += dy2;

            // move to the next pixel
            x0 += x_inc; //x坐标值每次画点后都递增1
        } // end for
    } // end if |slope| <= 1
    else   //y轴大于x轴，则每个y轴上只有一个点，x轴若干个点
    {
        //以y轴为递增画点
        // initialize error term
        error = dx2 - dy;

        // draw the line
        for (index = 0; index <= dy; index++)
        {
            // set the pixel
            Gui_DrawPoint(x0, y0, Color);

            // test if error overflowed
            if (error >= 0)
            {
                error -= dy2;

                // move to next line
                x0 += x_inc;
            } // end if error overflowed

            // adjust the error term
            error += dx2;

            // move to the next pixel
            y0 += y_inc;
        } // end for
    } // end else |slope| > 1
}


/****************************************************************************
* 名    称：void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc)
* 功    能：画矩形
* 入口参数：x,y->矩形左上角坐标;w,h->矩形的宽和高;bc->颜色
* 出口参数：无
* 说    明：
****************************************************************************/
void Gui_box(u16 x, u16 y, u16 w, u16 h, u16 bc)
{
    Gui_DrawLine(x, y, x + w, y, bc);
    Gui_DrawLine(x + w, y, x + w, y + h, bc);
    Gui_DrawLine(x, y + h, x + w, y + h, bc);
    Gui_DrawLine(x, y, x, y + h, bc);
    //Gui_DrawLine(x,y,x+w,y+h,bc);
}

/****************************************************************************
* 名    称：void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode)
* 功    能：画矩形
* 入口参数：x,y->矩形左上角坐标;w,h->矩形的宽和高;mode->0、1、2不同的模式画的效果不一样
* 出口参数：无
* 说    明：
****************************************************************************/
void Gui_box2(u16 x, u16 y, u16 w, u16 h, u8 mode)
{
    if (mode == 0)
    {
        Gui_DrawLine(x, y, x + w, y, 0xEF7D);
        Gui_DrawLine(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0x2965);
        Gui_DrawLine(x, y + h, x + w, y + h, 0x2965);
        Gui_DrawLine(x, y, x, y + h, 0xEF7D);
    }

    if (mode == 1)
    {
        Gui_DrawLine(x, y, x + w, y, 0x2965);
        Gui_DrawLine(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0xEF7D);
        Gui_DrawLine(x, y + h, x + w, y + h, 0xEF7D);
        Gui_DrawLine(x, y, x, y + h, 0x2965);
    }

    if (mode == 2)
    {
        Gui_DrawLine(x, y, x + w, y, 0xffff);
        Gui_DrawLine(x + w - 1, y + 1, x + w - 1, y + 1 + h, 0xffff);
        Gui_DrawLine(x, y + h, x + w, y + h, 0xffff);
        Gui_DrawLine(x, y, x, y + h, 0xffff);
    }
}

/****************************************************************************
* 名    称：void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2)
* 功    能：在屏幕显示一凸起的按钮框
* 入口参数：x1,y1->矩形左上角坐标;x2,y2->矩形右下角坐标;
* 出口参数：无
* 说    明：
****************************************************************************/
void DisplayButtonDown(u16 x1, u16 y1, u16 x2, u16 y2)
{
    Gui_DrawLine(x1,  y1,  x2, y1, GRAY2);   //H
    Gui_DrawLine(x1 + 1, y1 + 1, x2, y1 + 1, GRAY1); //H
    Gui_DrawLine(x1,  y1,  x1, y2, GRAY2);   //V
    Gui_DrawLine(x1 + 1, y1 + 1, x1 + 1, y2, GRAY1); //V
    Gui_DrawLine(x1,  y2,  x2, y2, WHITE);   //H
    Gui_DrawLine(x2,  y1,  x2, y2, WHITE);   //V
}

/****************************************************************************
* 名    称：void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2)
* 功    能：在屏幕显示一凹下的按钮框
* 入口参数：x1,y1->矩形左上角坐标;x2,y2->矩形右下角坐标;
* 出口参数：无
* 说    明：
****************************************************************************/
void DisplayButtonUp(u16 x1, u16 y1, u16 x2, u16 y2)
{
    Gui_DrawLine(x1,  y1,  x2, y1, WHITE);  //H
    Gui_DrawLine(x1,  y1,  x1, y2, WHITE);  //V
    Gui_DrawLine(x1 + 1, y2 - 1, x2, y2 - 1, GRAY1); //H
    Gui_DrawLine(x1,  y2,  x2, y2, GRAY2);  //H
    Gui_DrawLine(x2 - 1, y1 + 1, x2 - 1, y2, GRAY1); //V
    Gui_DrawLine(x2, y1, x2, y2, GRAY2);    //V
}

/****************************************************************************
* 名    称：void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
* 功    能：按16*16点阵的字体显示文字，或者8*16的字符
* 入口参数：x,y->矩形左上角坐标;fc->字体颜色;bc->字体的背景颜色;s->字符串指针
* 出口参数：无
* 说    明：这个函数真是用于16*16的点阵字体或者8*16的字符显示，实例：
*           Gui_DrawFont_GBK16(16,0,BLUE,GRAY0,"全动电子技术");
****************************************************************************/
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8* s)
{
    unsigned char i, j;
    unsigned short k, x0;
    x0 = x;

    while (*s)
    {
        if ((*s) < 128)
        {
            k = *s;

            if (k == 13)
            {
                x = x0;
                y += 16;
            }
            else
            {
                if (k > 32) k -= 32;
                else k = 0;

                for (i = 0; i < 16; i++)
                {
                    for (j = 0; j < 8; j++)
                    {
                        if (asc16[k * 16 + i] & (0x80 >> j))
                        {
                            Gui_DrawPoint(x + j, y + i, fc);
                        }
                        else
                        {
                            if (fc != bc) Gui_DrawPoint(x + j, y + i, bc);
                        }
                    }
                }

                x += 8;
            }

            s++;
        }
        else
        {
            for (k = 0; k < hz16_num; k++)
            {
                if ((hz16[k].Index[0] == *(s)) && (hz16[k].Index[1] == *(s + 1)))
                {
                    for (i = 0; i < 16; i++)
                    {
                        for (j = 0; j < 8; j++)
                        {
                            if (hz16[k].Msk[i * 2] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j, y + i, bc);
                            }
                        }

                        for (j = 0; j < 8; j++)
                        {
                            if (hz16[k].Msk[i * 2 + 1] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j + 8, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j + 8, y + i, bc);
                            }
                        }
                    }
                }
            }

            s += 2;
            x += 16;
        }
    }
}

/****************************************************************************
* 名    称：void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
* 功    能：按24*24点阵的字体显示文字,按12*24点阵显示字符
* 入口参数：x,y->矩形左上角坐标;fc->字体颜色;bc->字体的背景颜色;s->字符串指针
* 出口参数：无
* 说    明：这个函数真是用于24*24的点阵字体显示，实例：
*           Gui_DrawFont_GBK24(x_offset,50,BLUE,WHITE, "     欢迎您 ");
*           Gui_DrawFont_GBK24(x_offset,50,BLUE,WHITE, "16:22");
****************************************************************************/
void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8* s)
{
    unsigned char i, j;
    unsigned short k;

    while (*s)
    {
        if (*s < 0x80)
        {
            k = *s;

            if (k > 32) k -= 32;
            else k = 0;

            for (i = 0; i < 24; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (asc1224[k * 48 + i * 2] & (0x80 >> j))
                        Gui_DrawPoint(x + j, y + i, fc);
                    else
                    {
                        if (fc != bc) Gui_DrawPoint(x + j, y + i, bc);
                    }
                }

                for (j = 0; j < 4; j++)
                {
                    if (asc1224[k * 48 + i * 2 + 1] & (0x80 >> j))
                        Gui_DrawPoint(x + j + 8, y + i, fc);
                    else
                    {
                        if (fc != bc) Gui_DrawPoint(x + j + 8, y + i, bc);
                    }
                }
            }

            s++;
            x += 12;
        }
        else
        {
            for (k = 0; k < hz24_num; k++)
            {
                if ((hz24[k].Index[0] == *(s)) && (hz24[k].Index[1] == *(s + 1)))
                {
                    for (i = 0; i < 24; i++)
                    {
                        for (j = 0; j < 8; j++)
                        {
                            if (hz24[k].Msk[i * 3] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j, y + i, bc);
                            }
                        }

                        for (j = 0; j < 8; j++)
                        {
                            if (hz24[k].Msk[i * 3 + 1] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j + 8, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j + 8, y + i, bc);
                            }
                        }

                        for (j = 0; j < 8; j++)
                        {
                            if (hz24[k].Msk[i * 3 + 2] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j + 16, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j + 16, y + i, bc);
                            }
                        }
                    }
                }
            }

            s += 2;
            x += 24;
        }
    }
}

/****************************************************************************
* 名    称：void Gui_DrawFont_GBK32(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
* 功    能：按32*32点阵的字体显示文字,按16*32点阵显示字符
* 入口参数：x,y->矩形左上角坐标;fc->字体颜色;bc->字体的背景颜色;s->字符串指针
* 出口参数：无
* 说    明：这个函数真是用于32*32的点阵字体显示，实例：
*           Gui_DrawFont_GBK32(x_offset,50,BLUE,WHITE, "Hello world");
****************************************************************************/
void Gui_DrawFont_GBK32(u16 x, u16 y, u16 fc, u16 bc, u8* s)
{
    unsigned char i, j;
    unsigned short k;

    while (*s)
    {
        if (*s < 0x80)
        {
            k = *s;

            if (k > 32) k -= 32;
            else k = 0;

            for (i = 0; i < 32; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (asc1632[k * 64 + i * 2] & (0x80 >> j))
                        Gui_DrawPoint(x + j, y + i, fc);
                    else
                    {
                        if (fc != bc) Gui_DrawPoint(x + j, y + i, bc);
                    }
                }

                for (j = 0; j < 8; j++)
                {
                    if (asc1632[k * 64 + i * 2 + 1] & (0x80 >> j))
                        Gui_DrawPoint(x + j + 8, y + i, fc);
                    else
                    {
                        if (fc != bc) Gui_DrawPoint(x + j + 8, y + i, bc);
                    }
                }
            }

            s++;
            x += 16;
        }
        else
        {
            for (k = 0; k < hz32_num; k++)
            {
                if ((hz32[k].Index[0] == *(s)) && (hz32[k].Index[1] == *(s + 1)))
                {
                    for (i = 0; i < 32; i++)
                    {
                        for (j = 0; j < 8; j++)
                        {
                            if (hz32[k].Msk[i * 4] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j, y + i, bc);
                            }
                        }

                        for (j = 0; j < 8; j++)
                        {
                            if (hz32[k].Msk[i * 4 + 1] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j + 8, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j + 8, y + i, bc);
                            }
                        }

                        for (j = 0; j < 8; j++)
                        {
                            if (hz32[k].Msk[i * 4 + 2] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j + 16, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j + 16, y + i, bc);
                            }
                        }

                        for (j = 0; j < 8; j++)
                        {
                            if (hz32[k].Msk[i * 4 + 3] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j + 24, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j + 24, y + i, bc);
                            }
                        }
                    }
                }
            }

            s += 2;
            x += 32;
        }
    }
}

/****************************************************************************
* 名    称：void Gui_DrawFont_GBK40(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
* 功    能：按40*40点阵的字体显示文字,按20*40点阵显示字符
* 入口参数：x,y->矩形左上角坐标;fc->字体颜色;bc->字体的背景颜色;s->字符串指针
* 出口参数：无
* 说    明：这个函数真是用于40*40的点阵字体显示，实例：
*           Gui_DrawFont_GBK40(x_offset,50,BLUE,WHITE, "Hello world");
****************************************************************************/
void Gui_DrawFont_GBK40(u16 x, u16 y, u16 fc, u16 bc, u8* s)
{
    unsigned char i, j;
    unsigned short k;

    while (*s)
    {
        if (*s < 0x80)
        {
            k = *s;

            if (k > 32) k -= 32;
            else k = 0;

            for (i = 0; i < 32; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (asc1632[k * 64 + i * 2] & (0x80 >> j))
                        Gui_DrawPoint(x + j, y + i, fc);
                    else
                    {
                        if (fc != bc) Gui_DrawPoint(x + j, y + i, bc);
                    }
                }

                for (j = 0; j < 8; j++)
                {
                    if (asc1632[k * 64 + i * 2 + 1] & (0x80 >> j))
                        Gui_DrawPoint(x + j + 8, y + i, fc);
                    else
                    {
                        if (fc != bc) Gui_DrawPoint(x + j + 8, y + i, bc);
                    }
                }
            }

            s++;
            x += 16;
        }
        else
        {
            for (k = 0; k < hz24_num; k++)
            {
                if ((hz24[k].Index[0] == *(s)) && (hz24[k].Index[1] == *(s + 1)))
                {
                    for (i = 0; i < 24; i++)
                    {
                        for (j = 0; j < 8; j++)
                        {
                            if (hz24[k].Msk[i * 3] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j, y + i, bc);
                            }
                        }

                        for (j = 0; j < 8; j++)
                        {
                            if (hz24[k].Msk[i * 3 + 1] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j + 8, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j + 8, y + i, bc);
                            }
                        }

                        for (j = 0; j < 8; j++)
                        {
                            if (hz24[k].Msk[i * 3 + 2] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j + 16, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j + 16, y + i, bc);
                            }
                        }
                    }
                }
            }

            s += 2;
            x += 24;
        }
    }
}

/****************************************************************************
* 名    称：void Gui_DrawFont_GBK32(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
* 功    能：按64*64点阵的字体显示文字,按32*64点阵显示字符
* 入口参数：x,y->矩形左上角坐标;fc->字体颜色;bc->字体的背景颜色;s->字符串指针
* 出口参数：无
* 说    明：这个函数真是用于64*64的点阵字体显示，实例：
*           Gui_DrawFont_GBK32(x_offset,50,BLUE,WHITE, "Hello world");
****************************************************************************/
void Gui_DrawFont_GBK64(u16 x, u16 y, u16 fc, u16 bc, u8* s)
{
    unsigned char i, j;
    unsigned short k;

    while (*s)
    {
        if (*s < 0x80)
        {
            k = *s;

            if (k == 0x25)   //在界面中%号要比较小一点
            {
                Gui_DrawFont_GBK32(x, y + 27, fc, bc, "%");
            }
            else
            {
                if (k > 32) k -= 32;
                else k = 0;

                for (i = 0; i < 64; i++)
                {
                    for (j = 0; j < 8; j++)
                    {
                        if (asc3264[k * 256 + i * 4] & (0x80 >> j))
                            Gui_DrawPoint(x + j, y + i, fc);
                        else
                        {
                            if (fc != bc) Gui_DrawPoint(x + j, y + i, bc);
                        }
                    }

                    for (j = 0; j < 8; j++)
                    {
                        if (asc3264[k * 256 + i * 4 + 1] & (0x80 >> j))
                            Gui_DrawPoint(x + j + 8, y + i, fc);
                        else
                        {
                            if (fc != bc) Gui_DrawPoint(x + j + 8, y + i, bc);
                        }
                    }

                    for (j = 0; j < 8; j++)
                    {
                        if (asc3264[k * 256 + i * 4 + 2] & (0x80 >> j))
                            Gui_DrawPoint(x + j + 16, y + i, fc);
                        else
                        {
                            if (fc != bc) Gui_DrawPoint(x + j + 16, y + i, bc);
                        }
                    }

                    for (j = 0; j < 8; j++)
                    {
                        if (asc3264[k * 256 + i * 4 + 3] & (0x80 >> j))
                            Gui_DrawPoint(x + j + 24, y + i, fc);
                        else
                        {
                            if (fc != bc) Gui_DrawPoint(x + j + 24, y + i, bc);
                        }
                    }
                }
            }

            s++;
            x += 32;
        }
        else
        {
            for (k = 0; k < hz24_num; k++)
            {
                if ((hz24[k].Index[0] == *(s)) && (hz24[k].Index[1] == *(s + 1)))
                {
                    for (i = 0; i < 24; i++)
                    {
                        for (j = 0; j < 8; j++)
                        {
                            if (hz24[k].Msk[i * 3] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j, y + i, bc);
                            }
                        }

                        for (j = 0; j < 8; j++)
                        {
                            if (hz24[k].Msk[i * 3 + 1] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j + 8, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j + 8, y + i, bc);
                            }
                        }

                        for (j = 0; j < 8; j++)
                        {
                            if (hz24[k].Msk[i * 3 + 2] & (0x80 >> j))
                            {
                                Gui_DrawPoint(x + j + 16, y + i, fc);
                            }
                            else
                            {
                                if (fc != bc) Gui_DrawPoint(x + j + 16, y + i, bc);
                            }
                        }
                    }
                }
            }

            s += 2;
            x += 24;
        }
    }
}
/****************************************************************************
* 名    称：void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num)
* 功    能：按32*32点阵的字体显示数字
* 入口参数：x,y->矩形左上角坐标;fc->字体颜色;bc->字体的背景颜色;num->显示的数字
* 出口参数：无
* 说    明：这个函数只能用于32*32的点阵字体显示，实例：
*           Gui_DrawFont_Num32(0,0,RED,GRAY0,2);
****************************************************************************/
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num)
{
    unsigned char i, j, k, c;

    for (i = 0; i < 32; i++)
    {
        for (j = 0; j < 4; j++)
        {
            c = *(sz32 + num * 32 * 4 + i * 4 + j);

            for (k = 0; k < 8; k++)
            {
                if (c & (0x80 >> k))
                {
                    Gui_DrawPoint(x + j * 8 + k, y + i, fc);
                }
                else
                {
                    if (fc != bc) Gui_DrawPoint(x + j * 8 + k, y + i, bc);
                }
            }
        }
    }
}

/****************************************************************************
* 名    称：void show_pic(const unsigned char *p)
* 功    能：显示240*320的图片
* 入口参数：p->存放图片取模到的数组指针
* 出口参数：无
* 说    明：这里显示的图片的尺寸一定要是240*320，取模的时候16位 垂直扫描  右到左  高位在前
****************************************************************************/
void show_pic(const unsigned char* p)
{
    int i, j;
    unsigned char picH, picL;

    for (i = 0; i < 240; i++)
    {
        for (j = 0; j < 320; j++)
        {
            picH = *p++;
            picL = *p++;
            Lcd_WriteData(picH);
            Lcd_WriteData(picL);
        }
    }
}

/****************************************************************************
* 名    称：void Gui_DrawBluetooth_Logo(u16 x, u16 y, u16 fc, u16 bc)
* 功    能：按16*24点阵的蓝牙图标logo
* 入口参数：x,y->矩形左上角坐标;fc->字体颜色;bc->字体的背景颜色
* 出口参数：无
* 说    明：
****************************************************************************/
void Gui_DrawBluetooth_Logo(u16 x, u16 y, u16 fc, u16 bc)
{
    unsigned char i, j;

    for (i = 0; i < 24; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (bluetooth[i * 2] & (0x80 >> j))
                Gui_DrawPoint(x + j, y + i, fc);
            else
            {
                if (fc != bc) Gui_DrawPoint(x + j, y + i, bc);
            }
        }

        for (j = 0; j < 8; j++)
        {
            if (bluetooth[i * 2 + 1] & (0x80 >> j))
                Gui_DrawPoint(x + j + 8, y + i, fc);
            else
            {
                if (fc != bc) Gui_DrawPoint(x + j + 8, y + i, bc);
            }
        }
    }
}

/****************************************************************************
* 名    称：void Gui_DrawBattery_Logo(u16 x, u16 y, u16 fc, u16 bc, u8 valu)
* 功    能：按16*24点阵的蓝牙图标logo
* 入口参数：x,y->矩形左上角坐标;fc->字体颜色;bc->字体的背景颜色
* 出口参数：无
* 说    明：
****************************************************************************/
void Gui_DrawBattery_Logo(u16 x, u16 y, u16 fc, u16 bc, u8 valu)
{
    unsigned char i, j;

    for (i = 0; i < 24; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (battery[valu * 120 + i * 5] & (0x80 >> j))
                Gui_DrawPoint(x + j, y + i, fc);
            else
            {
                if (fc != bc) Gui_DrawPoint(x + j, y + i, bc);
            }
        }

        for (j = 0; j < 8; j++)
        {
            if (battery[valu * 120 + i * 5 + 1] & (0x80 >> j))
                Gui_DrawPoint(x + j + 8, y + i, fc);
            else
            {
                if (fc != bc) Gui_DrawPoint(x + j + 8, y + i, bc);
            }
        }

        for (j = 0; j < 8; j++)
        {
            if (battery[valu * 120 + i * 5 + 2] & (0x80 >> j))
                Gui_DrawPoint(x + j + 16, y + i, fc);
            else
            {
                if (fc != bc) Gui_DrawPoint(x + j + 16, y + i, bc);
            }
        }

        for (j = 0; j < 8; j++)
        {
            if (battery[valu * 120 + i * 5 + 3] & (0x80 >> j))
                Gui_DrawPoint(x + j + 24, y + i, fc);
            else
            {
                if (fc != bc) Gui_DrawPoint(x + j + 24, y + i, bc);
            }
        }

        for (j = 0; j < 8; j++)
        {
            if (battery[valu * 120 + i * 5 + 4] & (0x80 >> j))
                Gui_DrawPoint(x + j + 32, y + i, fc);
            else
            {
                if (fc != bc) Gui_DrawPoint(x + j + 32, y + i, bc);
            }
        }
    }
}

/****************************************************************************
* 名    称：void Gui_Draw44logo(u16 x, u16 y, u16 fc, u16 bc, u8 valu)
* 功    能：按44*44点阵的表情图标logo
* 入口参数：x,y->矩形左上角坐标;fc->字体颜色;bc->字体的背景颜色
* 出口参数：无
* 说    明：
****************************************************************************/
void disp_logo44(u16 x, u16 y, u16 fc, u16 bc, u8 valu)
{
    unsigned char i, j;

    for (i = 0; i < 40; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (logo44[valu * 200 + i * 5] & (0x80 >> j))
                Gui_DrawPoint(x + j, y + i, fc);
            else
            {
                if (fc != bc) Gui_DrawPoint(x + j, y + i, bc);
            }
        }

        for (j = 0; j < 8; j++)
        {
            if (logo44[valu * 200 + i * 5 + 1] & (0x80 >> j))
                Gui_DrawPoint(x + j + 8, y + i, fc);
            else
            {
                if (fc != bc) Gui_DrawPoint(x + j + 8, y + i, bc);
            }
        }

        for (j = 0; j < 8; j++)
        {
            if (logo44[valu * 200 + i * 5 + 2] & (0x80 >> j))
                Gui_DrawPoint(x + j + 16, y + i, fc);
            else
            {
                if (fc != bc) Gui_DrawPoint(x + j + 16, y + i, bc);
            }
        }

        for (j = 0; j < 8; j++)
        {
            if (logo44[valu * 200 + i * 5 + 3] & (0x80 >> j))
                Gui_DrawPoint(x + j + 24, y + i, fc);
            else
            {
                if (fc != bc) Gui_DrawPoint(x + j + 24, y + i, bc);
            }
        }

        for (j = 0; j < 8; j++)
        {
            if (logo44[valu * 200 + i * 5 + 4] & (0x80 >> j))
                Gui_DrawPoint(x + j + 32, y + i, fc);
            else
            {
                if (fc != bc) Gui_DrawPoint(x + j + 32, y + i, bc);
            }
        }
    }
}



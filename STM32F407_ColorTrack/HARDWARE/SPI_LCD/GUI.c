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
* ��    �ƣ�u16 LCD_BGR2RGB(u16 c)
* ��    �ܣ���ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��ͨ���ú���ת��
* ��ڲ�����c->GBR��ʽ����ɫֵ
* ���ڲ�����RGB��ʽ����ɫֵ
* ˵    ����
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
* ��    �ƣ�void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc)
* ��    �ܣ�����Ļ�ϻ�Բ
* ��ڲ�����x,y->Բ�����꣬R->Բ�İ뾶,fc->��ɫ���磺��ɫΪ0xf800
* ���ڲ�������
* ˵    ����
****************************************************************************/
void Gui_Circle(u16 X, u16 Y, u16 R, u16 fc)
{
    //Bresenham�㷨
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
* ��    �ƣ�void Gui_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color)
* ��    �ܣ����ߺ�����ʹ��Bresenham �����㷨
* ��ڲ�����x0,y0->���������;x1,y1->���յ�����;Color->�ߵ���ɫ
* ���ڲ�������
* ˵    ����
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
    dx = x1 - x0; //����x����
    dy = y1 - y0; //����y����

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

    if (dx > dy)   //x�������y���룬��ôÿ��x����ֻ��һ���㣬ÿ��y���������ɸ���
    {
        //���ߵĵ�������x���룬��x���������
        // initialize error term
        error = dy2 - dx;

        // draw the line
        for (index = 0; index <= dx; index++)   //Ҫ���ĵ������ᳬ��x����
        {
            //����
            Gui_DrawPoint(x0, y0, Color);

            // test if error has overflowed
            if (error >= 0)   //�Ƿ���Ҫ����y����ֵ
            {
                error -= dx2;

                // move to next line
                y0 += y_inc; //����y����ֵ
            } // end if error overflowed

            // adjust the error term
            error += dy2;

            // move to the next pixel
            x0 += x_inc; //x����ֵÿ�λ���󶼵���1
        } // end for
    } // end if |slope| <= 1
    else   //y�����x�ᣬ��ÿ��y����ֻ��һ���㣬x�����ɸ���
    {
        //��y��Ϊ��������
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
* ��    �ƣ�void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc)
* ��    �ܣ�������
* ��ڲ�����x,y->�������Ͻ�����;w,h->���εĿ�͸�;bc->��ɫ
* ���ڲ�������
* ˵    ����
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
* ��    �ƣ�void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode)
* ��    �ܣ�������
* ��ڲ�����x,y->�������Ͻ�����;w,h->���εĿ�͸�;mode->0��1��2��ͬ��ģʽ����Ч����һ��
* ���ڲ�������
* ˵    ����
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
* ��    �ƣ�void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2)
* ��    �ܣ�����Ļ��ʾһ͹��İ�ť��
* ��ڲ�����x1,y1->�������Ͻ�����;x2,y2->�������½�����;
* ���ڲ�������
* ˵    ����
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
* ��    �ƣ�void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2)
* ��    �ܣ�����Ļ��ʾһ���µİ�ť��
* ��ڲ�����x1,y1->�������Ͻ�����;x2,y2->�������½�����;
* ���ڲ�������
* ˵    ����
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
* ��    �ƣ�void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
* ��    �ܣ���16*16�����������ʾ���֣�����8*16���ַ�
* ��ڲ�����x,y->�������Ͻ�����;fc->������ɫ;bc->����ı�����ɫ;s->�ַ���ָ��
* ���ڲ�������
* ˵    �������������������16*16�ĵ����������8*16���ַ���ʾ��ʵ����
*           Gui_DrawFont_GBK16(16,0,BLUE,GRAY0,"ȫ�����Ӽ���");
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
* ��    �ƣ�void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
* ��    �ܣ���24*24�����������ʾ����,��12*24������ʾ�ַ�
* ��ڲ�����x,y->�������Ͻ�����;fc->������ɫ;bc->����ı�����ɫ;s->�ַ���ָ��
* ���ڲ�������
* ˵    �������������������24*24�ĵ���������ʾ��ʵ����
*           Gui_DrawFont_GBK24(x_offset,50,BLUE,WHITE, "     ��ӭ�� ");
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
* ��    �ƣ�void Gui_DrawFont_GBK32(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
* ��    �ܣ���32*32�����������ʾ����,��16*32������ʾ�ַ�
* ��ڲ�����x,y->�������Ͻ�����;fc->������ɫ;bc->����ı�����ɫ;s->�ַ���ָ��
* ���ڲ�������
* ˵    �������������������32*32�ĵ���������ʾ��ʵ����
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
* ��    �ƣ�void Gui_DrawFont_GBK40(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
* ��    �ܣ���40*40�����������ʾ����,��20*40������ʾ�ַ�
* ��ڲ�����x,y->�������Ͻ�����;fc->������ɫ;bc->����ı�����ɫ;s->�ַ���ָ��
* ���ڲ�������
* ˵    �������������������40*40�ĵ���������ʾ��ʵ����
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
* ��    �ƣ�void Gui_DrawFont_GBK32(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
* ��    �ܣ���64*64�����������ʾ����,��32*64������ʾ�ַ�
* ��ڲ�����x,y->�������Ͻ�����;fc->������ɫ;bc->����ı�����ɫ;s->�ַ���ָ��
* ���ڲ�������
* ˵    �������������������64*64�ĵ���������ʾ��ʵ����
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

            if (k == 0x25)   //�ڽ�����%��Ҫ�Ƚ�Сһ��
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
* ��    �ƣ�void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num)
* ��    �ܣ���32*32�����������ʾ����
* ��ڲ�����x,y->�������Ͻ�����;fc->������ɫ;bc->����ı�����ɫ;num->��ʾ������
* ���ڲ�������
* ˵    �����������ֻ������32*32�ĵ���������ʾ��ʵ����
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
* ��    �ƣ�void show_pic(const unsigned char *p)
* ��    �ܣ���ʾ240*320��ͼƬ
* ��ڲ�����p->���ͼƬȡģ��������ָ��
* ���ڲ�������
* ˵    ����������ʾ��ͼƬ�ĳߴ�һ��Ҫ��240*320��ȡģ��ʱ��16λ ��ֱɨ��  �ҵ���  ��λ��ǰ
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
* ��    �ƣ�void Gui_DrawBluetooth_Logo(u16 x, u16 y, u16 fc, u16 bc)
* ��    �ܣ���16*24���������ͼ��logo
* ��ڲ�����x,y->�������Ͻ�����;fc->������ɫ;bc->����ı�����ɫ
* ���ڲ�������
* ˵    ����
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
* ��    �ƣ�void Gui_DrawBattery_Logo(u16 x, u16 y, u16 fc, u16 bc, u8 valu)
* ��    �ܣ���16*24���������ͼ��logo
* ��ڲ�����x,y->�������Ͻ�����;fc->������ɫ;bc->����ı�����ɫ
* ���ڲ�������
* ˵    ����
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
* ��    �ƣ�void Gui_Draw44logo(u16 x, u16 y, u16 fc, u16 bc, u8 valu)
* ��    �ܣ���44*44����ı���ͼ��logo
* ��ڲ�����x,y->�������Ͻ�����;fc->������ɫ;bc->����ı�����ɫ
* ���ڲ�������
* ˵    ����
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



#include "key.h"

static u8 key_cnt, key_judge_cnt, key_click_cnt;
static u8 key_judge_flag, long_flag;
static u8 once_flag, twice_flag, thrice_flag, quartic_flag, quintic_flag;
u8 key_process;
KeyState key_state = KEY_UP;
char disBuff[30];

void key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);

    //GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
    //GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
}


u8 key_read(void)
{
    u8 key_val = 0;

    if (GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_DOWN)  key_val += 0x01;

    //if (GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_DOWN)  key_val += 0x02;
    return key_val;
}


void key_scan(void)
{
    u8 key_value;
    key_value = key_read();

    if (key_judge_flag)  key_judge_cnt++;

    if (key_judge_cnt > KEY_INTERVAL)
    {
        switch (key_click_cnt)
        {
            case 1:
                once_flag = 1;
                break;

            case 2:
                twice_flag = 1;
                break;

            case 3:
                thrice_flag = 1;
                break;

            case 4:
                quartic_flag = 1;
                break;

            case 5:
                quintic_flag = 1;
                break;

            default:
                break;
        }

        key_judge_cnt = 0;
        key_judge_flag = 0;
        key_click_cnt = 0;
    }

    switch (key_state)
    {
        case KEY_UP:
            if (key_value) key_state = KEY_DOWN;

            break;

        case KEY_DOWN:
            if (key_value)
            {
                key_judge_flag = 0;
                key_state = KEY_HOLD;
                key_process = key_value;
            }
            else
            {
                key_state = KEY_UP;
            }

            break;

        case KEY_HOLD:
            key_cnt++;

            if (!key_value)
            {
                key_cnt = 0;
                key_judge_cnt = 0;
                key_judge_flag = 1;
                key_click_cnt++;
                key_state = KEY_UP;
            }

            if (key_cnt > KEY_LONG_TIME)
            {
                key_state = KEY_LONG;
                key_judge_cnt = 0;
                key_judge_flag = 0;
                key_click_cnt = 0;
                long_flag = 1;
                key_process = key_value;
                //          key_state = KEY_AUTO;
                //          key_judge_cnt = 0;
                //          key_judge_flag = 0;
                //          key_click_cnt = 0;
                //          key_process = key_value;
            }

            break;

        case KEY_LONG:
            key_cnt++;

            if (!key_value)
            {
                key_cnt = 0;
                key_state = KEY_UP;
            }

            break;

        default:
            break;
    }
}


void key1_once_dispose(void)
{
    sprintf(disBuff, "once   ");
    OLED_ShowString(60, 0, (u8*)disBuff, 16);
    sprintf(disBuff, "pressure:%d temp:%.1f#", pressure, temperature);
    Usart_SendString(DEBUG_USARTx, disBuff);
    //    OLED_Print_Num1(10,0, mpu9.acc_x);
    //      OLED_Print_Num1(10,1, mpu9.acc_y);
    //      OLED_Print_Num1(10,2, mpu9.acc_z);

}

void key2_once_dispose(void)
{
    //    char dispBuff[50];
    //    LCD_SetFont(&Font16x24);
    //      LCD_SetTextColor(BLUE);
    //    sprintf(dispBuff,"once2");
    //    LCD_ClearLine(LINE(8));
    //      ILI9341_DispStringLine_EN(LINE(8),dispBuff);

}

void key1_twice_dispose(void)
{
    calib_pressure();
    sprintf(disBuff, "twice  ");
    OLED_ShowString(60, 0, (u8*)disBuff, 16);
}


void key2_twice_dispose(void)
{

}

void key1_thrice_dispose(void)
{
    sprintf(disBuff, "thrice ");
    OLED_ShowString(60, 0, (u8*)disBuff, 16);

}


void key2_thrice_dispose(void)
{


}

void key1_quartic_dispose(void)
{
    sprintf(disBuff, "quartic");
    OLED_ShowString(60, 0, (u8*)disBuff, 16);
}

void key2_quartic_dispose(void)
{
}

void key1_quintic_dispose(void)
{
    sprintf(disBuff, "quintic");
    OLED_ShowString(60, 0, (u8*)disBuff, 16);
}

void key2_quintic_dispose(void)
{
}

void key1_long_dispose(void)
{
    sprintf(disBuff, "long   ");
    OLED_ShowString(60, 0, (u8*)disBuff, 16);
}


void key2_long_dispose(void)
{

}

void key1_AUTO_dispose(void)
{

}


void key2_AUTO_dispose(void)
{

}


void key_dispose(void)
{
    if (once_flag)
    {
        once_flag = 0;

        switch (key_process)
        {
            case key1_val:
                key1_once_dispose();
                break;

            case key2_val:
                key2_once_dispose();
                break;

            default:
                break;
        }
    }

    if (twice_flag)
    {
        twice_flag = 0;

        switch (key_process)
        {
            case key1_val:
                key1_twice_dispose();
                break;

            case key2_val:
                key2_twice_dispose();
                break;

            default:
                break;
        }
    }

    if (thrice_flag)
    {
        thrice_flag = 0;

        switch (key_process)
        {
            case key1_val:
                key1_thrice_dispose();
                break;

            case key2_val:
                key2_thrice_dispose();
                break;

            default:
                break;
        }
    }

    if (quartic_flag)
    {
        quartic_flag = 0;

        switch (key_process)
        {
            case key1_val:
                key1_quartic_dispose();
                break;

            case key2_val:
                key2_quartic_dispose();
                break;

            default:
                break;
        }
    }


    if (quintic_flag)
    {
        quintic_flag = 0;

        switch (key_process)
        {
            case key1_val:
                key1_quintic_dispose();
                break;

            case key2_val:
                key2_quintic_dispose();
                break;

            default:
                break;
        }
    }

    if (long_flag)
    {
        long_flag = 0;

        switch (key_process)
        {
            case key1_val:
                key1_long_dispose();
                break;

            case key2_val:
                key2_long_dispose();
                break;

            default:
                break;
        }
    }

    //  if (key_state == KEY_AUTO && !(key_cnt % 10)) {
    //      switch (key_process) {
    //      case key1_val:
    //          key1_auto_dispose();break;
    //        case key2_val:
    //            key2_auto_dispose();break;
    //        default:break;
    //      }
    //  }
}


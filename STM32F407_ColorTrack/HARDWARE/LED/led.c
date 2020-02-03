#include "led.h"


//初始化PF9和PF10为输出口.并使能这两个口的时钟
//LED IO初始化
void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOF时钟


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化

    GPIO_SetBits(GPIOA, GPIO_Pin_6 | GPIO_Pin_7); //设置高，灯灭

}

//led打开
void LED_on(u8 num)
{
    switch (num)
    {
        case 0:
            GPIOA->ODR &= ~(1 << 6);
            break;

        case 1:
            GPIOA->ODR &= ~(1 << 7);
            break;

        case 0xff:
            GPIOA->ODR &= ~(3 << 6);
            break;

        default:
            break;
    }
}

//led关闭
void LED_off(u8 num)
{
    switch (num)
    {
        case 0:
            GPIOA->ODR |= (1 << 6);
            break;

        case 1:
            GPIOA->ODR |= (1 << 7);
            break;

        case 0xff:
            GPIOA->ODR |= (3 << 6);
            break;

        default:
            break;
    }
}

//led反转
void LED_chg(u8 num)
{
    switch (num)
    {
        case 0:
            GPIOA->ODR ^= (1 << 6);
            break;

        case 1:
            GPIOA->ODR ^= (1 << 7);
            break;

        case 0xff:
            GPIOA->ODR ^= (3 << 6);
            break;

        default:
            break;
    }
}



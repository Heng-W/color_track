#include "led.h"


//��ʼ��PF9��PF10Ϊ�����.��ʹ���������ڵ�ʱ��
//LED IO��ʼ��
void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOFʱ��


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��

    GPIO_SetBits(GPIOA, GPIO_Pin_6 | GPIO_Pin_7); //���øߣ�����

}

//led��
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

//led�ر�
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

//led��ת
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



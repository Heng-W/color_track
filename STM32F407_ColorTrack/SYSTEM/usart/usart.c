#include "sys.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"                   //ucos ʹ��
#endif
//////////////////////////////////////////////////////////////////////////////////

#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
    int handle;
};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
    x = x;
}
//�ض���fputc����
int fputc(int ch, FILE* f)
{
    while ((USART1->SR & 0X40) == 0); //ѭ������,ֱ���������
    USART1->DR = (u8) ch;
    return ch;
}
#endif

#if EN_USARTx_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���


//��ʼ��IO ����1
//bound:������
void uart_init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_GPIO_APBxClkCmd(USART_GPIO_CLK, ENABLE);  //ʹ��GPIOAʱ��
    USART_APBxClkCmd(USART_CLK, ENABLE);  //ʹ��USARTʱ��

    //���ڶ�Ӧ���Ÿ���ӳ��
    GPIO_PinAFConfig(USART_TX_GPIO_PORT, USART_TX_GPIO_PIN_SOURCE, USART_GPIO_AF_USARTx);  //GPIO����ΪUSARTx
    GPIO_PinAFConfig(USART_RX_GPIO_PORT, USART_RX_GPIO_PIN_SOURCE, USART_GPIO_AF_USARTx);  //GPIO����ΪUSARTx

    //USARTx�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //�ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
    GPIO_InitStructure.GPIO_Pin = USART_TX_GPIO_PIN;
    GPIO_Init(USART_TX_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = USART_RX_GPIO_PIN;
    GPIO_Init(USART_RX_GPIO_PORT, &GPIO_InitStructure);

    //USART ��ʼ������
    USART_InitStructure.USART_BaudRate = bound;//����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
    USART_Init(USARTx, &USART_InitStructure);  //��ʼ������1
    USART_Cmd(USARTx, ENABLE);  //ʹ�ܴ���1
    //USART_ClearFlag(USARTx, USART_FLAG_TC);

#if EN_USARTx_RX
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);  //��������ж�
    //Usartx NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART_IRQ;//����1�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);  //����ָ���Ĳ�����ʼ��VIC�Ĵ�����
#endif
}


u8 rx_buf[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 rx_idx, rxc_flag;       //��������������״̬���
u8 rx_len;                                  //���ݳ��ȣ��ֽڣ�

void USART_IRQHandler(void)
{
    u8 res;
    if (USART_GetITStatus(USARTx, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
        res = USART_ReceiveData(USARTx);
        if (rx_idx >= USART_REC_LEN)   //������󳤶������½���
            rx_idx = 0;
        if (rx_idx == 0 && res != 0xfe)
            return;    //����֡��ʼ�ֽ�Ϊ0xfe
        if (rx_idx == 1)
            rx_len = res;    //�ڶ����ֽ�Ϊ����
        rx_buf[rx_idx++] = res; //��Ч��Ϣ
        if (rx_idx > 2 && rx_idx >= rx_len)
        {
            rxc_flag = 1;   //���յ�ָ����������ɽ���
            USART_ITConfig(USARTx, USART_IT_RXNE, DISABLE);  //�رմ����ж�
        }
    }
}

/*****************  ����һ���ֽ� **********************/
void Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t ch)
{
    /* ����һ���ֽ����ݵ�USART */
    USART_SendData(pUSARTx, ch);
    /* �ȴ��������ݼĴ���Ϊ�� */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

/****************** ����8λ������ ************************/
void Usart_SendArray(USART_TypeDef* pUSARTx, uint8_t* array, uint16_t num)
{
    uint8_t i;
    for (i = 0; i < num; i++)
        Usart_SendByte(pUSARTx, array[i]);

    /* �ȴ�������� */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}

/*****************  �����ַ��� **********************/
void Usart_SendString(USART_TypeDef* pUSARTx, char* str)
{
    unsigned int k = 0;
    do
    {
        Usart_SendByte(pUSARTx, * (str + k));
        k++;
    }
    while (* (str + k) != '\0');
    /* �ȴ�������� */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
    {}
}

#include "MOTOR/motor.h"
#include "TIMER/timer.h"
#include "SPI_LCD/GUI.h"
#include "EXTI/exti.h"
#include "led.h"

#define PICK_W 20

#define h_range 15
#define s_range 30
#define l_range 30


u8 monitor_flag;

unsigned int judge_W = DEFAULT_W;
unsigned int judge_H = DEFAULT_H;

COLOR_HSL g_hsl;
COLOR_RGB g_rgb;

TARGET_CONDI condi_set;


//�����¼�����
void usart_dispose(void)
{
    if (rxc_flag == 0)
        return;

    LED_chg(1);
    char data[50];
    switch ((Command)rx_buf[2])   //����֡�������ֽ�Ϊ������ָ��
    {
        case start://�����̨��ʼ����
            start_flag = 1;
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case stop://�����ֹ̨ͣ����
            start_flag = 0;
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case up://����ͷ̧ͷ
            if (!start_flag)
            {
                g_angle_V -= 10;
                if (g_angle_V < 0)
                    g_angle_V = 0;
                Motor_SetAngle(CHx_MOTOR_V, g_angle_V);
            }
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case down://����ͷ��ͷ
            if (!start_flag)
            {
                g_angle_V += 10;
                if (g_angle_V > 180)
                    g_angle_V = 180;
                Motor_SetAngle(CHx_MOTOR_V, g_angle_V);
            }
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case left://����ͷ��ת
            if (!start_flag)
            {
                //Motor_AddToAngle(CHx_MOTOR_H, 10);
                g_angle_H += 10;
                if (g_angle_H > 180)
                    g_angle_H = 180;
                Motor_SetAngle(CHx_MOTOR_H, g_angle_H);
            }
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case right://����ͷ��ת
            if (!start_flag)
            {
                //Motor_AddToAngle(CHx_MOTOR_H, -10);
                g_angle_H -= 10;
                if (g_angle_H < 0)
                    g_angle_H = 0;
                Motor_SetAngle(CHx_MOTOR_H, g_angle_H);
            }
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case setsize://�趨׷��������С���
            judge_W = rx_buf[3];
            judge_H = rx_buf[4];
            set_size(judge_W, judge_H);
            condi_set.WIDTH_MIN = judge_W;
            condi_set.HIGHT_MIN = judge_H;
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case pick://���ղɼ�������ʶ���HSL
            Gui_box(160 - PICK_W / 2, 120 - PICK_W / 2, PICK_W, PICK_W, 0x0000);
            pick_avg();
            cal_condi(&g_hsl);
            free_def_flag = 1;
            sprintf(data, "%d %d %d %d\n", rx_buf[2], g_hsl.hue, g_hsl.saturation, g_hsl.luminance);
            break;
        case setcolor://����Ԥ������ɫ����ֵ
            color_idx = rx_buf[3];
            free_def_flag = 0;
            sprintf(data, "%d %d\n", rx_buf[2], color_idx);
            break;
        case defcolor://�����Զ�����ɫ��HSLֵ
            g_rgb.red = rx_buf[3];
            g_rgb.green = rx_buf[4];
            g_rgb.blue = rx_buf[5];
            rgb_to_hsl(&g_rgb, &g_hsl);
            cal_condi(&g_hsl);
            free_def_flag = 1;
            sprintf(data, "%d %d %d %d\n", rx_buf[2], g_rgb.red, g_rgb.green, g_rgb.blue);
            break;
        case start_monitor://������⣬��APP��������
            monitor_flag = 1;
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case stop_monitor://ֹͣ���
            monitor_flag = 0;
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case camera_reset://�����̨��λ
            g_angle_H = 90;
            g_angle_V = 90;
            Motor_SetAngle(CHx_MOTOR_H, 90);
            Motor_SetAngle(CHx_MOTOR_V, 90);
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case reset://����״̬��־��λ
            start_flag = 0;
            free_def_flag = 0;
            monitor_flag = 0;
            color_idx = 0;
            g_angle_H = 90;
            g_angle_V = 90;
            Motor_SetAngle(CHx_MOTOR_H, 90);
            Motor_SetAngle(CHx_MOTOR_V, 90);
            sprintf(data, "%d %d\n", rx_buf[2], color_idx);
            break;
        case init://����APP���Ӻ�����ʼ���������
            if (!free_def_flag)
                sprintf(data, "%d %d %d 0 %d\n", rx_buf[2], start_flag, monitor_flag, color_idx);
            else
                sprintf(data, "%d %d %d 1 %d %d %d\n", \
                        rx_buf[2], start_flag, monitor_flag, g_hsl.hue, g_hsl.saturation, g_hsl.luminance);
            break;
        case exit://�ر�����
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        default:
            break;
    }
    Usart_SendString(USARTx, data);  //��������
    rx_idx = 0;
    rxc_flag = 0;
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);  //�򿪴��ڽ����ж�
}


//�������е�״̬��Ϣ
void send_message(void)
{
    if (monitor_flag)
    {
        char data[50];
        if (find_flag)
            sprintf(data, "%d %d %.1f %.1f 1 %d %d %d %d\n", \
                    message, fps, g_angle_H, g_angle_V, Resu.x, Resu.y, Resu.w, Resu.h);
        else
            sprintf(data, "%d %d %.1f %.1f 0\n", message, fps, g_angle_H, g_angle_V);

        Usart_SendString(USARTx, data);
    }
}


//�ɼ���ΪPICK_W�ľ��������HSL�������ֵ
void pick_avg(void)
{
    COLOR_RGB rgb;
    COLOR_HSL hsl;
    u32 sum_h = 0, sum_s = 0, sum_l = 0;
    u16 starti = TRACK_H / 2 - PICK_W / 2;
    u16 startj = TRACK_W / 2 - PICK_W / 2;
    u16 endi = TRACK_H / 2 + PICK_W / 2;
    u16 endj = TRACK_W / 2 + PICK_W / 2;

    for (u16 i = starti; i < endi; i++)
    {
        for (u16 j = startj; j < endj; j++)
        {
            get_rgb(lcd_data[i][j], &rgb);
            rgb_to_hsl(&rgb, &hsl);  // �����rgbת��Ϊɫ�������ȣ��Աȶ�
            sum_h += hsl.hue;
            sum_s += hsl.saturation;
            sum_l += hsl.luminance;
        }
    }
    u16 cnt = PICK_W * PICK_W;
    u8 avg_h = sum_h / cnt;
    u8 avg_s = sum_s / cnt;
    u8 avg_l = sum_l / cnt;
    g_hsl.hue = avg_h;
    g_hsl.saturation = avg_s;
    g_hsl.luminance = avg_l;
}


//���ݼ����HSLֵȷ��ʶ����ж�����
void cal_condi(COLOR_HSL* hsl)
{
    u8 h1, h2, s1, s2, l1, l2;
    u8 h = hsl->hue;
    u8 s = hsl->saturation;
    u8 l = hsl->luminance;
    if (h < h_range)
        h1 = 0;
    else
        h1 = h - h_range;

    if (h + h_range > 240)
        h2 = 240;
    else
        h2 = h + h_range;

    if (s < s_range)
        s1 = 0;
    else
        s1 = s - s_range;

    if (s + s_range > 240)
        s2 = 240;
    else
        s2 = s + s_range;

    if (l < l_range)
        l1 = 0;
    else
        l1 = l - l_range;

    if (l + l_range > 240)
        l2 = 240;
    else
        l2 = l + l_range;

    TARGET_CONDI condi = {h1, h2, s1, s2, l1, l2, judge_W, judge_H, TRACK_W, TRACK_H};
    condi_set = condi;
}


#endif


#include "sys.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"                   //ucos 使用
#endif
//////////////////////////////////////////////////////////////////////////////////

#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE* f)
{
    while ((USART1->SR & 0X40) == 0); //循环发送,直到发送完毕
    USART1->DR = (u8) ch;
    return ch;
}
#endif

#if EN_USARTx_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误


//初始化IO 串口1
//bound:波特率
void uart_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_GPIO_APBxClkCmd(USART_GPIO_CLK, ENABLE);  //使能GPIOA时钟
    USART_APBxClkCmd(USART_CLK, ENABLE);  //使能USART时钟

    //串口对应引脚复用映射
    GPIO_PinAFConfig(USART_TX_GPIO_PORT, USART_TX_GPIO_PIN_SOURCE, USART_GPIO_AF_USARTx);  //GPIO复用为USARTx
    GPIO_PinAFConfig(USART_RX_GPIO_PORT, USART_RX_GPIO_PIN_SOURCE, USART_GPIO_AF_USARTx);  //GPIO复用为USARTx

    //USARTx端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_InitStructure.GPIO_Pin = USART_TX_GPIO_PIN;
    GPIO_Init(USART_TX_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = USART_RX_GPIO_PIN;
    GPIO_Init(USART_RX_GPIO_PORT, &GPIO_InitStructure);

    //USART 初始化设置
    USART_InitStructure.USART_BaudRate = bound;//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
    USART_Init(USARTx, &USART_InitStructure);  //初始化串口1
    USART_Cmd(USARTx, ENABLE);  //使能串口1
    //USART_ClearFlag(USARTx, USART_FLAG_TC);

#if EN_USARTx_RX
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);  //开启相关中断
    //Usartx NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART_IRQ;//串口1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);  //根据指定的参数初始化VIC寄存器、
#endif
}


u8 rx_buf[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 rx_idx, rxc_flag;       //接收索引，接收状态标记
u8 rx_len;                                  //数据长度（字节）

void USART_IRQHandler(void)
{
    u8 res;
    if (USART_GetITStatus(USARTx, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
        res = USART_ReceiveData(USARTx);
        if (rx_idx >= USART_REC_LEN)   //超过最大长度则重新接收
            rx_idx = 0;
        if (rx_idx == 0 && res != 0xfe)
            return;    //数据帧起始字节为0xfe
        if (rx_idx == 1)
            rx_len = res;    //第二个字节为长度
        rx_buf[rx_idx++] = res; //有效信息
        if (rx_idx > 2 && rx_idx >= rx_len)
        {
            rxc_flag = 1;   //接收到指定长度则完成接收
            USART_ITConfig(USARTx, USART_IT_RXNE, DISABLE);  //关闭串口中断
        }
    }
}

/*****************  发送一个字节 **********************/
void Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t ch)
{
    /* 发送一个字节数据到USART */
    USART_SendData(pUSARTx, ch);
    /* 等待发送数据寄存器为空 */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

/****************** 发送8位的数组 ************************/
void Usart_SendArray(USART_TypeDef* pUSARTx, uint8_t* array, uint16_t num)
{
    uint8_t i;
    for (i = 0; i < num; i++)
        Usart_SendByte(pUSARTx, array[i]);

    /* 等待发送完成 */
    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}

/*****************  发送字符串 **********************/
void Usart_SendString(USART_TypeDef* pUSARTx, char* str)
{
    unsigned int k = 0;
    do
    {
        Usart_SendByte(pUSARTx, * (str + k));
        k++;
    }
    while (* (str + k) != '\0');
    /* 等待发送完成 */
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


//串口事件处理
void usart_dispose(void)
{
    if (rxc_flag == 0)
        return;

    LED_chg(1);
    char data[50];
    switch ((Command)rx_buf[2])   //数据帧第三个字节为编码后的指令
    {
        case start://舵机云台开始动作
            start_flag = 1;
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case stop://舵机云台停止动作
            start_flag = 0;
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case up://摄像头抬头
            if (!start_flag)
            {
                g_angle_V -= 10;
                if (g_angle_V < 0)
                    g_angle_V = 0;
                Motor_SetAngle(CHx_MOTOR_V, g_angle_V);
            }
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case down://摄像头低头
            if (!start_flag)
            {
                g_angle_V += 10;
                if (g_angle_V > 180)
                    g_angle_V = 180;
                Motor_SetAngle(CHx_MOTOR_V, g_angle_V);
            }
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case left://摄像头左转
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
        case right://摄像头右转
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
        case setsize://设定追踪物体最小宽高
            judge_W = rx_buf[3];
            judge_H = rx_buf[4];
            set_size(judge_W, judge_H);
            condi_set.WIDTH_MIN = judge_W;
            condi_set.HIGHT_MIN = judge_H;
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case pick://拍照采集，计算识别的HSL
            Gui_box(160 - PICK_W / 2, 120 - PICK_W / 2, PICK_W, PICK_W, 0x0000);
            pick_avg();
            cal_condi(&g_hsl);
            free_def_flag = 1;
            sprintf(data, "%d %d %d %d\n", rx_buf[2], g_hsl.hue, g_hsl.saturation, g_hsl.luminance);
            break;
        case setcolor://设置预定义颜色索引值
            color_idx = rx_buf[3];
            free_def_flag = 0;
            sprintf(data, "%d %d\n", rx_buf[2], color_idx);
            break;
        case defcolor://设置自定义颜色的HSL值
            g_rgb.red = rx_buf[3];
            g_rgb.green = rx_buf[4];
            g_rgb.blue = rx_buf[5];
            rgb_to_hsl(&g_rgb, &g_hsl);
            cal_condi(&g_hsl);
            free_def_flag = 1;
            sprintf(data, "%d %d %d %d\n", rx_buf[2], g_rgb.red, g_rgb.green, g_rgb.blue);
            break;
        case start_monitor://开启监测，向APP传输数据
            monitor_flag = 1;
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case stop_monitor://停止监测
            monitor_flag = 0;
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case camera_reset://舵机云台复位
            g_angle_H = 90;
            g_angle_V = 90;
            Motor_SetAngle(CHx_MOTOR_H, 90);
            Motor_SetAngle(CHx_MOTOR_V, 90);
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        case reset://所有状态标志复位
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
        case init://传输APP连接后界面初始化所需变量
            if (!free_def_flag)
                sprintf(data, "%d %d %d 0 %d\n", rx_buf[2], start_flag, monitor_flag, color_idx);
            else
                sprintf(data, "%d %d %d 1 %d %d %d\n", \
                        rx_buf[2], start_flag, monitor_flag, g_hsl.hue, g_hsl.saturation, g_hsl.luminance);
            break;
        case exit://关闭连接
            sprintf(data, "%d\n", rx_buf[2]);
            break;
        default:
            break;
    }
    Usart_SendString(USARTx, data);  //发送数据
    rx_idx = 0;
    rxc_flag = 0;
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);  //打开串口接收中断
}


//传输运行的状态信息
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


//采集宽为PICK_W的矩形区域的HSL，计算均值
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
            rgb_to_hsl(&rgb, &hsl);  // 完成了rgb转化为色调，亮度，对比度
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


//根据计算的HSL值确定识别的判定条件
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


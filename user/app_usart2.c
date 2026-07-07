#include "app_usart2.h"

//
// @简介：对USART2进行初始化
//
void App_USART2_Init(void)
{
    //1.使能USART2时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    //2.配置USART2的GPIO引脚
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIOA时钟

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2为USART2的TX引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //输出速度为2MHz

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //PA3为USART2的RX引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入模式

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //3.配置USART2参数
    USART_InitTypeDef USART_InitStructure = {0};

    USART_InitStructure.USART_BaudRate = 921600; //波特率为921600
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位为8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位为1位
    USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //使能接收和发送功能

    USART_Init(USART2, &USART_InitStructure);

    USART_Cmd(USART2, ENABLE); //使能USART2
}

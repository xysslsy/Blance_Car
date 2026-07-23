#include "app_rc.h"
#include "string.h"
#include "stdio.h"
#include "app_control.h"

#define CMD_MAX_LEN 64

static char intBuf[CMD_MAX_LEN];   //中断接收缓冲区，用于存储从遥控模块接收到的数据
static char transBuf[CMD_MAX_LEN];  //传输缓冲区，用于存储从中断接收缓冲区复制过来的数据，以便在主循环中进行处理
static char procBuf[CMD_MAX_LEN];  //处理缓冲区，用于存储从传输缓冲区复制过来的数据，以便在主循环中进行处理

static volatile uint8_t lineReceivedFlag = 0; //标志位，表示是否接收到完整的一行数据
static uint16_t intBufCursor = 0; //中断接收缓冲区的游标，用于指示当前接收到的数据在缓冲区中的位置

//
//@简介： 初始化遥控模块
//
void App_RC_Init(void)
{
    //开启GPIOB的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //初始化PB10 - AFPP -USART3 Tx
    GPIO_InitTypeDef GPIO_InitStruct = {0}; //GPIO初始化结构体
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; //遥控模块连接到GPIOB的第10号引脚
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; //设置为上拉输入模式
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz; //设置GPIO速度为2MHz

    GPIO_Init(GPIOB, &GPIO_InitStruct);

    //初始化PB11 - IPU -USART3 Rx
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11; //遥控模块连接到GPIOB的第11号引脚
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; //设置为上拉输入模式
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz; //设置GPIO速度为2MHz

    GPIO_Init(GPIOB, &GPIO_InitStruct);

    //为USART3开启时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    //配置USART3的参数
    USART_InitTypeDef USART_InitStruct = {0}; //USART初始化结构体

    USART_InitStruct.USART_BaudRate = 9600; //设置波特率
    USART_InitStruct.USART_WordLength = USART_WordLength_8b; //设置数据位数
    USART_InitStruct.USART_StopBits = USART_StopBits_1; //设置停止位
    USART_InitStruct.USART_Parity = USART_Parity_No; //设置校验位
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //设置模式为收发

    USART_Init(USART3, &USART_InitStruct);

    //开启USART3的RXNE终端
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStruct = {0}; //NVIC初始化结构体

    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn; //设置中断通道为USART3
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; //设置抢占优先级为0
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0; //设置子优先级为0
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; //使能中断

    NVIC_Init(&NVIC_InitStruct);

    //使能USART3
    USART_Cmd(USART3, ENABLE);
}

//
//@简介： 遥控模块处理函数
//
void App_RC_Proc(void)
{
    if(lineReceivedFlag)
    {
        strcpy(procBuf, transBuf); //将传输缓冲区的数据复制到处理缓冲区

        lineReceivedFlag = 0; //清除标志位，表示已经处理完接收到的一行数据
    }

    //解析
    if(strncasecmp(procBuf, "move ", 5) == 0)
    {
        // 处理move命令
        int turnspeed, movespeed;

        if (sscanf(procBuf, "move %d %d", &turnspeed, &movespeed) == 2) //解析命令中的参数
        {
            // 使用turnspeed和movespeed进行相应的处理
            App_Control_SetMoveSpeed(-movespeed * 0.01f * 0.7f); //设置移动速度，最大行进速度0.7m/s
            App_Control_SetTurnSpeed(-turnspeed * 0.01f * 15.0f); //设置转向速度，最大转向速度15rad/s
        }
    }
}

//
//@简介： USART3中断响应函数
//
void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //检查是否接收到数据
    {
        uint8_t data = USART_ReceiveData(USART3); //读取接收到的数据

        if (data != '\n') //如果接收到换行符，表示一行数据接收完成
        {
            intBuf[intBufCursor++] = data; //将接收到的数据存入中断接收缓冲区，并移动游标
        }
        else
        {
            intBuf[intBufCursor] = '\0'; //在接收到的字符串末尾添加结束符
            intBufCursor = 0; //重置游标，为下一次接收做准备
            strcpy(transBuf, intBuf); //将中断接收缓冲区的数据复制到传输缓冲区
            lineReceivedFlag = 1; //设置标志位，表示接收到完整的一行数据
        }
    }
}

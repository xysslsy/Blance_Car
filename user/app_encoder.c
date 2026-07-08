#include "app_encoder.h"

static volatile int64_t encoder_l = 0; //左电机编码器的值
static volatile int64_t encoder_r = 0; //右电机编码器的值

static void Encoder_L_Init(void);
static void Encoder_R_Init(void);

//
//@简介：对编码器模块进行初始化
//
void App_Encoder_Init(void)
{
    Encoder_L_Init();
    Encoder_R_Init();
}

//
//@简介：获取左电机编码器的当前位置
//
int64_t App_Encoder_GetPos_L(void)
{
    return encoder_l;
}

//
//@简介：获取右电机编码器的当前位置
//
int64_t App_Encoder_GetPos_R(void)
{
    return encoder_r;
}

//
//@简介：对左电机编码器进行初始化
//
static void Encoder_L_Init(void)
{
    //初始化A B的IO引脚
    //PB14 PB15 - IPU
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //EXTI中断初始化
    //让EXTI_Line14与PB14相连
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);

    //配置EXTI的参数
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    EXTI_InitStructure.EXTI_Line = EXTI_Line14;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;

    EXTI_Init(&EXTI_InitStructure);

    //开启EXTI的中断
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

}

//
//@简介：对右电机编码器进行初始化
//
static void Encoder_R_Init(void)
{
    //初始化A B的IO引脚
    //关闭JTAG，开启SWD，PB3 PB4作为普通IO口使用
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    //PB3 PB4 - IPU
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //EXTI中断初始化
    //让EXTI_Line3与PB3相连
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3);

    //配置EXTI的参数
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;

    EXTI_Init(&EXTI_InitStructure);

    //开启EXTI的中断
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

}

    //
    //@简介：右电机编码器中断响应函数,右编码器A项
    //
    void EXTI3_IRQHandler(void)
    {
        EXTI_ClearFlag(EXTI_Line3); //清除中断标志位

        uint8_t a = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);  //读取A相的电平
        uint8_t b = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);  //读取B相的电平

        if(a == Bit_SET)  //上升沿
        {
            if(b == Bit_SET)  //B相为高电平，说明是正转
            {
                encoder_r++;
            }
            else  //B相为低电平，说明是反转
            {
                encoder_r--;
            }
        }
        else  //下降沿
        {
            if(b == Bit_SET)  //B相为高电平，说明是反转
            {
                encoder_r--;
            }
            else  //B相为低电平，说明是正转
            {
                encoder_r++;
            }
        }
    }

    //
    //@简介：左电机编码器中断响应函数,左编码器A项
    //
    void EXTI15_10_IRQHandler(void)
    {
        if(EXTI_GetITStatus(EXTI_Line14) == SET)  //判断是否是EXTI_Line14的中断
        {
            EXTI_ClearITPendingBit(EXTI_Line14); //清除中断标志位

            uint8_t a = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);  //读取A相的电平
            uint8_t b = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);  //读取B相的电平

            if(a == Bit_SET)  //上升沿
            {
                if(b == Bit_SET)  //B相为高电平，说明是正转
                {
                    encoder_l++;
                }
                else  //B相为低电平，说明是反转
                {
                    encoder_l--;
                }
            }
            else  //下降沿
            {
                if(b == Bit_SET)  //B相为高电平，说明是反转
                {
                    encoder_l--;
                }
                else  //B相为低电平，说明是正转
                {
                    encoder_l++;
                }
            }
        }
    }

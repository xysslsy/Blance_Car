#include "app_bat.h"
#include "delay.h"

static volatile float vbat = 0.0f; //电池电压值

static void TIM2_TRGO_Init(void); //TIM2_TRGO初始化函数
static void ADC1_Init(void); //ADC1初始化函数
static void LED_Init(void); //LED初始化函数

//
// @简介：对电池电压检测模块进行初始化
//
void App_Bat_Init(void)
{
   TIM2_TRGO_Init(); //初始化TIM2_TRGO
   ADC1_Init(); //初始化ADC1
   LED_Init(); //初始化LED
}

static uint32_t lastTime = 0; //上次LED切换的时间
static uint8_t stage = 0; //LED状态，0表示关闭，1表示

//
// @简介：电池电压检测模块的任务切片
//
void App_Bat_Proc(void)
{
    if (vbat > 7.5)  //电池满电，点亮三颗
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_6, Bit_SET);
    }
    else if (vbat > 7.4)  //%75 电量，点亮两颗
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_6, Bit_RESET);
    }
    else if (vbat > 7.0)  //%50 电量，点亮一颗
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_6, Bit_RESET);
    }
    else if (vbat > 6.5)  //%25 电量，熄灭所有LED
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_6, Bit_RESET);
    }
    else //电量过低，所有灯快闪
    {
        uint32_t now = GetTick(); //获取当前时间

        if(now - lastTime >= 100) //每100ms切换一次LED状态
        {
            switch(stage)
            {
                case 0: //当前熄灭
                    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
                    GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
                    GPIO_WriteBit(GPIOA, GPIO_Pin_6, Bit_SET);
                    stage = 1;
                    break;
                case 1: //当前点亮
                    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
                    GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
                    GPIO_WriteBit(GPIOA, GPIO_Pin_6, Bit_RESET);
                    stage = 0;
                    break;
            }
            lastTime = now; //更新上次切换时间
        }
    }
}

//
//@简介：对电源指示灯进行初始化
//      PA4 PA5 PA6 - Out_PP
//
static void LED_Init(void)
{
    //1.初始化LED
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIOC时钟

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6; //PC13为板载LED引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //输出速度为2MHz

    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void TIM2_TRGO_Init(void)
{
     //1.初始化TIM2_TRGO,每10ms产生一个脉冲
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //使能TIM2时钟
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    TIM_TimeBaseStructure.TIM_Period = 10000 - 1; //ARR
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; //PSC
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update); //选择触发输出源为更新事件

    TIM_Cmd(TIM2, ENABLE); //使能TIM2
}

static void ADC1_Init(void)
{
     //2.对ADC进行初始化
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); //ADC时钟为12MHz

    //将PB0配置为模拟输入模式
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //PB0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入

    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

    ADC_InitTypeDef ADC_InitStructure = {0};

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //独立模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //非连续转换模式
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //右对齐
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //外部触发转换源为无
    ADC_InitStructure.ADC_NbrOfChannel = 1; //转换通道数
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; //非扫描模式

    ADC_Init(ADC1, &ADC_InitStructure);

    //设置注入序列的参数
    ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T2_TRGO); //注入序列的外部触发源为TIM2_TRGO
    ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE); //使能注入序列的外部触发
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_1Cycles5); //设置注入序列的通道为ADC_Channel_8，采样时间为1.5个周期

    //配置ADC的JEOC中断
    ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE); //使能ADC1的注入序列转换完成中断

    //NIVIC 中断优先级分组 0(4位抢占优先级，0位响应优先级)
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn; //ADC1和ADC2的中断
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级为0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //响应优先级为2

    NVIC_Init(&NVIC_InitStructure);

    ADC_Cmd(ADC1, ENABLE); //使能ADC1
}

//
// @简介：获取电池电压值,单位为V
//
float App_Bat_Get(void)
{
    return vbat;
}

//
//@简介：ADC1和ADC2的中断服务函数
//
void ADC1_2_IRQHandler(void)
{
    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC) == SET) //判断是否为注入序列转换完成中断
    {
        ADC_ClearFlag(ADC1, ADC_FLAG_JEOC); //清除中断标志位

        uint16_t jdr1 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1); //获取注入序列的转换值

        //在这里可以对adc_value进行处理，比如计算电池电压等
        vbat = (float)jdr1 / 4095.0f * 3.3f * 8.4f / 3.3f;

        ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC); //清除中断标志位
    }
}

#include "app_pwm.h"
#include "math.h"

static void STBY_Pin_Init(void);
static void Motor_L_Init(void);
static void Motor_R_Init(void);

//
//简介：对tb6612进行初始化
//
void App_PWM_Init(void)
{
    STBY_Pin_Init(); //初始化STBY引脚
    Motor_L_Init(); //初始化左电机
    Motor_R_Init(); //初始化右电机
}

//
//简介：对tb6612的STBY引脚进行控制,控制tb6612的工作状态
//参数：on - 非0:使能tb6612,向STBY引脚输出高电平
//           0:禁用tb6612,向STBY引脚输出低电平
//
void App_PWM_Cmd(uint8_t on)
{
    if(on == 0)
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET); //向STBY引脚输出低电平
    }
    else
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET); //向STBY引脚输出高电平
    }
}

//
//简介：设置左电机的PWM占空比
//参数：duty - 占空比,范围:-100.0f~100.0f
//
void App_PWM_Set_L(float duty)
{
    float sign; //符号位,用于判断电机的转动方向

    if(duty >= 0.0f)
    {
        sign = 1.0f; //正数,电机正转
    }
    else
    {
        sign = -1.0f; //负数,电机反转
    }

    duty = fabsf(duty); //取绝对值

    if(sign < 0.0f) //电机反转
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET); //向AIN1引脚输出高电平
        GPIO_WriteBit(GPIOA, GPIO_Pin_10, Bit_RESET); //向AIN2引脚输出低电平
    }
    else //电机正转
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET); //向AIN1引脚输出低电平
        GPIO_WriteBit(GPIOA, GPIO_Pin_10, Bit_SET); //向AIN2引脚输出高电平
    }

    uint16_t ccr = duty / 100.0f * 999; //计算捕获/比较寄存器的值

    TIM_SetCompare1(TIM1, ccr); //设置捕获/比较寄存器的值
}

//
//简介：设置右电机的PWM占空比
//参数：duty - 占空比,范围:-100.0f~100.0f
//
void App_PWM_Set_R(float duty)
{
    float sign; //符号位,用于判断电机的转动方向

    if(duty >= 0.0f)
    {
        sign = 1.0f; //正数,电机正转
    }
    else
    {
        sign = -1.0f; //负数,电机反转
    }

    duty = fabsf(duty); //取绝对值

    if(sign > 0.0f) //电机正转
    {
        GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET); //向BIN1引脚输出高电平
        GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET); //向BIN2引脚输出低电平
    }
    else //电机反转
    {
        GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET); //向BIN1引脚输出低电平
        GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET); //向BIN2引脚输出高电平
    }

    uint16_t ccr = duty / 100.0f * 999; //计算捕获/比较寄存器的值

    TIM_SetCompare1(TIM4, ccr); //设置捕获/比较寄存器的值
}

//
//简介：对tb6612的STBY引脚进行初始化
//     PA1 - Out_PP
//
static void STBY_Pin_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIOA时钟

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PA1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //2MHz

    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化GPIOA
}

//
//简介：对tb6612的左电机进行初始化
//        AIN1 - PA9 - Out_PP
//        AIN2 - PA10 - Out_PP
//
static void Motor_L_Init(void)
{
    //初始化AIN1 AIN2
    
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIOA时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //PA9, PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //2MHz

    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化GPIOA

    //初始化PWM
    //初始化PA8 - AF_PP
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIOA时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //PA8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //2MHz

    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化GPIOA

    //对定时器1进行初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //使能定时器1时钟

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};

    TIM_TimeBaseStructure.TIM_Period = 1000 - 1; //自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //预分频器
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; //重复计数器
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //初始化定时器1

    //配置输出比较
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出比较极性:TIM输出比较极性高
    TIM_OCInitStructure.TIM_OutputState = ENABLE; //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = 0; //捕获/比较值

    TIM_OC1Init(TIM1, &TIM_OCInitStructure); //初始化输出比较1

    //配置moe的开关
    TIM_CtrlPWMOutputs(TIM1, ENABLE); //MOE的开关

    TIM_Cmd(TIM1, ENABLE); //使能定时器1
}

//
//简介：对tb6612的右电机进行初始化
//        BIN1 - PB5 - Out_PP
//        BIN2 - PB7 - Out_PP
//
static void Motor_R_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能GPIOB时钟

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7; //PB5, PB7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //2MHz

    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化GPIOB

    //初始化PWM
    //初始化PB6 - AF_PP
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能GPIOB时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //PB6
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //2MHz

    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化GPIOB

    //对定时器4进行初始化
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //使能定时器4时钟

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};

    TIM_TimeBaseStructure.TIM_Period = 1000 - 1; //自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //预分频器
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; //重复计数器
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //初始化定时器4

    //配置输出比较
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出比较极性:TIM输出比较极性高
    TIM_OCInitStructure.TIM_OutputState = ENABLE; //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = 0; //捕获/比较值

    TIM_OC1Init(TIM4, &TIM_OCInitStructure); //初始化输出比较1

    //配置moe的开关
    TIM_CtrlPWMOutputs(TIM4, ENABLE); //MOE的开关

    TIM_Cmd(TIM4, ENABLE); //使能定时器4
}

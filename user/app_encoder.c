#include "app_encoder.h"
#include "delay.h"

static volatile int64_t encoder_l = 0; //左电机编码器的值
static volatile int64_t encoder_r = 0; //右电机编码器的值
static volatile int8_t  direction_l = 0; //左电机编码器的方向，1表示正转，-1表示反转
static volatile int8_t  direction_r = 0; //右电机编码器的方向，1表示正转，-1表示反转
static volatile uint64_t t0_l = 0, t1_l = 0;//左电机编码器发生变化的时间，单位是us
static volatile uint64_t t0_r = 0, t1_r = 0;//右电机编码器发生变化的时间，单位是us
static float filtered_speed_l = 0.0f; //左轮滤波后的角速度
static float filtered_speed_r = 0.0f; //右轮滤波后的角速度

static void Encoder_L_Init(void);
static void Encoder_R_Init(void);

//
//@简介：对编码器模块进行初始化
//
void App_Encoder_Init(void)
{
    filtered_speed_l = 0.0f;
    filtered_speed_r = 0.0f;

    Encoder_L_Init();
    Encoder_R_Init();
}

//
//@简介：获取左轮胎旋转的角度，单位是°
//
float App_Encoder_GetPos_L(void)
{
    return encoder_l / 22.0f / (30613.0f / 1500.0f) * 360.0f; //返回角度值
}

//
//@简介：获取右轮胎旋转的角度，单位是°
//
float App_Encoder_GetPos_R(void)
{
    return encoder_r / 22.0f / (30613.0f / 1500.0f) * 360.0f; //返回角度值
}

//
//@简介：获取左轮胎旋转的角速度，单位是°/s
//
float App_Encoder_GetSpeed_L(void)
{
    __disable_irq(); //关闭中断，防止在计算速度时发生中断，导致时间不准确

    int8_t direction_cpy = direction_l; //保存当前的方向值
    uint64_t t0_cpy = t0_l; //保存当前的时间值
    uint64_t t1_cpy = t1_l; //保存上一次的时间值

    __enable_irq(); //开启中断

    if(direction_cpy == 2 || direction_cpy == -2) //如果发生了方向的改变
    {
        filtered_speed_l = 0.0f; //无效状态时重置滤波
        return 0.0f; //返回0，表示速度为0
    }

    uint64_t now = GetUs(); //获取当前时间，单位是us
    float T = 0.0f; //时间间隔，单位是s

    if(t0_cpy - t1_cpy > now - t0_cpy)
    {
        T = (t0_cpy - t1_cpy) * 1.0e-6f; //计算时间间隔，单位是s
    }
    else
    {
        T = (now - t0_cpy) * 1.0e-6f; //计算时间间隔，单位是s
    }

    float raw_speed = direction_cpy / T / 22.0f / (30613.0f / 1500.0f) * 360.0f; //原始角速度

    const float alpha = 0.1f; //指数移动平均滤波系数
    filtered_speed_l = alpha * raw_speed + (1.0f - alpha) * filtered_speed_l;

    return filtered_speed_l; //返回滤波后的角速度值
}

//
//@简介：获取右轮胎旋转的角速度，单位是°/s
//
float App_Encoder_GetSpeed_R(void)
{
    __disable_irq(); //关闭中断，防止在计算速度时发生中断，导致时间不准确

    int8_t direction_cpy = direction_r; //保存当前的方向值
    uint64_t t0_cpy = t0_r; //保存当前的时间值
    uint64_t t1_cpy = t1_r; //保存上一次的时间值

    __enable_irq(); //开启中断

    if(direction_cpy == 2 || direction_cpy == -2) //如果发生了方向的改变
    {
        filtered_speed_r = 0.0f; //无效状态时重置滤波
        return 0.0f; //返回0，表示速度为0
    }

    uint64_t now = GetUs(); //获取当前时间，单位是us
    float T = 0.0f; //时间间隔，单位是s

    if(t0_cpy - t1_cpy > now - t0_cpy)
    {
        T = (t0_cpy - t1_cpy) * 1.0e-6f; //计算时间间隔，单位是s
    }
    else
    {
        T = (now - t0_cpy) * 1.0e-6f; //计算时间间隔，单位是s
    }

    float raw_speed = direction_cpy / T / 22.0f / (30613.0f / 1500.0f) * 360.0f; //原始角速度

    const float alpha = 0.1f; //指数移动平均滤波系数
    filtered_speed_r = alpha * raw_speed + (1.0f - alpha) * filtered_speed_r;

    return filtered_speed_r; //返回滤波后的角速度值
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

        t1_r = t0_r; //保存上一次的时间
        t0_r = GetUs(); //获取当前时间，单位是us

        uint8_t a = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);  //读取A相的电平
        uint8_t b = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);  //读取B相的电平

        if((a ==  Bit_SET && b == Bit_RESET) || (a == Bit_RESET && b == Bit_SET)) //轮胎正转
        {
            encoder_r++;

            if(direction_r < 0) //如果之前是反转
            {
                direction_r = 2; //表示发生了正转
            }
            else
            {
                direction_r = 1; //正转
            }
        }
        else //轮胎反转
        {
            encoder_r--;

            if(direction_r > 0) //如果之前是正转
            {
                direction_r = -2; //表示发生了反转
            }
            else
            {
                direction_r = -1; //反转
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

            t1_l = t0_l; //保存上一次的时间
            t0_l = GetUs(); //获取当前时间，单位是us

            uint8_t a = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);  //读取A相的电平
            uint8_t b = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);  //读取B相的电平

            if((a ==  Bit_SET && b == Bit_RESET) || (a == Bit_RESET && b == Bit_SET)) //轮胎反转
            {
                encoder_l--;
                
                if(direction_l > 0) //如果之前是正转
                {
                    direction_l = -2; //表示发生了反转
                }
                else
                {
                    direction_l = -1; //反转
                }
            }
            else //轮胎正转
            {
                encoder_l++;

                if(direction_l < 0) //如果之前是反转
                {
                    direction_l = 2; //表示发生了正转
                }
                else
                {
                    direction_l = 1; //正转
                }
            }          
        }
    }

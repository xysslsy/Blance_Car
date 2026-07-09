#include "encoder_test.h"
#include "app_usart2.h"
#include "delay.h"
#include "app_encoder.h"

//
//@简介：对编码器的位置信号进行测试，会通过调试用的串口把编码器的值发送到vofa
//
void Encoder_Test(void)
{
    //在这里编写编码器测试代码
    App_USART2_Init();
    App_Encoder_Init();

    while(1)
    {
        float pos_l = App_Encoder_GetPos_L();
        float pos_r = App_Encoder_GetPos_R();

        My_USART_Printf(USART2, "%f,%f\n", pos_l, pos_r);

        Delay(50);
    }
}

static float last_pos_l = 0.0f;
static float last_pos_r = 0.0f;

//
//@简介：对编码器的M法测速进行测试，会通过调试用的串口把编码器的值发送到vofa
//
void Encoder_M_Method_Test(void)
{
    App_Encoder_Init();
    App_USART2_Init();

    while(1)
    {
        Delay(1);
        
        float pos_l = App_Encoder_GetPos_L();
        float pos_r = App_Encoder_GetPos_R();

        float M_l = pos_l - last_pos_l;
        float M_r = pos_r - last_pos_r;

        float omega_l = M_l / 0.001f; //左轮胎的角速度，单位是°/s
        float omega_r = M_r / 0.001f; //右轮胎的角速度，单位是°/s

        My_USART_Printf(USART2, "%f,%f,%f,%f\n", pos_l, pos_r, omega_l, omega_r);

        last_pos_l = pos_l;
        last_pos_r = pos_r;
    }
}

//
//@简介：对编码器的T法测速进行测试，会通过调试用的串口把编码器的值发送到vofa
//
void Encoder_T_Method_Test(void)
{
    App_Encoder_Init();
    App_USART2_Init();

    while(1)
    {
        Delay(1);
        
        float speed_l = App_Encoder_GetSpeed_L();
        float speed_r = App_Encoder_GetSpeed_R();

        My_USART_Printf(USART2, "%f,%f\n", speed_l, speed_r);
    }
}

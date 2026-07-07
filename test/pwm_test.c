#include "pwm_test.h"
#include "app_pwm.h"
#include "delay.h"

//
//简介：测试PWM输出
//     先使能电机
//     让电机正转30% 2s
//     让电机正转60% 2s
//     让电机正转90% 2s
//     关闭电机
//
void PWM_Test(void)   // main
{
    App_PWM_Init(); //初始化PWM

    App_PWM_Cmd(1); //使能电机

    App_PWM_Set_L(30.0f); //设置左电机占空比为30%
    App_PWM_Set_R(30.0f); //设置右电机占空比为30%

    Delay(2000); //延时2s

    App_PWM_Set_L(60.0f); //设置左电机占空比为60%
    App_PWM_Set_R(60.0f); //设置右电机占空比为60%

    Delay(2000); //延时2s

    App_PWM_Set_L(90.0f); //设置左电机占空比为90%
    App_PWM_Set_R(90.0f); //设置右电机占空比为90%

    Delay(2000); //延时2s

    App_PWM_Cmd(0); //关闭电机

    while(1)
    {

    }
}

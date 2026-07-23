#include "app_button.h"
#include "button.h"
#include "app_motor.h"
#include "app_control.h"

static Button_TypeDef userKey;  //用户按钮

static void OnUserKeyClicked(uint8_t clicks);

//
//@简介： 初始化用户按钮
//
void App_Button_Init(void)
{
    Button_InitTypeDef Button_InitStruct = {0}; //用户按钮初始化结构体
    Button_InitStruct.GPIOx = GPIOA; //用户按钮连接到GPIOA
    Button_InitStruct.GPIO_Pin = GPIO_Pin_11; //用户按钮连接到GPIOA的第11号引脚

    My_Button_Init(&userKey, &Button_InitStruct); //初始化用户按钮

    My_Button_SetClickCb(&userKey, OnUserKeyClicked); //设置用户按钮的点击回调函数
}

void App_Button_Proc(void)
{
    My_Button_Proc(&userKey); //处理用户按钮的状态
}

static uint8_t pwm_on = 0; //PWM状态标志，0表示关闭，1表示开启

//
//@简介： 用户按钮点击回调函数
//
static void OnUserKeyClicked(uint8_t clicks)
{
    if (clicks == 1)
    {
        App_Control_Reset(); //重置控制模块的状态

        //翻转电机状态
        if (pwm_on == 0)
        {
            pwm_on = 1; //更新状态标志
        }
        else
        {
            pwm_on = 0; //更新状态标志
        }
        App_Motor_Cmd(pwm_on); //根据状态标志控制电机
    }
}


#include "app_motor.h"
#include "pid.h"
#include "task.h"
#include "app_encoder.h"
#include "app_bat.h"
#include "app_pwm.h"

static PID_TypeDef pid_motor_l; // 左电机PID控制器
static PID_TypeDef pid_motor_r; // 右电机PID控制器

//
//@简介: 初始化左右电机调速的PID控制器
//
void App_Motor_Init(void)
{
    PID_Init(&pid_motor_l, 0.5, 7, 0); // 初始化左电机PID控制器
    PID_LimitConfig(&pid_motor_l, 8.4f, -8.4f); // 设置左电机PID控制器的输出限制
    PID_Init(&pid_motor_r, 0.5, 7, 0); // 初始化右电机PID控制器
    PID_LimitConfig(&pid_motor_r, 8.4f, -8.4f); // 设置右电机PID控制器的输出限制
}

//
//@简介: 左右电机调速的任务切片(进程)函数
//
void App_Motor_Proc(void)
{
    PERIODIC(1)  // 每1ms执行一次
    
    //1. 获取左右电机旋转的角速度
    float omega_l = App_Encoder_GetSpeed_L(); // 获取左轮胎的角速度
    float omega_r = App_Encoder_GetSpeed_R(); // 获取右轮胎的角速度

    //2. 计算左右电机的PID控制器输出值
    float ua_l = PID_Compute(&pid_motor_l, omega_l); // 计算左电机的输出值
    float ua_r = PID_Compute(&pid_motor_r, omega_r); // 计算右电机的输出值

    //3. 将电压Ua设置到电机两端
    float vbat = App_Bat_Get(); // 获取电池电压

    //计算左右电机的占空比
    float duty_l = ua_l / vbat * 100.0f; // 计算左电机的占空比
    float duty_r = ua_r / vbat * 100.0f; // 计算右电机的占空比

    App_PWM_Set_L(duty_l); // 设置左电机的占空比
    App_PWM_Set_R(duty_r); // 设置右电机的占空比
}

//
//@简介: 设置左电机的目标角速度
//@参数: omega: 目标角速度，单位是rad/s
//
void App_Motor_SetOmega_L(float omega)
{
    PID_ChangeSP(&pid_motor_l, omega); // 设置左电机PID控制器的设定值
}

//
//@简介: 设置右电机的目标角速度
//@参数: omega: 目标角速度，单位是rad/s
//
void App_Motor_SetOmega_R(float omega)
{
    PID_ChangeSP(&pid_motor_r, omega); // 设置右电机PID控制器的设定值
}

//
//@简介: 控制电机的开关状态
//@参数: on: 1表示开启电机，0表示关闭电机
//
void App_Motor_Cmd(uint8_t on)
{
    App_PWM_Cmd(on); // 控制PWM输出的开关状态
    PID_Reset(&pid_motor_l); // 重置左电机PID控制器的状态变量
    PID_Reset(&pid_motor_r); // 重置右电机PID控制器的状态变量
}

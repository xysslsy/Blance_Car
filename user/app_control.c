#include "app_control.h"
#include "pid.h"
#include "task.h"
#include "app_mpu6050.h"
#include "qmath.h"
#include "app_motor.h"

static PID_TypeDef pid_theta; //theta环的PID结构体
static PID_TypeDef pid_theta_dot; //theta_dot环的PID结构体

static const float g = 9.81f; //重力加速度 m/s^2
static const float lp = 0.062f; //小车的质心到轮轴的距离 m
static const float rw = 0.032f; //轮胎半径 m

//
//@简介: 初始化控制模块
//
void App_Control_Init(void)
{
    PID_Init(&pid_theta, 4.0f, 0.0f, 0.0f); //初始化theta环的PID结构体
    PID_LimitConfig(&pid_theta, +12.57f, -12.57f); //设置theta环的PID输出上限和下限 rad/s

    PID_Init(&pid_theta_dot, 10.0f, 10.0f, 0.0f); //初始化theta_dot环的PID结构体
    PID_LimitConfig(&pid_theta_dot, +125.7f, -125.7f); //设置theta_dot环的PID输出上限和下限 rad/s^2
}

static float omega_ref = 0.0f; //轮胎的角速度 rad/s
static uint64_t last_time = 0; //上一次计算的时间戳

//
//@简介: 控制模块的任务切片,在while循环调用
//
void App_Control_Proc(void)
{
    PERIODIC(5) //每5ms执行一次

    uint64_t now = GetUs();  //获取当前的微秒级时间
    float deltaT = (now - last_time) * 1.0e-6f; //计算时间间隔 s

    //1.将外环的设定值SP设置为0.0rad,即保持小车直立
    PID_ChangeSP(&pid_theta, 0.0f); //设置theta环的PID设定值为0.0rad

    //2.读取传感器的数据
    float theta = App_MPU6050_GetPitch() * 0.01745329; //获取小车的倾角theta(rad)°转为rad的系数为0.01745329
    float theta_dot = App_MPU6050_GetGx() * 0.01745329; //获取小车的角速度theta_dot(rad/s)

    //3.计算外环的PID输出,即theta_dot的设定值
    float theta_dot_ref = PID_Compute(&pid_theta, theta); //计算theta环的PID输出,即theta_dot的设定值

    //4.改变内环的设定值SP
    PID_ChangeSP(&pid_theta_dot, theta_dot_ref); //设置theta_dot环的PID设定值为theta_dot_ref

    //5.计算内环的PID输出,即电机的目标角加速度
    float theta_dot_dot_ref = PID_Compute(&pid_theta_dot, theta_dot); //计算theta_dot环的PID输出,即电机的目标角加速度

    //6.倒立摆的逆解散
    float x_dot_dot_ref = (g * qsin(theta) - theta_dot_dot_ref * lp) / qcos(theta); //计算小车的目标线加速度

    //7.计算轮胎转速
    omega_ref += 1.0f / rw * x_dot_dot_ref * deltaT; //计算轮胎的目标角速度 rad/s

    last_time = now; //更新上一次计算的时间戳

    //8.将轮胎的目标角速度输出到电机驱动模块
    App_Motor_SetOmega_L(omega_ref);
    App_Motor_SetOmega_R(omega_ref);
}

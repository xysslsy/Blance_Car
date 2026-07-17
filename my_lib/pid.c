#include "pid.h"
#include "delay.h"

//
//@简介: PID控制器初始化函数
//@参数：Kp: 比例系数, Ki: 积分系数, Kd: 微分系数
//
void PID_Init(PID_TypeDef *PID, float Kp, float Ki, float Kd)
{
    PID->Kp = Kp;
    PID->Ki = Ki;
    PID->Kd = Kd;
    PID->SP = 0.0f; // 初始化设定值为0

    PID->t_k_1 = 0; // 初始化上一次计算的时间戳为0
    PID->err_k_1 = 0.0f; // 初始化上一次计算的误差值为0
    PID->err_int_k_1 = 0.0f; // 初始化上一次计算的积分值为0
}

//
//@简介: PID控制器设定值修改函数
//@参数：SP: 新的设定值
//
void PID_ChangeSP(PID_TypeDef *PID, float SP)
{
    PID->SP = SP;
}

//
//@简介: PID控制器计算函数
//@参数：FB: 当前反馈值
//@返回值：控制器输出值
//
float PID_Compute(PID_TypeDef *PID, float FB)
{
    float err = PID->SP - FB;

    uint64_t t_k = GetUs(); // 获取当前时间戳（微秒）
    float deltaT = (t_k - PID->t_k_1) * 1.0e-6f; // 计算时间间隔（秒）
    float err_dev = (err - PID->err_k_1) / deltaT; // 计算误差变化量
    float err_int = PID->err_int_k_1 + (err + PID->err_k_1) * deltaT / 2.0f; // 计算误差积分值

    float COp = PID->Kp * err; // 比例项
    float COi = PID->Ki * err_int; // 积分项
    float COd = PID->Kd * err_dev; // 微分项
    float CO = COp + COi + COd; // PID输出

    // 更新状态变量
    PID->t_k_1 = t_k;
    PID->err_k_1 = err;
    PID->err_int_k_1 = err_int;

    return CO;
}

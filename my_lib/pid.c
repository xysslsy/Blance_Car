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

    PID->UpperLimit = +3.4e+38f; // 设置输出上限
    PID->LowerLimit = -3.4e+38f; // 设置输出下限
}

//
//@简介: 设置PID控制器的输出限制
//@参数：Upper: 输出上限, Lower: 输出下限
//
void PID_LimitConfig(PID_TypeDef *PID, float Upper, float Lower)
{
    PID->UpperLimit = Upper;
    PID->LowerLimit = Lower;
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

    float err_dev = 0.0f; // 误差变化量
    float err_int = 0.0f; // 误差积分值

    if(PID->t_k_1 != 0) // 如果不是第一次计算
    {
        err_dev = (err - PID->err_k_1) / deltaT; // 计算误差变化量
        err_int = PID->err_int_k_1 + (err + PID->err_k_1) * deltaT / 2.0f; // 计算误差积分值
    }

    float COp = PID->Kp * err; // 比例项
    float COi = PID->Ki * err_int; // 积分项
    float COd = PID->Kd * err_dev; // 微分项
    float CO = COp + COi + COd; // PID输出

    // 更新状态变量
    PID->t_k_1 = t_k;
    PID->err_k_1 = err;
    PID->err_int_k_1 = err_int;

    // 限制PID输出值在设定的上下限之间
    if(CO > PID->UpperLimit) // 输出超过上限
    {
        CO = PID->UpperLimit; // 限制输出为上限值
    }
    else if(CO < PID->LowerLimit) // 输出低于下限
    {
        CO = PID->LowerLimit; // 限制输出为下限值
    }

    // 更新积分值限制
    if(PID->err_int_k_1 > PID->UpperLimit) // 积分值超过上限
    {
        PID->err_int_k_1 = PID->UpperLimit; // 限制积分值为上限值
    }
    else if(PID->err_int_k_1 < PID->LowerLimit) // 积分值低于下限
    {
        PID->err_int_k_1 = PID->LowerLimit; // 限制积分值为下限值
    }

    return CO;
}

//
//@简介: 重置PID控制器的状态变量
//
void PID_Reset(PID_TypeDef *PID)
{
    PID->t_k_1 = 0; // 初始化上一次计算的时间戳为0
    PID->err_k_1 = 0.0f; // 初始化上一次计算的误差值为0
    PID->err_int_k_1 = 0.0f; // 初始化上一次计算的积分值为0
}

#ifndef PID_H
#define PID_H

#include "stm32f10x.h"

typedef struct {
    float Kp; // 比例系数
    float Ki; // 积分系数
    float Kd; // 微分系数
    float SP; // 设定值

    uint64_t t_k_1; // 上一次计算的时间戳
    float err_k_1; // 上一次计算的误差值
    float err_int_k_1; // 上一次计算的积分值
} PID_TypeDef;

void PID_Init(PID_TypeDef *PID, float Kp, float Ki, float Kd);
void PID_ChangeSP(PID_TypeDef *PID, float SP);
float PID_Compute(PID_TypeDef *PID, float FB); 

#endif // PID_H

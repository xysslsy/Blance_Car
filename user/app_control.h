#ifndef APP_CONTROL_H
#define APP_CONTROL_H

#include "stm32f10x.h"

void App_Control_Init(void); //初始化控制模块
void App_Control_Proc(void); //控制模块的任务切片
void App_Control_Reset(void); //重置控制模块的状态
void App_Control_SetMoveSpeed(float MoveSpeed); //设置移动速度
void App_Control_SetTurnSpeed(float TurnSpeed); //设置转向速度

#endif

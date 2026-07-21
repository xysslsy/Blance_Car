#include "stm32f10x.h"
#include "bat_test.h"
#include "app_bat.h"
#include "app_button.h"
#include "app_pwm.h"
#include "pwm_test.h"
#include "encoder_test.h"
#include "mpu6050_test.h"
#include "qmath_test.h"
#include "app_encoder.h"
#include "app_motor.h"
#include "delay.h"
#include "app_usart2.h"
#include "task.h"

static float targetOmega;
static void USART2_Proc(void);

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); //设置中断优先级分组为0

	//QMath_Test(); //测试数学运算的速度
	//MPU6050_Euler_Angle_Test(); //测试MPU6050欧拉角计算
	//MPU6050_Test(); //测试MPU6050模块
	//Encoder_T_Method_Test(); //测试编码器的T法测速
	//Encoder_M_Method_Test(); //测试编码器的M法测速
	//Encoder_Test(); //测试编码器模块
	//Bat_Test(); //测试电池电压检测模块
	//PWM_Test(); //测试PWM输出

	App_USART2_Init(); //初始化串口2模块
	App_Button_Init(); //初始化按钮模块
	App_Encoder_Init(); //初始化编码器模块
	App_PWM_Init(); //初始化PWM模块 (TB6612)
	App_Bat_Init(); //初始化电池电压检测模块
	App_Motor_Init(); //初始化电机模块 调速系统

	while(1)
	{
		targetOmega = (GetTick() / 1000) % 10 * 2.0f; //目标角速度在0~18rad/s之间变化

		App_Motor_SetOmega_L(targetOmega); //设置左电机的目标角速度
		App_Motor_SetOmega_R(targetOmega); //设置右电机的目标角速度

		App_Button_Proc(); //按钮模块的任务切片
		App_Bat_Proc(); //电池电压检测模块的任务切片
		App_Motor_Proc(); //电机模块的任务切片
		USART2_Proc(); //串口2模块的任务切片 
	}
}

static void USART2_Proc(void)
{
	PERIODIC(10) //每10ms执行一次

	float omega_l = App_Encoder_GetSpeed_L(); //获取左轮胎的角速度
	float omega_r = App_Encoder_GetSpeed_R(); //获取右轮胎的角速度

	My_USART_Printf(USART2, "%.3f, %.3f, %.3f\n", targetOmega, omega_l, omega_r); //打印目标角速度和左右轮胎的角速度
}

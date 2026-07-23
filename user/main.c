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
#include "app_control.h"
#include "app_mpu6050.h"
#include "app_rc.h"


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
	App_MPU6050_Init(); //初始化MPU6050模块
	App_Control_Init(); //初始化控制模块
	App_RC_Init(); //初始化遥控模块

	while(1)
	{
		App_Button_Proc(); //按钮模块的任务切片
		App_Bat_Proc(); //电池电压检测模块的任务切片
		App_Motor_Proc(); //电机模块的任务切片
		App_MPU6050_Proc(); //MPU6050模块的任务切片ks
		App_Control_Proc(); //控制模块的任务切片
		App_RC_Proc(); //遥控模块的任务切片
	}
}

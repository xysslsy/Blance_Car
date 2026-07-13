#include "stm32f10x.h"
#include "bat_test.h"
#include "app_bat.h"
#include "app_button.h"
#include "app_pwm.h"
#include "pwm_test.h"
#include "encoder_test.h"
#include "mpu6050_test.h"
#include "qmath_test.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); //设置中断优先级分组为0

	QMath_Test(); //测试数学运算的速度

	//MPU6050_Euler_Angle_Test(); //测试MPU6050欧拉角计算

	//MPU6050_Test(); //测试MPU6050模块

	//Encoder_T_Method_Test(); //测试编码器的T法测速

	//Encoder_M_Method_Test(); //测试编码器的M法测速

	//Encoder_Test(); //测试编码器模块

	//Bat_Test(); //测试电池电压检测模块

	//PWM_Test(); //测试PWM输出

	App_Bat_Init(); //初始化电池电压检测模块
	App_Button_Init(); //初始化按钮模块
	App_PWM_Init(); //初始化PWM模块

	while(1)
	{
		App_Bat_Proc(); //电池电压检测模块的任务切片
		App_Button_Proc(); //按钮模块的任务切片
	}
}

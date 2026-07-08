#include "stm32f10x.h"
#include "bat_test.h"
#include "app_bat.h"
#include "app_button.h"
#include "app_pwm.h"
#include "pwm_test.h"
#include "encoder_test.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); //设置中断优先级分组为0

	Encoder_Test(); //测试编码器模块

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

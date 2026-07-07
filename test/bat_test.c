#include "bat_test.h"
#include "app_usart2.h"
#include "app_bat.h"
#include "delay.h"

//
// @简介：对电池电压检测模块进行测试
// 通过串口2把电压发送给vofa显示曲线
//
void Bat_Test(void)
{
    App_USART2_Init(); //初始化USART2
    App_Bat_Init(); //初始化电池电压检测模块

    while (1)
    {
        //每间隔10ms显示一个数据点
        float volt = App_Bat_Get(); //获取电池电压值

        My_USART_Printf(USART2, "%.3f\n", volt); //通过串口2发送电池电压值

        Delay(10);
    }
}

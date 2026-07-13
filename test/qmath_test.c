#include "qmath_test.h"
#include "app_usart2.h"
#include "delay.h"

//
//@简介：对基本运算的运算速度测试
//整数的+ - *  % 运算速度测试
//浮点数的+ - *  / 运算速度测试
//三角函数的运算速度测试
//
void QMath_Test(void)
{
    App_USART2_Init();

    //@简介：整数的+ - *  % 运算速度测试
    My_USART_Printf(USART2, "\n整数的+运算速度测试:");

    int a = 1, b = 2, c;

    uint32_t t1 = GetTick();

    for(uint32_t i = 0; i < 1000000; i++)
    {
        c = a + b;
    }

    uint32_t t2 = GetTick();

    (void)c; //避免编译器优化掉c的计算

    My_USART_Printf(USART2, "耗时: %.3f us\n", (t2 - t1) / 1000000.0f * 1000.0f);

    while(1)
    {

    }
}

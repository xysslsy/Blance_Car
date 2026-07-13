#include "mpu6050_test.h"
#include "app_usart2.h"
#include "app_mpu6050.h"
#include "delay.h"
#include "task.h"

void MPU6050_Test(void)
{
    App_USART2_Init();
    App_MPU6050_Init();

    while(1)
    {
        App_MPU6050_Update();

        float ax = App_MPU6050_GetAx();
        float ay = App_MPU6050_GetAy();
        float az = App_MPU6050_GetAz();

        float temperature = App_MPU6050_GetTemperature();

        float gx = App_MPU6050_GetGx();
        float gy = App_MPU6050_GetGy();
        float gz = App_MPU6050_GetGz();

        // 通过USART2发送数据
        My_USART_Printf(USART2, "%f,%f,%f,%f,%f,%f,%f\n", ax, ay, az, temperature, gx, gy, gz);

        Delay(10);
    }
}

static void USART2_Proc(void);

//
//@简介：MPU6050欧拉角测试函数
//
void MPU6050_Euler_Angle_Test(void)
{
    App_MPU6050_Init();
    App_USART2_Init();

    while(1)
    {
        App_MPU6050_Proc(); //调用MPU6050进程函数
        USART2_Proc(); //调用USART2进程函数
    }
}

//
//@简介：每间隔10ms吧欧拉角的计算结果发给电脑
//
static void USART2_Proc(void)
{
    PERIODIC(10) //每10ms执行一次

    float ax = App_MPU6050_GetAx();
    float ay = App_MPU6050_GetAy();
    float az = App_MPU6050_GetAz();

    float temperature = App_MPU6050_GetTemperature();

    float gx = App_MPU6050_GetGx();
    float gy = App_MPU6050_GetGy();
    float gz = App_MPU6050_GetGz();

    float roll = App_MPU6050_GetRoll();
    float pitch = App_MPU6050_GetPitch();
    float yaw = App_MPU6050_GetYaw();

    My_USART_Printf(USART2, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", ax, ay, az, temperature, gx, gy, gz, yaw, pitch, roll);
}

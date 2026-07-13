#include "app_mpu6050.h"
#include "i2c.h"
#include "delay.h"
#include "task.h"
#include "math.h"
#include "qmath.h"

static float ax, ay, az; //加速度计的值,单位为g
static float temperature; //温度的值,单位为℃
static float gx, gy, gz; //陀螺仪的值,单位为°/s
static float roll, pitch, yaw; //欧拉角的值,单位为°

static void reg_write(uint8_t reg, uint8_t value);
static uint8_t reg_read(uint8_t reg);

//
//@简介：MPU6050初始化
//
void App_MPU6050_Init(void)
{
    //1.初始化I2C PB8 PB9 - I2C1
    //将I2C1的引脚重映射PB8 PB9
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
    
    //初始化PB8 PB9为AF_OD 复用开漏输出
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 

    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //2.初始化I2C1
    //开启I2C1时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    //设置I2C的参数
    I2C_InitTypeDef I2C_InitStructure = {0};

    I2C_InitStructure.I2C_ClockSpeed = 400000; //400kHz
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;

    I2C_Init(I2C1, &I2C_InitStructure);

    //3.设置MPU6050的参数
    reg_write(0x6b, 0x80); //复位MPU6050
    Delay(100); //等待复位完成

    reg_write(0x6b, 0x00); //唤醒MPU6050

    reg_write(0x1b, 0x18); //设置陀螺仪量程为±2000°/s
    reg_write(0x1c, 0x00); //设置加速度计量程为±2g
}

//
//@简介：MPU6050进程函数
//
void App_MPU6050_Proc(void)
{
    PERIODIC(5) //每5ms执行一次

    App_MPU6050_Update(); //更新MPU6050的数据

    //通过陀螺仪的测量结果测算欧拉角
    float yaw_g = yaw + gz * 0.005;
    float pitch_g = pitch + gx * 0.005;
    float roll_g = roll - gy * 0.005;

    //通过加速度g解算欧拉角
    float pitch_a = qatan2(ay, az) / 3.1415927f * 180.0f;
    float roll_a = qatan2(ax, az) / 3.1415927f * 180.0f;

    //通过互补滤波融合陀螺仪和加速度计的测量结果
    yaw = yaw_g;
    pitch = 0.95238 * pitch_g + (1 - 0.95238) * pitch_a;
    roll = 0.95238 * roll_g + (1 - 0.95238) * roll_a;

}

//
//@简介：MPU6050更新数据
//
void App_MPU6050_Update(void)
{
    int16_t ax_raw = (int16_t)((reg_read(0x3b) << 8) + reg_read(0x3c)); //ax的原始值
    int16_t ay_raw = (int16_t)((reg_read(0x3d) << 8) + reg_read(0x3e)); //ay的原始值
    int16_t az_raw = (int16_t)((reg_read(0x3f) << 8) + reg_read(0x40)); //az的原始值

    ax = ax_raw * 6.1035e-5f; //将原始值转换为g
    ay = ay_raw * 6.1035e-5f; //将原始值转换为g
    az = az_raw * 6.1035e-5f; //将原始值转换为g

    int16_t temperature_raw = (int16_t)((reg_read(0x41) << 8) + reg_read(0x42)); //温度的原始值

    temperature = temperature_raw / 333.87f + 21.0f; //将原始值转换为℃，6500芯片公式

    int16_t gx_raw = (int16_t)((reg_read(0x43) << 8) + reg_read(0x44)); //gx的原始值
    int16_t gy_raw = (int16_t)((reg_read(0x45) << 8) + reg_read(0x46)); //gy的原始值
    int16_t gz_raw = (int16_t)((reg_read(0x47) << 8) + reg_read(0x48)); //gz的原始值

    gx = gx_raw * 6.1035e-2f; //将原始值转换为°/s
    gy = gy_raw * 6.1035e-2f; //将原始值转换为°/s
    gz = gz_raw * 6.1035e-2f; //将原始值转换为°/s
}

//
//@简介：获取加速度计的X轴值
//@返回值：加速度计的X轴值，单位为g
//
float App_MPU6050_GetAx(void)
{
    return ax;
}

//
//@简介：获取加速度计的Y轴值
//@返回值：加速度计的Y轴值，单位为g
//
float App_MPU6050_GetAy(void)
{
    return ay;
}

//
//@简介：获取加速度计的Z轴值
//@返回值：加速度计的Z轴值，单位为g
//
float App_MPU6050_GetAz(void)
{
    return az;
}

//
//@简介：获取温度值
//@返回值：温度值，单位为℃
//
float App_MPU6050_GetTemperature(void)
{
    return temperature;
}

//
//@简介：获取陀螺仪的X轴值
//@返回值：陀螺仪的X轴值，单位为°/s
//
float App_MPU6050_GetGx(void)
{
    return gx;
}

//
//@简介：获取陀螺仪的Y轴值
//@返回值：陀螺仪的Y轴值，单位为°/s
//
float App_MPU6050_GetGy(void)
{
    return gy;
}

//
//@简介：获取陀螺仪的Z轴值
//@返回值：陀螺仪的Z轴值，单位为°/s
//
float App_MPU6050_GetGz(void)
{
    return gz;
}

//
//@简介：获取欧拉角的Roll值
//@返回值：欧拉角的Roll值，单位为°
//
float App_MPU6050_GetRoll(void)
{
    return roll;
}

//
//@简介：获取欧拉角的Pitch值
//@返回值：欧拉角的Pitch值，单位为°
//
float App_MPU6050_GetPitch(void)
{
    return pitch;
}

//
//@简介：获取欧拉角的Yaw值
//@返回值：欧拉角的Yaw值，单位为°
//
float App_MPU6050_GetYaw(void)
{
    return yaw;
}

//
//@简介：向MPU6050寄存器写入数据
//@参数：reg - 寄存器地址，value - 要写入的值
//
static void reg_write(uint8_t reg, uint8_t value)
{
    uint8_t bytesToSend[] = {reg, value};

    My_I2C_SendBytes(I2C1, 0xd0, bytesToSend, 2);
}

//
//@简介：从MPU6050寄存器读取数据
//@参数：reg - 寄存器地址
//@返回值：寄存器的值
//
static uint8_t reg_read(uint8_t reg)
{
    My_I2C_SendBytes(I2C1, 0xd0, &reg, 1);   //发送寄存器地址

    uint8_t regValue;

    My_I2C_ReceiveBytes(I2C1, 0xd0, &regValue, 1); //读取一个字节
    return regValue;
}

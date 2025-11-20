#include "zf_common_headfile.h"  // 包含必要的头文件

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IMU660数据调试输出函数
// 参数说明     void
// 返回参数     void
// 使用示例     IMU660_Debug();
// 备注信息     通过无线串口发送IMU660的角速度、加速度和滤波后的角度数据，以逗号分隔，最后换行
//-------------------------------------------------------------------------------------------------------------------
void IMU660_Debug()
{
    wireless_uart_send_decimal(imu660ra_gyro_x);  // 发送X轴角速度值
    wireless_uart_send_string(",");               // 发送分隔符
    wireless_uart_send_decimal(imu660ra_gyro_y);  // 发送Y轴角速度值
    wireless_uart_send_string(",");               // 发送分隔符
    wireless_uart_send_decimal(imu660ra_gyro_z);  // 发送Z轴角速度值
    wireless_uart_send_string(",");               // 发送分隔符
    wireless_uart_send_decimal(imu660ra_acc_x);   // 发送X轴加速度值
    wireless_uart_send_string(",");               // 发送分隔符
    wireless_uart_send_decimal(imu660ra_acc_y);   // 发送Y轴加速度值
    wireless_uart_send_string(",");               // 发送分隔符
    wireless_uart_send_decimal(imu660ra_acc_z);   // 发送Z轴加速度值
    wireless_uart_send_string(",");               // 发送分隔符
    wireless_uart_send_decimal((int32)(Angle_Forward.filtering_angle));  // 发送滤波后的角度值
    wireless_uart_send_string("\n");              // 发送换行符
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IMU660数据获取与预处理函数
// 参数说明     void
// 返回参数     void
// 使用示例     IMU660_GetData();
// 备注信息     获取IMU660原始数据并进行校准补偿、死区处理和平滑处理
//-------------------------------------------------------------------------------------------------------------------
void IMU660_GetData()
{
    imu660ra_get_acc();                                                         // 获取 IMU660RA 的加速度测量数值
    imu660ra_get_gyro();                                                        // 获取 IMU660RA 的角速度测量数值

    // 角速度数据校准补偿
    imu660ra_gyro_z += 3;                                                       // Z轴角速度补偿
    imu660ra_gyro_y -= 6;                                                       // Y轴角速度补偿

    // 角速度数据死区处理：小范围波动视为零
    if(imu660ra_gyro_y <= 5 && imu660ra_gyro_y >= -5) imu660ra_gyro_y = 0;      // Y轴角速度死区处理
    if(imu660ra_gyro_x <= 5 && imu660ra_gyro_x >= -5) imu660ra_gyro_x = 0;      // X轴角速度死区处理
    if(imu660ra_gyro_z <= 5 && imu660ra_gyro_z >= -5) imu660ra_gyro_z = 0;      // Z轴角速度死区处理

    // 角速度数据平滑处理：减小跳动
    if(imu660ra_gyro_y > 5) imu660ra_gyro_y -= 5;                               // Y轴正方向平滑处理
    else if(imu660ra_gyro_y < -5) imu660ra_gyro_y += 5;                         // Y轴负方向平滑处理

    if(imu660ra_gyro_x > 5) imu660ra_gyro_x -= 5;                               // X轴正方向平滑处理
    else if(imu660ra_gyro_x < -5) imu660ra_gyro_x += 5;                         // X轴负方向平滑处理

    if(imu660ra_gyro_z > 5) imu660ra_gyro_z -= 5;                               // Z轴正方向平滑处理
    else if(imu660ra_gyro_z < -5) imu660ra_gyro_z += 5;                         // Z轴负方向平滑处理
}

// 角度计算结构体：存储滤波参数和中间状态
cascade_common_value_struct Angle_Forward = {
    .gyro_ration = 4.0f,        // 角速度置信度系数（权重）
    .acc_ration = 4.0f,         // 加速度置信度系数（权重）
    .angle_temp = 0.0f,         // 初始角度值
    .call_cycle = 0.005f,       // 采样周期（5ms）
    .mechanical_zero = 660.0f,  // 机械零点偏移量
    .filtering_angle = 0.0f     // 初始滤波角度值
};

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     一阶互补滤波函数
// 参数说明     filter_value     滤波器参数结构体指针
// 参数说明     gyro_raw_data    原始角速度数据
// 参数说明     acc_raw_data     原始加速度数据
// 返回参数     void
// 使用示例     first_order_complementary_filtering(&Angle_Forward, imu660ra_gyro_x, imu660ra_acc_y);
// 备注信息     使用一阶互补滤波算法融合角速度和加速度数据，计算稳定角度
//-------------------------------------------------------------------------------------------------------------------
void first_order_complementary_filtering(cascade_common_value_struct *filter_value, int16 gyro_raw_data, int16 acc_raw_data)
{
    float gyro_temp;    // 角速度计算临时变量
    float acc_temp;     // 加速度计算临时变量

    // 角速度数据处理：乘以置信度系数
    gyro_temp = gyro_raw_data * filter_value->gyro_ration;

    // 加速度数据处理：计算与当前角度的差值并乘以置信度系数
    acc_temp = (acc_raw_data - filter_value->angle_temp) * filter_value->acc_ration;

    // 角度积分计算：融合角速度和加速度数据，乘以采样周期
    filter_value->angle_temp += ((gyro_temp + acc_temp) * filter_value->call_cycle);

    // 最终角度计算：加上机械零点偏移
    filter_value->filtering_angle = filter_value->angle_temp + filter_value->mechanical_zero;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     角度计算函数
// 参数说明     void
// 返回参数     void
// 使用示例     Angle_Calculation();
// 备注信息     使用X轴角速度和Y轴加速度计算前向角度
//-------------------------------------------------------------------------------------------------------------------
void Angle_Calculation()
{
    // 调用一阶互补滤波函数计算角度
    first_order_complementary_filtering(&Angle_Forward, imu660ra_gyro_x, imu660ra_acc_y);
}

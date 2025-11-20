#include "zf_common_headfile.h"  // 包含必要的头文件

//-------------------------------------------------------------------------------------------------------------------
// 角度计算结构体：存储滤波参数和中间状态
cascade_common_value_struct Angle_Forward = {
    .gyro_ration = 4.0f,        // 角速度置信度系数（权重）
    .acc_ration = 4.0f,         // 加速度置信度系数（权重）
    .angle_temp = 0.0f,         // 初始角度值
    .call_cycle = 0.005f,       // 采样周期（5ms）
    .mechanical_zero = 660.0f,  // 机械零点偏移量
    .filtering_angle = 0.0f     // 初始滤波角度值
};

// 偏航角计算结构体
cascade_common_value_struct Angle_Yaw = {
    .gyro_ration = 4.0f,        // 角速度置信度系数
    .acc_ration = 0.1f,         // 加速度置信度系数（偏航角主要依赖陀螺仪）
    .angle_temp = 0.0f,         // 初始偏航角值
    .call_cycle = 0.005f,       // 采样周期
    .mechanical_zero = 0.0f,    // 机械零点偏移
    .filtering_angle = 0.0f     // 滤波后偏航角
};

// 卡尔曼滤波结构体
typedef struct {
    float Q_angle;      // 过程噪声协方差
    float Q_gyro;       // 陀螺仪噪声协方差  
    float R_angle;      // 测量噪声协方差
    float x_bias;       // 陀螺仪偏差
    float P[2][2];      // 误差协方差矩阵
    float K[2];         // 卡尔曼增益
    float y;            // 残差
    float S;            // 创新协方差
} kalman_filter_struct;

kalman_filter_struct kalman_filter_forward;
kalman_filter_struct kalman_filter_yaw;

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     卡尔曼滤波初始化
// 参数说明     filter           卡尔曼滤波器结构体指针
// 返回参数     void
// 使用示例     kalman_filter_init(&kalman_filter_forward);
//-------------------------------------------------------------------------------------------------------------------
void kalman_filter_init(kalman_filter_struct *filter)
{
    filter->Q_angle = 0.001f;
    filter->Q_gyro = 0.003f;
    filter->R_angle = 5.0f;
    filter->x_bias = 0.0f;
    
    // 初始化误差协方差矩阵
    filter->P[0][0] = 0.0f;
    filter->P[0][1] = 0.0f;
    filter->P[1][0] = 0.0f;
    filter->P[1][1] = 0.0f;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     卡尔曼滤波更新
// 参数说明     filter           卡尔曼滤波器结构体指针
// 参数说明     newAngle         测量角度（加速度计计算）
// 参数说明     newRate          角速度测量值
// 参数说明     dt               采样时间
// 返回参数     float            滤波后的角度
// 使用示例     angle = kalman_filter_update(&kalman_filter_forward, acc_angle, gyro_rate, 0.005f);
//-------------------------------------------------------------------------------------------------------------------
float kalman_filter_update(kalman_filter_struct *filter, float newAngle, float newRate, float dt)
{
    // 预测步骤
    filter->x_bias = newRate - filter->x_bias;
    filter->P[0][0] += dt * (dt * filter->P[1][1] - filter->P[0][1] - filter->P[1][0] + filter->Q_angle);
    filter->P[0][1] -= dt * filter->P[1][1];
    filter->P[1][0] -= dt * filter->P[1][1];
    filter->P[1][1] += filter->Q_gyro * dt;
    
    // 更新步骤
    filter->S = filter->P[0][0] + filter->R_angle;
    filter->K[0] = filter->P[0][0] / filter->S;
    filter->K[1] = filter->P[1][0] / filter->S;
    
    filter->y = newAngle - filter->x_bias;
    filter->x_bias += filter->K[0] * filter->y;
    
    // 更新误差协方差矩阵
    float P00_temp = filter->P[0][0];
    float P01_temp = filter->P[0][1];
    
    filter->P[0][0] -= filter->K[0] * P00_temp;
    filter->P[0][1] -= filter->K[0] * P01_temp;
    filter->P[1][0] -= filter->K[1] * P00_temp;
    filter->P[1][1] -= filter->K[1] * P01_temp;
    
    return filter->x_bias;
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
// 函数简介     从加速度计计算角度
// 参数说明     acc_x            X轴加速度
// 参数说明     acc_y            Y轴加速度  
// 参数说明     acc_z            Z轴加速度
// 返回参数     float           计算出的角度（度）
// 使用示例     angle = calculate_angle_from_acc(imu660ra_acc_x, imu660ra_acc_y, imu660ra_acc_z);
//-------------------------------------------------------------------------------------------------------------------
float calculate_angle_from_acc(int16 acc_x, int16 acc_y, int16 acc_z)
{
    // 使用加速度计计算俯仰角（前向角度）
    float angle_forward = atan2f(acc_y, sqrtf(acc_x * acc_x + acc_z * acc_z)) * 180.0f / PI;
    
    return angle_forward;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     从加速度计计算偏航角
// 参数说明     acc_x            X轴加速度
// 参数说明     acc_y            Y轴加速度  
// 参数说明     acc_z            Z轴加速度
// 返回参数     float           计算出的偏航角（度）
// 使用示例     yaw = calculate_yaw_from_acc(imu660ra_acc_x, imu660ra_acc_y, imu660ra_acc_z);
//-------------------------------------------------------------------------------------------------------------------
float calculate_yaw_from_acc(int16 acc_x, int16 acc_y, int16 acc_z)
{
    // 使用加速度计计算偏航角（需要磁力计数据更准确，这里仅作演示）
    // 实际应用中偏航角主要依赖陀螺仪积分，加速度计用于校正
    float yaw = atan2f(acc_x, acc_z) * 180.0f / PI;
    
    return yaw;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     角度计算函数
// 参数说明     void
// 返回参数     void
// 使用示例     Angle_Calculation();
// 备注信息     计算前向角度和偏航角，并应用卡尔曼滤波
//-------------------------------------------------------------------------------------------------------------------
void Angle_Calculation()
{
    static uint8 init_flag = 0;
    
    // 初始化卡尔曼滤波器
    if(!init_flag)
    {
        kalman_filter_init(&kalman_filter_forward);
        kalman_filter_init(&kalman_filter_yaw);
        init_flag = 1;
    }
    
    // 计算前向角度（俯仰角）不使用卡尔曼滤波
    float acc_angle_forward = calculate_angle_from_acc(imu660ra_acc_x, imu660ra_acc_y, imu660ra_acc_z);
    
    // 使用互补滤波计算前向角度
    first_order_complementary_filtering(&Angle_Forward, imu660ra_gyro_x, imu660ra_acc_y);
    
 
      //6轴偏差过大，以弃用
//    // 计算偏航角
//    float acc_yaw = calculate_yaw_from_acc(imu660ra_acc_x, imu660ra_acc_y, imu660ra_acc_z);
//    
//    // 使用互补滤波计算偏航角（主要依赖陀螺仪Z轴）
//    first_order_complementary_filtering(&Angle_Yaw, imu660ra_gyro_z, 0); // 加速度计对偏航角贡献较小
//    
//    // 应用卡尔曼滤波到偏航角
//    Angle_Yaw.filtering_angle = kalman_filter_update(&kalman_filter_yaw, 
//                                                    acc_yaw, 
//                                                    imu660ra_gyro_z, 
//                                                    Angle_Yaw.call_cycle);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IMU660数据调试输出函数
// 参数说明     void
// 返回参数     void
// 使用示例     IMU660_Debug();
// 备注信息     通过无线串口发送IMU660的角速度、加速度和滤波后的角度数据，包含偏航角
//-------------------------------------------------------------------------------------------------------------------
void IMU660_Debug()
{
//    wireless_uart_send_decimal(imu660ra_gyro_x);  // 发送X轴角速度值
//    wireless_uart_send_string(",");               // 发送分隔符
//    wireless_uart_send_decimal(imu660ra_gyro_y);  // 发送Y轴角速度值
//    wireless_uart_send_string(",");               // 发送分隔符
//    wireless_uart_send_decimal(imu660ra_gyro_z);  // 发送Z轴角速度值
//    wireless_uart_send_string(",");               // 发送分隔符
//    wireless_uart_send_decimal(imu660ra_acc_x);   // 发送X轴加速度值
//    wireless_uart_send_string(",");               // 发送分隔符
//    wireless_uart_send_decimal(imu660ra_acc_y);   // 发送Y轴加速度值
//    wireless_uart_send_string(",");               // 发送分隔符
//    wireless_uart_send_decimal(imu660ra_acc_z);   // 发送Z轴加速度值分隔符

    wireless_uart_send_string("\n");              // 发送换行符
}

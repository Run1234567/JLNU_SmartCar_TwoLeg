#include "zf_common_headfile.h"  // 包含必要的头文件
#include "JLNU_imu963ar.h"

euler_angle_struct euler_angle;

typedef struct
{
    float Q_angle;      // 过程噪声协方差 - 角度
    float Q_gyro;       // 过程噪声协方差 - 角速度
    float R_angle;      // 测量噪声协方差
    float x_angle;      // 滤波后的角度（关键变量）
    float x_bias;       // 陀螺仪偏差
    float P_00, P_01, P_10, P_11;  // 误差协方差矩阵
    float K_0, K_1;     // 卡尔曼增益
    float dt;           // 采样时间
} kalman_filter_struct;

kalman_filter_struct roll_filter, pitch_filter;

// 地磁数据校准参数（需要根据实际测量确定）
typedef struct {
    float mag_offset[3];    // 地磁偏移量
    float mag_scale[3];     // 地磁缩放因子
} mag_calibration_struct;

mag_calibration_struct mag_calib;

uint32_t TIMER_FLAG = 0;



// 地磁传感器校准（需要在不同方向旋转设备）
void mag_calibration_init(void)
{
    // 初始化默认校准参数（需要实际测量校准）
    mag_calib.mag_offset[0] = 0;
    mag_calib.mag_offset[1] = 0;
    mag_calib.mag_offset[2] = 0;
    mag_calib.mag_scale[0] = 1.0f;
    mag_calib.mag_scale[1] = 1.0f;
    mag_calib.mag_scale[2] = 1.0f;
}

// 地磁数据预处理和校准
void mag_data_calibration(void)
{

    // 应用校准参数（需要预先校准）
    float mag_x = (float)imu963ra_mag_x * mag_calib.mag_scale[0] + mag_calib.mag_offset[0];
    float mag_y = (float)imu963ra_mag_y * mag_calib.mag_scale[1] + mag_calib.mag_offset[1];
    float mag_z = (float)imu963ra_mag_z * mag_calib.mag_scale[2] + mag_calib.mag_offset[2];

    // 存储校准后的数据
    imu963ra_mag_x = (int16_t)mag_x;
    imu963ra_mag_y = (int16_t)mag_y;
    imu963ra_mag_z = (int16_t)mag_z;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     从加速度计计算横滚角和俯仰角
// 参数说明     void
// 返回参数     void
// 使用示例     calculate_angle_from_acc2();
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
void calculate_angle_from_acc2(void)
{
    // 将原始数据转换为物理单位 (m/s2)
    float acc_x = (float)imu963ra_acc_x / imu963ra_transition_factor[0];
    float acc_y = (float)imu963ra_acc_y / imu963ra_transition_factor[0];
    float acc_z = (float)imu963ra_acc_z / imu963ra_transition_factor[0];

    // 计算横滚角 (绕X轴旋转)
    euler_angle.acc_roll = atan2f(acc_y, acc_z) * 57.29578f;  // 弧度转角度

    // 计算俯仰角 (绕Y轴旋转)
    euler_angle.acc_pitch = atan2f(-acc_x, sqrtf(acc_y*acc_y + acc_z*acc_z)) * 57.29578f;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     从陀螺仪计算角度
// 参数说明     void
// 返回参数     void
// 使用示例     calculate_angle_from_gyro();
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
void calculate_angle_from_gyro2(void)
{



    // 将原始数据转换为物理单位 (°/s)
    float gyro_x = (float)imu963ra_gyro_x / imu963ra_transition_factor[1];

    // 陀螺仪积分计算角度
    euler_angle.gyro_roll += gyro_x * 0.005f;//5ms间隔

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     卡尔曼滤波器初始化
// 参数说明     filter          滤波器结构体指针
// 参数说明     Q_angle         角度过程噪声
// 参数说明     Q_gyro          陀螺仪过程噪声
// 参数说明     R_angle         测量噪声
// 返回参数     void
// 使用示例     kalman_filter_init(&roll_filter, 0.001f, 0.003f, 0.5f);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void kalman_filter_init(kalman_filter_struct *filter, float Q_angle, float Q_gyro, float R_angle)
{
    filter->Q_angle = Q_angle;
    filter->Q_gyro = Q_gyro;
    filter->R_angle = R_angle;
    filter->x_angle = 0;      // 初始化角度为0
    filter->x_bias = 0;        // 初始化偏差为0
    filter->P_00 = 1;          // 修正：不能初始化为0
    filter->P_01 = 0;
    filter->P_10 = 0;
    filter->P_11 = 1;          // 修正：不能初始化为0
    filter->dt = 0.005f;
}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介     稳定的卡尔曼滤波角度计算
// 参数说明     filter          滤波器结构体指针
// 参数说明     acc_angle       加速度计角度（测量值）
// 参数说明     gyro_rate       陀螺仪角速度
// 返回参数     float            滤波后的角度
// 使用示例     filtered_roll = kalman_filter_calculate(&roll_filter, euler_angle.acc_roll, gyro_x);
//-------------------------------------------------------------------------------------------------------------------
float kalman_filter_calculate(kalman_filter_struct *filter, float acc_angle, float gyro_rate)
{
    // ==================== 预测步骤 ====================
    // 1. 预测角度：新角度 = 旧角度 + (陀螺仪读数 - 偏差) * 时间
    filter->x_angle += (gyro_rate - filter->x_bias) * filter->dt;

    // 2. 预测协方差矩阵
    filter->P_00 += filter->dt * (filter->dt * filter->P_11 - filter->P_01 - filter->P_10 + filter->Q_angle);
    filter->P_01 -= filter->dt * filter->P_11;
    filter->P_10 -= filter->dt * filter->P_11;
    filter->P_11 += filter->Q_gyro * filter->dt;

    // ==================== 更新步骤 ====================
    // 1. 计算卡尔曼增益
    float S = filter->P_00 + filter->R_angle;
    filter->K_0 = filter->P_00 / S;
    filter->K_1 = filter->P_10 / S;

    // 2. 计算测量残差
    float y = acc_angle - filter->x_angle;

    // 3. 状态更新（正确版本）
    filter->x_angle += filter->K_0 * y;
    filter->x_bias += filter->K_1 * y;

    // 4. 更新协方差矩阵
    filter->P_00 -= filter->K_0 * filter->P_00;
    filter->P_01 -= filter->K_0 * filter->P_01;
    filter->P_10 -= filter->K_1 * filter->P_00;
    filter->P_11 -= filter->K_1 * filter->P_01;

    return filter->x_angle;  // 返回角度，不是偏差
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     互补滤波计算欧拉角
// 参数说明     void
// 返回参数     void
// 使用示例     calculate_euler_angle();
// 备注信息     主函数中调用
//-------------------------------------------------------------------------------------------------------------------
void calculate_euler_angle(void)
{
    // 读取所有传感器数据
    imu963ra_get_acc();
    imu963ra_get_gyro();
    imu963ra_get_mag();  // 新增地磁数据读取

    // 地磁数据校准
    mag_data_calibration();

    // 从加速度计计算角度
    calculate_angle_from_acc2();

    // 从陀螺仪计算角度
    calculate_angle_from_gyro2();

    // 转换陀螺仪数据
    float gyro_x = (float)imu963ra_gyro_x / imu963ra_transition_factor[1];
    float gyro_y = (float)imu963ra_gyro_y / imu963ra_transition_factor[1];

    // 卡尔曼滤波融合横滚和俯仰
    euler_angle.roll = kalman_filter_calculate(&roll_filter, euler_angle.acc_roll, gyro_x);
    euler_angle.pitch = kalman_filter_calculate(&pitch_filter, euler_angle.acc_pitch, gyro_y);

    // 使用地磁改进的偏航角计算
    calculate_yaw_with_mag();



}

// 改进的偏航角计算函数
void calculate_yaw_with_mag(void)
{
    static float yaw_gyro = 0;
    static uint32 last_mag_update = 0;

    // 陀螺仪积分（短期精度）
    float gyro_z = (float)imu963ra_gyro_z / imu963ra_transition_factor[1];
    yaw_gyro += gyro_z * 0.005f;  // 5ms积分



    if(TIMER_FLAG >= 3)  // 每100ms更新一次地磁偏航
    {
        // 将加速度计数据转换为重力矢量
        float acc_x = (float)imu963ra_acc_x / imu963ra_transition_factor[0];
        float acc_y = (float)imu963ra_acc_y / imu963ra_transition_factor[0];
        float acc_z = (float)imu963ra_acc_z / imu963ra_transition_factor[0];

        // 归一化加速度矢量
        float acc_norm = sqrtf(acc_x*acc_x + acc_y*acc_y + acc_z*acc_z);
        acc_x /= acc_norm;
        acc_y /= acc_norm;
        acc_z /= acc_norm;

        // 计算横滚和俯仰角
        float roll = atan2f(acc_y, acc_z);
        float pitch = atan2f(-acc_x, sqrtf(acc_y*acc_y + acc_z*acc_z));



        // 地磁数据补偿倾斜
        float mag_x = (float)imu963ra_mag_x;
        float mag_y = (float)imu963ra_mag_y;
        float mag_z = (float)imu963ra_mag_z;

        // 倾斜补偿
        float mag_x_comp = mag_x * cosf(pitch) + mag_z * sinf(pitch);
        float mag_y_comp = mag_x * sinf(roll) * sinf(pitch) +
                          mag_y * cosf(roll) - mag_z * sinf(roll) * cosf(pitch);

        // 计算地磁偏航角
        float mag_yaw = atan2f(-mag_y_comp, mag_x_comp) * 57.29578f;

        // 互补滤波融合（地磁长期稳定 + 陀螺仪短期精确）
        float alpha = 0.55f;  // 融合系数，可调整
        euler_angle.yaw = (1 - alpha) * (euler_angle.yaw + gyro_z * 0.06f) +
                         alpha * mag_yaw;
    wireless_uart_send_string(",mag_yaw:");
    wireless_uart_send_decimal(mag_yaw);
    wireless_uart_send_string(",gyro_z:");
    wireless_uart_send_decimal(gyro_z);
    wireless_uart_send_string(",Yaw:");
    wireless_uart_send_decimal((int16)(euler_angle.yaw * 10));
    wireless_uart_send_string("\n");
        TIMER_FLAG=0;
    }
    // 偏航角范围限制
    if(euler_angle.yaw > 180.0f) euler_angle.yaw -= 360.0f;
    else if(euler_angle.yaw < -180.0f) euler_angle.yaw += 360.0f;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     初始化欧拉角计算
// 参数说明     void
// 返回参数     void
// 使用示例     euler_angle_init();
// 备注信息     系统启动时调用一次
//-------------------------------------------------------------------------------------------------------------------
void euler_angle_init(void)
{
    // 初始化卡尔曼滤波器
        kalman_filter_init(&roll_filter, 0.05f, 0.1f, 0.8f);
        kalman_filter_init(&pitch_filter, 0.05f, 0.1f, 0.8f);

    // 初始化地磁校准
    mag_calibration_init();

    // 初始角度校准
    imu963ra_get_acc();
    calculate_angle_from_acc2();

    euler_angle.roll = euler_angle.acc_roll;
    euler_angle.pitch = euler_angle.acc_pitch;
    euler_angle.yaw = 0;

}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取滤波后的欧拉角
// 参数说明     void
// 返回参数     void
// 使用示例     get_filtered_euler_angle();
// 备注信息     返回的角度存储在 euler_angle 结构体中
//-------------------------------------------------------------------------------------------------------------------
void get_filtered_euler_angle(void)
{

}

// 使用示例：
/*
// 系统初始化
euler_angle_init();

// 在主循环中调用
while(1)
{
    get_filtered_euler_angle();

    // 使用滤波后的角度
    // euler_angle.roll, euler_angle.pitch, euler_angle.yaw

    system_delay_ms(5);  // 5ms采样周期
}
*/

// 调试输出函数
void print_euler_angle(void)
{

    wireless_uart_send_string("Roll:");
    wireless_uart_send_decimal((int16)(euler_angle.roll * 10));  // 发送角度值，放大10倍提高精度
    wireless_uart_send_string(",Pitch:");
    wireless_uart_send_decimal((int16)(euler_angle.pitch * 10));
    wireless_uart_send_string(",Yaw:");
    wireless_uart_send_decimal((int16)(euler_angle.yaw * 10));
    wireless_uart_send_string("\n");
}

#include "zf_common_headfile.h"
#include <math.h>
void IMU963_Init()
{
    imu963ra_init();
}
void IMU963_Read_Data()
{
    imu963ra_get_acc();                                     // 获取 IMU963RA 加速度计数据
    imu963ra_get_gyro();                                     // 获取 IMU963RA 陀螺仪数据
    imu963ra_get_mag();                                     // 获取 IMU963RA 磁力计数据
    imu963ra_gyro_z=imu963ra_gyro_z+5;
    imu963ra_gyro_x-=3;
    imu963ra_gyro_y+=3;
    if(imu963ra_gyro_z<=7&&imu963ra_gyro_z>=-7)
    {
        imu963ra_gyro_z=0;
    }
 if(imu963ra_gyro_x<=5&&imu963ra_gyro_x>=-5)
    {
        imu963ra_gyro_x=0;
    }
     if(imu963ra_gyro_y<=5&&imu963ra_gyro_y>=-5)
    {
        imu963ra_gyro_y=0;
    }
}
// 定义互补滤波参数
#define Kp 2.0f  // 比例增益
#define Ki 0.1f  // 积分增益
#define Kp_mag 2.0f  // 磁力计比例增益
#define INTEGRAL_LIMIT 0.1f  // 积分项限幅值

// 全局变量用于误差积分
float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f;
float ezInt_mag = 0.0f;  // 磁力计误差积分

// 磁力计校准参数（需要根据实际校准结果修改）
float mag_offset_x = 0.0f, mag_offset_y = 0.0f, mag_offset_z = 0.0f;
float mag_scale_x = 1.0f, mag_scale_y = 1.0f, mag_scale_z = 1.0f;

// 地磁偏角（根据您的地理位置设置，单位：弧度）
// 例如，北京地区的地磁偏角约为-7°（西偏）
#define MAGNETIC_DECLINATION (-10.0f * DEG_TO_RAD)

void updateAttitude_2(void)
{
    // 读取陀螺仪数据并转换为弧度/秒
    float gx = imu963ra_gyro_transition(imu963ra_gyro_x) * DEG_TO_RAD;
    float gy = imu963ra_gyro_transition(imu963ra_gyro_y) * DEG_TO_RAD;
    float gz = imu963ra_gyro_transition(imu963ra_gyro_z) * DEG_TO_RAD;
    
    // 读取加速度数据并归一化
    float ax = imu963ra_acc_x;
    float ay = imu963ra_acc_y;
    float az = imu963ra_acc_z;
    
    // 加速度计归一化
    float norm = sqrtf(ax*ax + ay*ay + az*az);
    if(norm > 0.0f)
    {
        norm = 1.0f / norm;
        ax *= norm;
        ay *= norm;
        az *= norm;
    }
    
    // 读取磁力计数据并校准
    float mx = (imu963ra_mag_x - mag_offset_x) * mag_scale_x;
    float my = (imu963ra_mag_y - mag_offset_y) * mag_scale_y;
    float mz = (imu963ra_mag_z - mag_offset_z) * mag_scale_z;
    
    // 磁力计归一化
    norm = sqrtf(mx*mx + my*my + mz*mz);
    if(norm > 0.0f)
    {
        norm = 1.0f / norm;
        mx *= norm;
        my *= norm;
        mz *= norm;
    }
    
    float dt = SAMPLE_TIME_MS / 1000.0f;
    float q0 = attitude.q0, q1 = attitude.q1, q2 = attitude.q2, q3 = attitude.q3;
    
    // 从当前四元数计算重力方向（用于加速度计校正）
    float vx = 2.0f * (q1*q3 - q0*q2);
    float vy = 2.0f * (q0*q1 + q2*q3);
    float vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
    
    // 加速度计测量值与四元数推算值的误差（叉积）
    float ex = (ay*vz - az*vy);
    float ey = (az*vx - ax*vz);
    float ez = (ax*vy - ay*vx);
    
    // 误差积分
    exInt += ex * Ki * dt;
    eyInt += ey * Ki * dt;
    ezInt += ez * Ki * dt;
    
    // 积分项限幅
    if (exInt > INTEGRAL_LIMIT) exInt = INTEGRAL_LIMIT;
    if (exInt < -INTEGRAL_LIMIT) exInt = -INTEGRAL_LIMIT;
    if (eyInt > INTEGRAL_LIMIT) eyInt = INTEGRAL_LIMIT;
    if (eyInt < -INTEGRAL_LIMIT) eyInt = -INTEGRAL_LIMIT;
    if (ezInt > INTEGRAL_LIMIT) ezInt = INTEGRAL_LIMIT;
    if (ezInt < -INTEGRAL_LIMIT) ezInt = -INTEGRAL_LIMIT;
    
    // 磁力计误差计算
    // 将磁力计数据从机体坐标系转换到地球坐标系
    float hx = mx * (2.0f * (q0*q0 + q1*q1) - 1.0f) + 
               my * 2.0f * (q1*q2 + q0*q3) + 
               mz * 2.0f * (q1*q3 - q0*q2);
    
    float hy = mx * 2.0f * (q1*q2 - q0*q3) + 
               my * (2.0f * (q0*q0 + q2*q2) - 1.0f) + 
               mz * 2.0f * (q2*q3 + q0*q1);
    
    // 计算磁北方向（在水平面上的投影）
    float bx = sqrtf(hx*hx + hy*hy);
    float bz = 2.0f * (q1*q3 + q0*q2) * mx + 
               2.0f * (q2*q3 - q0*q1) * my + 
               (2.0f * (q0*q0 + q3*q3) - 1.0f) * mz;
    
    // 归一化地球坐标系下的磁力计向量水平分量
    norm = sqrtf(bx*bx + bz*bz);
    if(norm > 0.0f)
    {
        norm = 1.0f / norm;
        bx *= norm;
        bz *= norm;
    }
    
    // 计算期望的磁力计方向（在机体坐标系中）
    // 期望的磁场方向：X轴指向北，Z轴指向地磁方向
    float wx = bx;
    float wy = 0.0f;
    float wz = bz;
    
    // 将期望方向从地球坐标系转换回机体坐标系
    float mx_ref = wx * (2.0f * (q0*q0 + q1*q1) - 1.0f) + 
                   wy * 2.0f * (q1*q2 - q0*q3) + 
                   wz * 2.0f * (q1*q3 + q0*q2);
    
    float my_ref = wx * 2.0f * (q1*q2 + q0*q3) + 
                   wy * (2.0f * (q0*q0 + q2*q2) - 1.0f) + 
                   wz * 2.0f * (q2*q3 - q0*q1);
    
    float mz_ref = wx * 2.0f * (q1*q3 - q0*q2) + 
                   wy * 2.0f * (q2*q3 + q0*q1) + 
                   wz * (2.0f * (q0*q0 + q3*q3) - 1.0f);
    
    // 计算磁力计测量值与期望值的误差（叉积）
    float ex_mag = (my*mz_ref - mz*my_ref);
    float ey_mag = (mz*mx_ref - mx*mz_ref);
    float ez_mag = (mx*my_ref - my*mx_ref);
    
    // 磁力计误差积分
    ezInt_mag += ez_mag * Ki * dt;
    if (ezInt_mag > INTEGRAL_LIMIT) ezInt_mag = INTEGRAL_LIMIT;
    if (ezInt_mag < -INTEGRAL_LIMIT) ezInt_mag = -INTEGRAL_LIMIT;
    
    // 应用比例积分反馈修正陀螺仪数据
    // 加速度计校正俯仰和横滚，磁力计校正偏航
    gx = gx + Kp * ex + exInt;
    gy = gy + Kp * ey + eyInt;
    gz = gz + Kp * ez + ezInt + Kp_mag * ez_mag + ezInt_mag;
    
    // 四元数积分（修正后的陀螺仪数据）
    attitude.q0 += (-q1*gx - q2*gy - q3*gz) * 0.5f * dt;
    attitude.q1 += ( q0*gx - q3*gy + q2*gz) * 0.5f * dt;
    attitude.q2 += ( q3*gx + q0*gy - q1*gz) * 0.5f * dt;
    attitude.q3 += (-q2*gx + q1*gy + q0*gz) * 0.5f * dt;
    
    // 四元数归一化
    norm = sqrtf(attitude.q0*attitude.q0 + attitude.q1*attitude.q1 + 
                attitude.q2*attitude.q2 + attitude.q3*attitude.q3);
    if(norm > 0.0f) {
        norm = 1.0f / norm;
        attitude.q0 *= norm;
        attitude.q1 *= norm;
        attitude.q2 *= norm;
        attitude.q3 *= norm;
    }
    
    // 转换为欧拉角（相对于地球参考系）
    // 横滚角（围绕X轴旋转，相对于水平面）
    attitude.roll = atan2f(2*(attitude.q0*attitude.q1 + attitude.q2*attitude.q3), 
                         1 - 2*(attitude.q1*attitude.q1 + attitude.q2*attitude.q2)) * RAD_TO_DEG;
    
    // 俯仰角（围绕Y轴旋转，相对于水平面）
    attitude.pitch = asinf(2*(attitude.q0*attitude.q2 - attitude.q3*attitude.q1)) * RAD_TO_DEG;
    
    // 偏航角（围绕Z轴旋转，相对于正北方向）
    // 首先计算相对于磁北的偏航角
    float yaw_magnetic = atan2f(2*(attitude.q0*attitude.q3 + attitude.q1*attitude.q2), 
                              1 - 2*(attitude.q2*attitude.q2 + attitude.q3*attitude.q3));
    
    // 应用地磁偏角校正，得到相对于正北的偏航角
    attitude.yaw = (yaw_magnetic + MAGNETIC_DECLINATION) * RAD_TO_DEG;
    
    // 确保偏航角在0-360度范围内
    if (attitude.yaw < 0) attitude.yaw += 360.0f;
    if (attitude.yaw >= 360.0f) attitude.yaw -= 360.0f;
}

// 初始化函数，确保初始姿态正确
void initAttitude(void)
{
    // 使用加速度计和磁力计计算初始姿态
    float ax = imu963ra_acc_x;
    float ay = imu963ra_acc_y;
    float az = imu963ra_acc_z;
    
    // 加速度计归一化
    float norm = sqrtf(ax*ax + ay*ay + az*az);
    if(norm > 0.0f)
    {
        norm = 1.0f / norm;
        ax *= norm;
        ay *= norm;
        az *= norm;
    }
    
    // 计算初始俯仰和横滚
    float initial_roll = atan2f(ay, az);
    float initial_pitch = atan2f(-ax, sqrtf(ay*ay + az*az));
    
    // 使用磁力计计算初始偏航
    float mx = (imu963ra_mag_x - mag_offset_x) * mag_scale_x;
    float my = (imu963ra_mag_y - mag_offset_y) * mag_scale_y;
    float mz = (imu963ra_mag_z - mag_offset_z) * mag_scale_z;
    
    // 磁力计归一化
    norm = sqrtf(mx*mx + my*my + mz*mz);
    if(norm > 0.0f)
    {
        norm = 1.0f / norm;
        mx *= norm;
        my *= norm;
        mz *= norm;
    }
    
    // 计算初始偏航角
    float cos_roll = cosf(initial_roll);
    float sin_roll = sinf(initial_roll);
    float cos_pitch = cosf(initial_pitch);
    float sin_pitch = sinf(initial_pitch);
    
    float mx_hor = mx * cos_pitch + mz * sin_pitch;
    float my_hor = mx * sin_roll * sin_pitch + my * cos_roll - mz * sin_roll * cos_pitch;
    
    float initial_yaw = atan2f(-my_hor, mx_hor);
    
    // 应用地磁偏角校正
    initial_yaw += MAGNETIC_DECLINATION;
    
    // 从欧拉角计算初始四元数
    float cy = cosf(initial_yaw * 0.5f);
    float sy = sinf(initial_yaw * 0.5f);
    float cp = cosf(initial_pitch * 0.5f);
    float sp = sinf(initial_pitch * 0.5f);
    float cr = cosf(initial_roll * 0.5f);
    float sr = sinf(initial_roll * 0.5f);
    
    attitude.q0 = cr * cp * cy + sr * sp * sy;
    attitude.q1 = sr * cp * cy - cr * sp * sy;
    attitude.q2 = cr * sp * cy + sr * cp * sy;
    attitude.q3 = cr * cp * sy - sr * sp * cy;
}
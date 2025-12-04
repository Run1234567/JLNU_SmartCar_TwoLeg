#include "zf_common_headfile.h"
#include <math.h>

Attitude_t attitude = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
extKalman_t p;
void IMU660_Init()
{
   imu660ra_init();
   KalmanCreate(&p,10,100);
}

// 数据获取函数（保持不变）
void IMU660_GetData(void)
{
    imu660ra_get_acc();                                                         
    imu660ra_get_gyro();                                                        
imu660ra_gyro_z+=3;
    // 角速度数据校准补偿                                                       
    imu660ra_gyro_y -= 6;                                                       

    // 角速度数据死区处理
    if(imu660ra_gyro_y <= 5 && imu660ra_gyro_y >= -5) imu660ra_gyro_y = 0;      
    if(imu660ra_gyro_x <= 5 && imu660ra_gyro_x >= -5) imu660ra_gyro_x = 0;      
    if(imu660ra_gyro_z <= 10 && imu660ra_gyro_z >= -10) imu660ra_gyro_z = 0;      

    // // 角速度数据平滑处理
    if(imu660ra_gyro_y > 0) imu660ra_gyro_y -= 5;                               
    else if(imu660ra_gyro_y <0) imu660ra_gyro_y += 5;                         

    if(imu660ra_gyro_x > 0) imu660ra_gyro_x -= 5;                               
    else if(imu660ra_gyro_x < 0) imu660ra_gyro_x += 5;                         

    if(imu660ra_gyro_z > 0) imu660ra_gyro_z -= 5;                               
    else if(imu660ra_gyro_z < 0) imu660ra_gyro_z += 5;  
   imu660ra_gyro_z= KalmanFilter(&p,imu660ra_gyro_z);                       
    imu660ra_gyro_z=imu660ra_gyro_z/10*10;
    // 加速度数据平滑处理
//    imu660ra_acc_x = imu660ra_acc_x / 10 * 10;
//    imu660ra_acc_y = imu660ra_acc_y / 10 * 10;
//    imu660ra_acc_z = imu660ra_acc_z / 10 * 10; 
}

// 定义Mahony滤波器参数
#define KP 1.0f        // 比例增益
#define KI 0.003f      // 积分增益
#define INTEGRAL_LIMIT 0.1f  // 积分限幅

// 全局变量保存积分项
static float integralFBx = 0.0f;
static float integralFBy = 0.0f;
static float integralFBz = 0.0f;

void updateAttitude(void)
{
    // 读取陀螺仪数据并转换为弧度/秒
    float gx = imu660ra_gyro_transition(imu660ra_gyro_x) * DEG_TO_RAD;
    float gy = imu660ra_gyro_transition(imu660ra_gyro_y) * DEG_TO_RAD;
    float gz = imu660ra_gyro_transition(imu660ra_gyro_z) * DEG_TO_RAD;
    
    float dt = SAMPLE_TIME_MS / 1000.0f;
    float q0 = attitude.q0, q1 = attitude.q1, q2 = attitude.q2, q3 = attitude.q3;
    
    // Mahony滤波算法开始 ====================================
    
    // 1. 归一化加速度计数据
    float ax = imu660ra_acc_x;
    float ay = imu660ra_acc_y;
    float az = imu660ra_acc_z;
    
    float norm_acc = sqrtf(ax*ax + ay*ay + az*az);
    if (norm_acc > 0.0f) {
        ax /= norm_acc;
        ay /= norm_acc;
        az /= norm_acc;
        
        // 2. 从当前四元数中提取重力方向
        float vx = 2.0f * (q1*q3 - q0*q2);
        float vy = 2.0f * (q0*q1 + q2*q3);
        float vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
        
        // 3. 计算误差（叉积）
        float ex = (ay*vz - az*vy);
        float ey = (az*vx - ax*vz);
        float ez = (ax*vy - ay*vx);
        
        // 4. 积分误差（带限幅）
        integralFBx += KI * ex * dt;
        integralFBy += KI * ey * dt;
        integralFBz += KI * ez * dt;
        
        // 积分限幅
        if (integralFBx > INTEGRAL_LIMIT) integralFBx = INTEGRAL_LIMIT;
        if (integralFBx < -INTEGRAL_LIMIT) integralFBx = -INTEGRAL_LIMIT;
        if (integralFBy > INTEGRAL_LIMIT) integralFBy = INTEGRAL_LIMIT;
        if (integralFBy < -INTEGRAL_LIMIT) integralFBy = -INTEGRAL_LIMIT;
        if (integralFBz > INTEGRAL_LIMIT) integralFBz = INTEGRAL_LIMIT;
        if (integralFBz < -INTEGRAL_LIMIT) integralFBz = -INTEGRAL_LIMIT;
        
        // 5. 应用比例和积分反馈到陀螺仪数据
        gx += KP * ex + integralFBx;
        gy += KP * ey + integralFBy;
        gz += KP * ez + integralFBz;
    }
    
    // Mahony滤波算法结束 ====================================
    
    // 使用修正后的陀螺仪数据进行四元数积分
    attitude.q0 += (-q1*gx - q2*gy - q3*gz) * 0.5f * dt;
    attitude.q1 += ( q0*gx - q3*gy + q2*gz) * 0.5f * dt;
    attitude.q2 += ( q3*gx + q0*gy - q1*gz) * 0.5f * dt;
    attitude.q3 += (-q2*gx + q1*gy + q0*gz) * 0.5f * dt;
    
    // 四元数归一化
    float norm = sqrtf(attitude.q0*attitude.q0 + attitude.q1*attitude.q1 + 
                      attitude.q2*attitude.q2 + attitude.q3*attitude.q3);
    if(norm > 0.0f) 
    {
        norm = 1.0f / norm;
        attitude.q0 *= norm;
        attitude.q1 *= norm;
        attitude.q2 *= norm;
        attitude.q3 *= norm;
    }
    // 转换为欧拉角
    attitude.roll = atan2f(2*(attitude.q0*attitude.q1 + attitude.q2*attitude.q3), 
                          1 - 2*(attitude.q1*attitude.q1 + attitude.q2*attitude.q2)) * RAD_TO_DEG;
    attitude.pitch = asinf(2*(attitude.q0*attitude.q2 - attitude.q3*attitude.q1)) * RAD_TO_DEG;
    attitude.yaw = atan2f(2*(attitude.q0*attitude.q3 + attitude.q1*attitude.q2), 
                         1 - 2*(attitude.q2*attitude.q2 + attitude.q3*attitude.q3)) * RAD_TO_DEG;
}

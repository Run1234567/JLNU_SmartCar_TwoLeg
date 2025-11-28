#include "zf_common_headfile.h"
#include <math.h>

Attitude_t attitude = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
float acc_weight = 0.02f;  // 加速度计权重，用于互补滤波
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

    // 角速度数据平滑处理
    if(imu660ra_gyro_y > 5) imu660ra_gyro_y -= 5;                               
    else if(imu660ra_gyro_y < -5) imu660ra_gyro_y += 5;                         

    if(imu660ra_gyro_x > 5) imu660ra_gyro_x -= 5;                               
    else if(imu660ra_gyro_x < -5) imu660ra_gyro_x += 5;                         

    if(imu660ra_gyro_z > 10) imu660ra_gyro_z -= 10;                               
    else if(imu660ra_gyro_z < -10) imu660ra_gyro_z += 10;                         
    
    // 加速度数据平滑处理
   // imu660ra_acc_x = imu660ra_acc_x / 10 * 10;
  //  imu660ra_acc_y = imu660ra_acc_y / 10 * 10;
 //   imu660ra_acc_z = imu660ra_acc_z / 10 * 10; 
}
void updateAttitude(void)
{
    float gx = imu660ra_gyro_transition(imu660ra_gyro_x) * DEG_TO_RAD;
    float gy = imu660ra_gyro_transition(imu660ra_gyro_y) * DEG_TO_RAD;
    float gz = imu660ra_gyro_transition(imu660ra_gyro_z) * DEG_TO_RAD;
    
    float dt = SAMPLE_TIME_MS / 1000.0f;
    float q0 = attitude.q0, q1 = attitude.q1, q2 = attitude.q2, q3 = attitude.q3;
    
    // Mahony算法修正
    float ax = imu660ra_acc_x, ay = imu660ra_acc_y, az = imu660ra_acc_z;
    float acc_norm = sqrtf(ax*ax + ay*ay + az*az);
    
    if(acc_norm > 0.5f && acc_norm < 1.5f) {
        ax /= acc_norm; ay /= acc_norm; az /= acc_norm;
        
        float vx = 2.0f * (q1*q3 - q0*q2);
        float vy = 2.0f * (q0*q1 + q2*q3);
        float vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
        
        float ex = ay*vz - az*vy;
        float ey = az*vx - ax*vz;
        float ez = ax*vy - ay*vx;
        
        static float integralFBx = 0.0f, integralFBy = 0.0f, integralFBz = 0.0f;
        integralFBx += ex * 0.1f * dt;  // 乘以dt
        integralFBy += ey * 0.1f * dt;
        integralFBz += ez * 0.1f * dt;
        
        // 修正角速度
        gx += 2.0f * ex + integralFBx;
        gy += 2.0f * ey + integralFBy;
        gz += 2.0f * ez + integralFBz;
    }
    
    // 只进行一次四元数积分
    attitude.q0 += (-q1*gx - q2*gy - q3*gz) * 0.5f * dt;
    attitude.q1 += ( q0*gx - q3*gy + q2*gz) * 0.5f * dt;
    attitude.q2 += ( q3*gx + q0*gy - q1*gz) * 0.5f * dt;
    attitude.q3 += (-q2*gx + q1*gy + q0*gz) * 0.5f * dt;
    
 // 四元数归一化
    float norm = sqrtf(attitude.q0*attitude.q0 + attitude.q1*attitude.q1 + 
                      attitude.q2*attitude.q2 + attitude.q3*attitude.q3);
    if(norm > 0.0f) {
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
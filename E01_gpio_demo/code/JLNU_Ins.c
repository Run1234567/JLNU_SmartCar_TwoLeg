/**
 * 惯性导航解算模块 - 固定5ms时间间隔版本
 */

#include "zf_common_headfile.h"
#include <math.h>
#include "JLNU_Ins.h"

// 定义导航状态结构体
static INS_NavState_t nav_state;

// 重力加速度 (m/s?)
#define GRAVITY_CONST 9.80665f

// 固定时间间隔 5ms = 0.005秒
#define INS_FIXED_DT 0.005f

// 加速度计校准参数
static AccelCalib_t accel_calib = {
    .offset_x = 0.0f,
    .offset_y = 0.0f,
    .offset_z = 0.0f,
    .scale_x = 1.0f,
    .scale_y = 1.0f,
    .scale_z = 1.0f
};

// 旋转矩阵（存储方向余弦矩阵）
static float rotation_matrix[3][3];

// 四元数转旋转矩阵
static void quat_to_rotation_matrix(float q0, float q1, float q2, float q3) {
    // 预先计算常用项
    float q0q0 = q0 * q0;
    float q0q1 = q0 * q1;
    float q0q2 = q0 * q2;
    float q0q3 = q0 * q3;
    
    float q1q1 = q1 * q1;
    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    
    float q3q3 = q3 * q3;
    
    // 计算旋转矩阵
    rotation_matrix[0][0] = 1.0f - 2.0f * (q2q2 + q3q3);
    rotation_matrix[0][1] = 2.0f * (q1q2 - q0q3);
    rotation_matrix[0][2] = 2.0f * (q1q3 + q0q2);
    
    rotation_matrix[1][0] = 2.0f * (q1q2 + q0q3);
    rotation_matrix[1][1] = 1.0f - 2.0f * (q1q1 + q3q3);
    rotation_matrix[1][2] = 2.0f * (q2q3 - q0q1);
    
    rotation_matrix[2][0] = 2.0f * (q1q3 - q0q2);
    rotation_matrix[2][1] = 2.0f * (q2q3 + q0q1);
    rotation_matrix[2][2] = 1.0f - 2.0f * (q1q1 + q2q2);
}

// 加速度计校准补偿
static void compensate_accelerometer(float* ax, float* ay, float* az) {
    *ax = (*ax - accel_calib.offset_x) * accel_calib.scale_x;
    *ay = (*ay - accel_calib.offset_y) * accel_calib.scale_y;
    *az = (*az - accel_calib.offset_z) * accel_calib.scale_z;
}

// 校准加速度计（需要在静止水平状态下调用）
void INS_CalibrateAccelerometer(void) {
    float sum_x = 0.0f, sum_y = 0.0f, sum_z = 0.0f;
    int samples = 100;
    
    printf("开始加速度计校准，请保持设备静止水平...\n");
    
    for (int i = 0; i < samples; i++) {
        IMU660_GetData();
        sum_x += imu660ra_acc_x;
        sum_y += imu660ra_acc_y;
        sum_z += imu660ra_acc_z;
        system_delay_ms(10);
    }
    
    // 计算零偏
    accel_calib.offset_x = sum_x / samples;
    accel_calib.offset_y = sum_y / samples;
    accel_calib.offset_z = (sum_z / samples) + GRAVITY_CONST;  // 静止时z轴应为-g
    
    printf("校准完成: offset_x=%.3f, offset_y=%.3f, offset_z=%.3f\n",
           accel_calib.offset_x, accel_calib.offset_y, accel_calib.offset_z);
}

// 初始化惯性导航系统
void INS_Init(void) {
    // 初始化导航状态
    nav_state.position_x = 0.0f;
    nav_state.position_y = 0.0f;
    nav_state.position_z = 0.0f;
    
    nav_state.velocity_x = 0.0f;
    nav_state.velocity_y = 0.0f;
    nav_state.velocity_z = 0.0f;
    
    nav_state.accel_nav_x = 0.0f;
    nav_state.accel_nav_y = 0.0f;
    nav_state.accel_nav_z = 0.0f;
    
    nav_state.is_moving = 0;
    nav_state.motion_counter = 0;
    
    // 初始化旋转矩阵为单位矩阵
    rotation_matrix[0][0] = 1.0f; rotation_matrix[0][1] = 0.0f; rotation_matrix[0][2] = 0.0f;
    rotation_matrix[1][0] = 0.0f; rotation_matrix[1][1] = 1.0f; rotation_matrix[1][2] = 0.0f;
    rotation_matrix[2][0] = 0.0f; rotation_matrix[2][1] = 0.0f; rotation_matrix[2][2] = 1.0f;
    
    printf("INS导航系统初始化完成，固定dt=%.3fs (%.0fHz)\n", 
           INS_FIXED_DT, 1.0f/INS_FIXED_DT);
}

// 重置导航状态（归零）
void INS_Reset(void) {
    nav_state.position_x = 0.0f;
    nav_state.position_y = 0.0f;
    nav_state.position_z = 0.0f;
    
    nav_state.velocity_x = 0.0f;
    nav_state.velocity_y = 0.0f;
    nav_state.velocity_z = 0.0f;
    
    nav_state.accel_nav_x = 0.0f;
    nav_state.accel_nav_y = 0.0f;
    nav_state.accel_nav_z = 0.0f;
    
    printf("INS导航状态已重置\n");
}

// 设置加速度计校准参数（可选）
void INS_SetAccelCalibration(float offset_x, float offset_y, float offset_z,
                             float scale_x, float scale_y, float scale_z) {
    accel_calib.offset_x = offset_x;
    accel_calib.offset_y = offset_y;
    accel_calib.offset_z = offset_z;
    
    accel_calib.scale_x = scale_x;
    accel_calib.scale_y = scale_y;
    accel_calib.scale_z = scale_z;
}

// 核心导航解算函数（固定5ms间隔）
void INS_Update(void) {
    int a=0;
    // 使用固定时间间隔
    float dt = INS_FIXED_DT;  // 固定为5ms
    
    // 常量定义
    #define GRAVITY_M_S2 9.80485f      // 重力加速度 m/s?
    #define G_TO_MS2 9.80485f          // g到m/s?的转换系数
    #define MOTION_THRESHOLD 0.15f     // 运动阈值 m/s?
    #define MAX_VELOCITY 10.0f         // 最大速度 m/s
    #define MAX_POSITION 1000.0f       // 最大位置 m
    
    // 1. 获取最新传感器数据和姿态
    IMU660_GetData();
    updateAttitude();
    
    // 2. 获取当前姿态四元数
    float q0 = attitude.q0;
    float q1 = attitude.q1;
    float q2 = attitude.q2;
    float q3 = attitude.q3;
    
    // 3. 计算旋转矩阵
    quat_to_rotation_matrix(q0, q1, q2, q3);
    
    // 4. 加速度计数据转换与补偿

    float ax = imu660ra_acc_transition(imu660ra_acc_x/10*10) *GRAVITY_CONST;  
    float ay = imu660ra_acc_transition(imu660ra_acc_y/10*10) *GRAVITY_CONST;
    float az = imu660ra_acc_transition(imu660ra_acc_z/10*10) *GRAVITY_CONST;
    
    compensate_accelerometer(&ax, &ay, &az);
    
    // 5. 将机体加速度转换到导航系（ENU）
    float accel_nav_x = rotation_matrix[0][0] * ax + rotation_matrix[0][1] * ay + rotation_matrix[0][2] * az;
    float accel_nav_y = rotation_matrix[1][0] * ax + rotation_matrix[1][1] * ay + rotation_matrix[1][2] * az;
    float accel_nav_z = rotation_matrix[2][0] * ax + rotation_matrix[2][1] * ay + rotation_matrix[2][2] * az;
    
    float motion_accel_x = accel_nav_x;
    float motion_accel_y = accel_nav_y;
    float motion_accel_z = accel_nav_z - GRAVITY_M_S2-0.1;
   
if(motion_accel_x<0.02&&motion_accel_x>=-0.02)motion_accel_x=0;
if(motion_accel_y<=0.03&&motion_accel_y>=-0.03)motion_accel_y=0;
if(motion_accel_z<=0.03&&motion_accel_z>=-0.03)motion_accel_z=0;
if(motion_accel_x==0&&motion_accel_y==0&&motion_accel_z==0)a++;
else a=0;
    nav_state.accel_nav_x = motion_accel_x;
    nav_state.accel_nav_y = motion_accel_y;
    nav_state.accel_nav_z = motion_accel_z;
    
    // 8. 速度更新（积分）
        // 正常积分，单位：m/s? × s = m/s
        nav_state.velocity_x += motion_accel_x * dt;
        nav_state.velocity_y += motion_accel_y * dt;
        nav_state.velocity_z += motion_accel_z * dt;
        if(a!=0)
        {
            nav_state.velocity_x=0;
            nav_state.velocity_y=0;
            nav_state.velocity_z=0;
        }
    // 10. 位置更新（积分）
    nav_state.position_x += nav_state.velocity_x * dt;
    nav_state.position_y += nav_state.velocity_y * dt;
    nav_state.position_z += nav_state.velocity_z * dt;
}

// 获取导航状态
INS_NavState_t* INS_GetNavState(void) {
    return &nav_state;
}

// 获取当前位置
void INS_GetPosition(float* x, float* y, float* z) {
    *x = nav_state.position_x;
    *y = nav_state.position_y;
    *z = nav_state.position_z;
}

// 设置当前位置（用于外部校正）
void INS_SetPosition(float x, float y, float z) {
    nav_state.position_x = x;
    nav_state.position_y = y;
    nav_state.position_z = z;
}

// 获取当前速度
void INS_GetVelocity(float* vx, float* vy, float* vz) {
    *vx = nav_state.velocity_x;
    *vy = nav_state.velocity_y;
    *vz = nav_state.velocity_z;
}

// 设置当前速度（用于外部校正）
void INS_SetVelocity(float vx, float vy, float vz) {
    nav_state.velocity_x = vx;
    nav_state.velocity_y = vy;
    nav_state.velocity_z = vz;
}

// 获取姿态欧拉角
void INS_GetEulerAngles(float* roll, float* pitch, float* yaw) {
    *roll = attitude.roll;
    *pitch = attitude.pitch;
    *yaw = attitude.yaw;
}

// 获取导航系加速度
void INS_GetNavigationAccel(float* ax, float* ay, float* az) {
    *ax = nav_state.accel_nav_x;
    *ay = nav_state.accel_nav_y;
    *az = nav_state.accel_nav_z;
}

// 检查是否在运动中
uint8_t INS_IsMoving(void) {
    return nav_state.is_moving;
}

// 计算总位移距离
float INS_GetTotalDistance(void) {
    return sqrtf(nav_state.position_x*nav_state.position_x + 
                 nav_state.position_y*nav_state.position_y + 
                 nav_state.position_z*nav_state.position_z);
}

// 计算总速度大小
float INS_GetSpeed(void) {
    return sqrtf(nav_state.velocity_x*nav_state.velocity_x + 
                 nav_state.velocity_y*nav_state.velocity_y + 
                 nav_state.velocity_z*nav_state.velocity_z);
}

// 获取水平位移距离（忽略高度）
float INS_GetHorizontalDistance(void) {
    return sqrtf(nav_state.position_x*nav_state.position_x + 
                 nav_state.position_y*nav_state.position_y);
}

// 获取水平速度大小
float INS_GetHorizontalSpeed(void) {
    return sqrtf(nav_state.velocity_x*nav_state.velocity_x + 
                 nav_state.velocity_y*nav_state.velocity_y);
}

// 获取运行统计信息
void INS_GetStats(uint32_t* update_count, float* total_distance) {
    static uint32_t internal_update_count = 0;
    internal_update_count++;
    
    *update_count = internal_update_count;
    *total_distance = INS_GetTotalDistance();
}
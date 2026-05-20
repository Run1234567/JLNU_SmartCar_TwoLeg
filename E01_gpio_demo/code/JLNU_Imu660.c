#include "zf_common_headfile.h"
#include <math.h>

// 初始状态：Roll=-176, Pitch=-38, Yaw=0
Attitude_t attitude = {0.0330f, -0.9449f, -0.0114f, -0.3254f, -176.0f, -38.0f, 0.0f};
// ? 存放低通滤波后的 IMU 数据
float lpf_acc_x = 0.0f, lpf_acc_y = 0.0f, lpf_acc_z = 0.0f;
float lpf_gyro_x = 0.0f, lpf_gyro_y = 0.0f, lpf_gyro_z = 0.0f;

void DWT_Init(void)
{
    // 1. 开启 DWT 外设（使能 DEMCR 寄存器的 TRCENA 位）
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    
    // 2. 解锁 DWT 寄存器（Cortex-M7 需要这步，M4/M3 通常不需要）
    DWT->LAR = 0xC5ACCE55; 
    
    // 3. 清空周期计数器
    DWT->CYCCNT = 0;
    
    // 4. 使能 DWT 周期计数器
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
void IMU660_Init()
{
   imu660ra_init();
}

// 数据获取函数（保持不变）
void IMU660_GetData(void)
{
    imu660ra_get_acc();                                                         
    imu660ra_get_gyro();                                                        
    imu660ra_gyro_z+=3;
//     // 角速度数据校准补偿                                                       
    imu660ra_gyro_y -= 6;                                                       
    imu660ra_gyro_x+=2;
//     // 角速度数据死区处理
    if(imu660ra_gyro_y <= 5 && imu660ra_gyro_y >= -5) imu660ra_gyro_y = 0;      
    if(imu660ra_gyro_x <= 5 && imu660ra_gyro_x >= -5) imu660ra_gyro_x = 0;      
    if(imu660ra_gyro_z <= 5 && imu660ra_gyro_z >= -5) imu660ra_gyro_z = 0;      

    // // // 角速度数据平滑处理
    if(imu660ra_gyro_y > 0) imu660ra_gyro_y -= 5;                               
    else if(imu660ra_gyro_y <0) imu660ra_gyro_y += 5;                         

    if(imu660ra_gyro_x > 0) imu660ra_gyro_x -= 5;                               
    else if(imu660ra_gyro_x < 0) imu660ra_gyro_x += 5;                         

    if(imu660ra_gyro_z > 0) imu660ra_gyro_z -= 5;                               
    else if(imu660ra_gyro_z < 0) imu660ra_gyro_z += 5;  
    imu660ra_gyro_z=imu660ra_gyro_z;
    // 加速度数据平滑处理
   imu660ra_acc_x = imu660ra_acc_x / 10 * 10;
   imu660ra_acc_y = imu660ra_acc_y / 10 * 10;
   imu660ra_acc_z = imu660ra_acc_z / 10 * 10; 
}

// 定义Mahony滤波器参数
#define KP 0.5f        // 比例增益
#define KI 0.02      // 积分增益
#define INTEGRAL_LIMIT  10.0f  // 积分限幅

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
        // 4. 积分误差（带限幅）
        integralFBx += KI * ex * dt;
        integralFBy += KI * ey * dt;
        
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
    }
    
    // Mahony滤波算法结束 ====================================
    
    // 使用修正后的陀螺仪数据进行四元数积分--
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

void Convert_GPS_To_XY(void)
{
    // 如果一个点都没采，直接退出
    if (current_point_count == 0) return; 

    // 1. 提取第 0 个点作为绝对原点 (0, 0)
    double lat0 = Route_Points[0].latitude;
    double lon0 = Route_Points[0].longitude;

    // 2. 预先算好角度转弧度的系数，以及纬度补偿的 cos 值
    // 因为是局部小范围寻迹，整个地图共用原点的 cos 值，误差可以忽略不计
    double deg2rad = PI / 180.0;
    double cos_lat0 = cos(lat0 * deg2rad);

    // 3. 第 0 个点自身设为原点
    XY_Points[0].x = 0.0f;
    XY_Points[0].y = 0.0f;

    // 4. 遍历剩下的所有点，依次转换为米
    for (uint16 i = 1; i < current_point_count; i++)
    {
        double d_lat = Route_Points[i].latitude - lat0;
        double d_lon = Route_Points[i].longitude - lon0;

        // 计算 Y 轴（正北）距离
        XY_Points[i].y = (float)(d_lat * deg2rad * EARTH_RADIUS);

        // 计算 X 轴（正东）距离，注意要乘以纬度补偿 cos_lat0
        XY_Points[i].x = (float)(d_lon * deg2rad * EARTH_RADIUS * cos_lat0);
    }
}

// ==========================================
// 函数功能：实时更新当前 GPS 的局部 XY 坐标
// 调用时机：在模式 6 的寻迹控制循环中调用
// ==========================================
void Update_GPS_Now_XY(void)
{
    // 预先计算弧度转换系数
    double deg2rad = PI / 180.0;
    
    // 1. 使用进入模式6时记录的起点纬度，计算纬度补偿 cos 值
    double cos_lat0 = cos(Reference_GPS.latitude * deg2rad);
    
    // 2. 计算当前实时 GPS 距离参考原点的经纬度差值
    // （假设 gnss 是你底层存放最新 GPS 数据的结构体）
    double d_lat = gnss.latitude - Reference_GPS.latitude;
    double d_lon = gnss.longitude - Reference_GPS.longitude;
    
    // 3. 转化为局部的 X 和 Y 坐标，并存入你指定的新变量中！
    GPS_Y_Now = (float)(d_lat * deg2rad * EARTH_RADIUS);
    GPS_X_Now = (float)(d_lon * deg2rad * EARTH_RADIUS * cos_lat0);
}

void IMU660RC_GetData(void)
{
    imu660rc_get_acc();                                                         
    imu660rc_get_gyro();    
    
    // 1. --- 零偏与死区处理（保持你的原样） ---
    imu660rc_gyro_x += 6;
    imu660rc_gyro_y -= 4;
    imu660rc_gyro_z -= 6;        
    if(imu660rc_gyro_y <= 3 && imu660rc_gyro_y >= -3) imu660rc_gyro_y = 0;      
    if(imu660rc_gyro_x <= 3 && imu660rc_gyro_x >= -3) imu660rc_gyro_x = 0;      
    if(imu660rc_gyro_z <= 3 && imu660rc_gyro_z >= -3) imu660rc_gyro_z = 0; 
    else   imu660rc_gyro_z-=1;
    
    // 2. --- ? 一阶低通滤波 ---
    // 滤波系数 alpha 取值范围 0~1。
    // 越小：抗震动能力越强，线条越平滑，但响应会有延迟。
    // 越大：响应越灵敏，但抗噪能力弱。
    float acc_alpha = 0.5f;  // 加速度计受底盘震动影响极大，滤狠一点（只信任10%的新数据）
    float gyro_alpha = 0.9f; // 陀螺仪本身高频特性好，少滤一点保留灵敏度（信任40%的新数据）

    // 加速度计滤波
    lpf_acc_x = acc_alpha * imu660rc_acc_x + (1.0f - acc_alpha) * lpf_acc_x;
    lpf_acc_y = acc_alpha * imu660rc_acc_y + (1.0f - acc_alpha) * lpf_acc_y;
    lpf_acc_z = acc_alpha * imu660rc_acc_z + (1.0f - acc_alpha) * lpf_acc_z;

    // 陀螺仪滤波
    lpf_gyro_x = gyro_alpha * imu660rc_gyro_x + (1.0f - gyro_alpha) * lpf_gyro_x;
    lpf_gyro_y = gyro_alpha * imu660rc_gyro_y + (1.0f - gyro_alpha) * lpf_gyro_y;
    lpf_gyro_z = gyro_alpha * imu660rc_gyro_z + (1.0f - gyro_alpha) * lpf_gyro_z;
}
// 记录上一次的周期数
static uint32_t last_cycle_count = 0;
void updateAttitude_rc(void)
{
    // 1. 获取当前的高精度周期数 (确保之前已经在初始化里调用了 DWT_Init)
    uint32_t current_cycle_count = DWT->CYCCNT;
    
    // 2. 计算周期差 (利用 32 位无符号整型的自然溢出回绕特性，安全无忧)
    uint32_t cycle_diff = current_cycle_count - last_cycle_count;
    
    // 3. 更新时间戳
    last_cycle_count = current_cycle_count;
    
    // 4. 计算极高精度的 dt (单位：秒)
    float dt = (float)cycle_diff / CPU_FREQ_HZ;

    // 5. 异常保护：防止第一次进函数，或者断点调试时 dt 爆炸
    if (dt > 0.05f || dt <= 0.0f) {
        dt = 0.005f; // 假设你的正常解算周期是 5ms (200Hz)
    }

    float gx = imu660rc_gyro_transition(lpf_gyro_x) * DEG_TO_RAD;
    float gy = imu660rc_gyro_transition(lpf_gyro_y) * DEG_TO_RAD;
    float gz = imu660rc_gyro_transition(lpf_gyro_z) * DEG_TO_RAD;
    // float dt = SAMPLE_TIME_MS / 1000.0f;
    float q0 = attitude.q0, q1 = attitude.q1, q2 = attitude.q2, q3 = attitude.q3;
    
    // ? 注意这里！把赋给 ax, ay, az 的值换成 lpf_acc_x 等等
    float ax = lpf_acc_x;
    float ay = lpf_acc_y;
    float az = lpf_acc_z;
    
    float norm_acc = sqrtf(ax*ax + ay*ay + az*az);
    if (norm_acc > 0.0f) {
        ax /= norm_acc;
        ay /= norm_acc;
        az /= norm_acc;
        float dynamic_KP = KP; // 默认使用你原本的常数比例项
        float dynamic_KI = KI; // 默认使用你原本的常数积分项
        // 2. 从当前四元数中提取重力方向
        float vx = 2.0f * (q1*q3 - q0*q2);
        float vy = 2.0f * (q0*q1 + q2*q3);
        float vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;
        
        // 3. 计算误差（叉积）
        float ex = (ay*vz - az*vy);
        float ey = (az*vx - ax*vz);
        // 4. 积分误差（带限幅）
        integralFBx += dynamic_KI * ex * dt;
        integralFBy += dynamic_KI * ey * dt;
        if (norm_acc > 4500||norm_acc < 3500)
        { // 如果加速度计数据异常，说明可能正在经历剧烈震动，这时暂时不信任加速度计的反馈，切断 P 和 I 反馈，等震动过去了再慢慢恢复信任{
            dynamic_KP = 0.0f;  // 彻底不信加速度计！切断 P 反馈
            dynamic_KI = 0.0f;  // 切断 I 反馈，防止这段时间的错误力方向被当成温漂积进去
        }
        // 积分限幅
        if (integralFBx > INTEGRAL_LIMIT) integralFBx = INTEGRAL_LIMIT;
        if (integralFBx < -INTEGRAL_LIMIT) integralFBx = -INTEGRAL_LIMIT;
        if (integralFBy > INTEGRAL_LIMIT) integralFBy = INTEGRAL_LIMIT;
        if (integralFBy < -INTEGRAL_LIMIT) integralFBy = -INTEGRAL_LIMIT;
        if (integralFBz > INTEGRAL_LIMIT) integralFBz = INTEGRAL_LIMIT;
        if (integralFBz < -INTEGRAL_LIMIT) integralFBz = -INTEGRAL_LIMIT;
        
        // 5. 应用比例和积分反馈到陀螺仪数据
        gx += dynamic_KP * ex + integralFBx;
        gy += dynamic_KP * ey + integralFBy;
    }
    
    // Mahony滤波算法结束 ====================================
    
    // 使用修正后的陀螺仪数据进行四元数积分--
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
attitude.yaw = -atan2f(2*(attitude.q0*attitude.q3 + attitude.q1*attitude.q2), 
                          1 - 2*(attitude.q2*attitude.q2 + attitude.q3*attitude.q3)) * RAD_TO_DEG;
}

/**
 * @brief  强行重置 IMU 的偏航角 (Yaw)
 * @param  new_yaw_deg  你刚测出来的新偏航角 (单位: 度, 0~360 或 -180~180)
 * @note   在 GPS 算出初始航向后调用此函数一次即可
 */
void IMU_Force_Reset_Yaw(float new_yaw_deg)
{
    // 1. 将现有的欧拉角(Roll, Pitch)和新的 Yaw 转换为弧度
    float roll_rad  = attitude.roll  * DEG_TO_RAD; // 保持当前的 Roll 不变 (不影响平衡)
    float pitch_rad = attitude.pitch * DEG_TO_RAD; // 保持当前的 Pitch 不变
    float yaw_rad   = -new_yaw_deg    * DEG_TO_RAD; // 替换为你刚算出来的 Yaw

    // 2. 计算半角的正弦和余弦
    float cy = cosf(yaw_rad * 0.5f);
    float sy = sinf(yaw_rad * 0.5f);
    float cp = cosf(pitch_rad * 0.5f);
    float sp = sinf(pitch_rad * 0.5f);
    float cr = cosf(roll_rad * 0.5f);
    float sr = sinf(roll_rad * 0.5f);

    // 3. 欧拉角转四元数 (标准 ZYX 旋转顺序)
    // 这一步直接覆盖 Mahony 滤波器的底层记忆！
    attitude.q0 = cr * cp * cy + sr * sp * sy;
    attitude.q1 = sr * cp * cy - cr * sp * sy;
    attitude.q2 = cr * sp * cy + sr * cp * sy;
    attitude.q3 = cr * cp * sy - sr * sp * cy;

    // 4. 同步更新一下表面的度数显示
    attitude.yaw = new_yaw_deg;
    
    // (可选) 如果你发现强行赋值后瞬间有一点抖动，可以顺手清空一下积分项
    // extern float integralFBx, integralFBy, integralFBz; // 如果这些在外部没有定义 extern，忽略这两句即可
    // integralFBx = 0.0f; integralFBy = 0.0f; integralFBz = 0.0f;
}


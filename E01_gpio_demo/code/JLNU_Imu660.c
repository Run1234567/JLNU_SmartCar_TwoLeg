#include "zf_common_headfile.h"
#include <math.h>

/* 初始状态：Roll=-176, Pitch=-38, Yaw=0 */
Attitude_t attitude = {0.0330f, -0.9449f, -0.0114f, -0.3254f, -176.0f, -38.0f, 0.0f};
/* 一阶低通滤波后的 IMU 数据 */
float lpf_acc_x = 0.0f, lpf_acc_y = 0.0f, lpf_acc_z = 0.0f;
float lpf_gyro_x = 0.0f, lpf_gyro_y = 0.0f, lpf_gyro_z = 0.0f;

void DWT_Init(void)
{
    /* 1. 使能 DWT 模块（通过 DEMCR 寄存器的 TRCENA 位） */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* 2. 解锁 DWT 寄存器（Cortex-M7 需要此步骤，M4/M3 通常不需要） */
    DWT->LAR = 0xC5ACCE55;

    /* 3. 清零周期计数器 */
    DWT->CYCCNT = 0;

    /* 4. 使能 DWT 周期计数器 */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
void IMU660_Init()
{
   imu660ra_init();
}

/* 数据获取（基本参数不变） */
void IMU660_GetData(void)
{
    imu660ra_get_acc();
    imu660ra_get_gyro();
    imu660ra_gyro_z+=3;
//     /* 加速度数据校准（可选） */
    imu660ra_gyro_y -= 6;
    imu660ra_gyro_x+=2;
//     /* 陀螺仪数据死区处理 */
    if(imu660ra_gyro_y <= 5 && imu660ra_gyro_y >= -5) imu660ra_gyro_y = 0;
    if(imu660ra_gyro_x <= 5 && imu660ra_gyro_x >= -5) imu660ra_gyro_x = 0;
    if(imu660ra_gyro_z <= 5 && imu660ra_gyro_z >= -5) imu660ra_gyro_z = 0;

    /* 陀螺仪数据死区平衡处理 */
    if(imu660ra_gyro_y > 0) imu660ra_gyro_y -= 5;
    else if(imu660ra_gyro_y <0) imu660ra_gyro_y += 5;

    if(imu660ra_gyro_x > 0) imu660ra_gyro_x -= 5;
    else if(imu660ra_gyro_x < 0) imu660ra_gyro_x += 5;

    if(imu660ra_gyro_z > 0) imu660ra_gyro_z -= 5;
    else if(imu660ra_gyro_z < 0) imu660ra_gyro_z += 5;
    imu660ra_gyro_z=imu660ra_gyro_z;
    /* 加速度数据死区平衡处理 */
   imu660ra_acc_x = imu660ra_acc_x / 10 * 10;
   imu660ra_acc_y = imu660ra_acc_y / 10 * 10;
   imu660ra_acc_z = imu660ra_acc_z / 10 * 10;
}

/* Mahony互补滤波器参数 */
#define KP 0.5f        /* 比例增益 */
#define KI 0.02      /* 积分增益 */
#define INTEGRAL_LIMIT  10.0f  /* 积分限幅 */

/* 全局变量：积分项 */
static float integralFBx = 0.0f;
static float integralFBy = 0.0f;
static float integralFBz = 0.0f;

void updateAttitude(void)
{
    /* 获取陀螺仪数据并转换为弧度/秒 */
    float gx = imu660ra_gyro_transition(imu660ra_gyro_x) * DEG_TO_RAD;
    float gy = imu660ra_gyro_transition(imu660ra_gyro_y) * DEG_TO_RAD;
    float gz = imu660ra_gyro_transition(imu660ra_gyro_z) * DEG_TO_RAD;
    float dt = SAMPLE_TIME_MS / 1000.0f;
    float q0 = attitude.q0, q1 = attitude.q1, q2 = attitude.q2, q3 = attitude.q3;

    /* 1. 归一化加速度计数据 */
    float ax = imu660ra_acc_x;
    float ay = imu660ra_acc_y;
    float az = imu660ra_acc_z;

    float norm_acc = sqrtf(ax*ax + ay*ay + az*az);
    if (norm_acc > 0.0f) {
        ax /= norm_acc;
        ay /= norm_acc;
        az /= norm_acc;

        /* 2. 从当前四元数中提取重力方向 */
        float vx = 2.0f * (q1*q3 - q0*q2);
        float vy = 2.0f * (q0*q1 + q2*q3);
        float vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;

        /* 3. 计算误差（叉积） */
        float ex = (ay*vz - az*vy);
        float ey = (az*vx - ax*vz);
        /* 4. 积分误差（带限幅） */
        integralFBx += KI * ex * dt;
        integralFBy += KI * ey * dt;

        /* 积分限幅 */
        if (integralFBx > INTEGRAL_LIMIT) integralFBx = INTEGRAL_LIMIT;
        if (integralFBx < -INTEGRAL_LIMIT) integralFBx = -INTEGRAL_LIMIT;
        if (integralFBy > INTEGRAL_LIMIT) integralFBy = INTEGRAL_LIMIT;
        if (integralFBy < -INTEGRAL_LIMIT) integralFBy = -INTEGRAL_LIMIT;
        if (integralFBz > INTEGRAL_LIMIT) integralFBz = INTEGRAL_LIMIT;
        if (integralFBz < -INTEGRAL_LIMIT) integralFBz = -INTEGRAL_LIMIT;

        /* 5. 应用比例和积分反馈到陀螺仪数据 */
        gx += KP * ex + integralFBx;
        gy += KP * ey + integralFBy;
    }

    /* Mahony互补滤波算法核心 ================================ */

    /* 使用修正后的陀螺仪数据进行四元数积分更新 */
    attitude.q0 += (-q1*gx - q2*gy - q3*gz) * 0.5f * dt;
    attitude.q1 += ( q0*gx - q3*gy + q2*gz) * 0.5f * dt;
    attitude.q2 += ( q3*gx + q0*gy - q1*gz) * 0.5f * dt;
    attitude.q3 += (-q2*gx + q1*gy + q0*gz) * 0.5f * dt;

    /* 四元数归一化 */
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
    /* 转换为欧拉角 */
    attitude.roll = atan2f(2*(attitude.q0*attitude.q1 + attitude.q2*attitude.q3),
                          1 - 2*(attitude.q1*attitude.q1 + attitude.q2*attitude.q2)) * RAD_TO_DEG;
    attitude.pitch = asinf(2*(attitude.q0*attitude.q2 - attitude.q3*attitude.q1)) * RAD_TO_DEG;
    attitude.yaw = atan2f(2*(attitude.q0*attitude.q3 + attitude.q1*attitude.q2),
                         1 - 2*(attitude.q2*attitude.q2 + attitude.q3*attitude.q3)) * RAD_TO_DEG;
}

void Convert_GPS_To_XY(void)
{
    /* 如果一个点都没有，直接退出 */
    if (current_point_count == 0) return;

    /* 1. 取第 0 个点作为坐标原点 (0, 0) */
    double lat0 = Route_Points[0].latitude;
    double lon0 = Route_Points[0].longitude;

    /* 2. 预计算角度转弧度系数，以及纬度的 cos 值 */
    /*    因为是局部小范围搜索，所以地图上原点的 cos 值可以忽略不计 */
    double deg2rad = PI / 180.0;
    double cos_lat0 = cos(lat0 * deg2rad);

    /* 3. 将第 0 个点设为原点 */
    XY_Points[0].x = 0.0f;
    XY_Points[0].y = 0.0f;

    /* 4. 遍历剩下的路径点，全部转换为米 */
    for (uint16 i = 1; i < current_point_count; i++)
    {
        double d_lat = Route_Points[i].latitude - lat0;
        double d_lon = Route_Points[i].longitude - lon0;

        /* 计算 Y 轴（南北方向距离） */
        XY_Points[i].y = (float)(d_lat * deg2rad * EARTH_RADIUS);

        /* 计算 X 轴（东西方向距离，注意要乘纬度差的 cos_lat0） */
        XY_Points[i].x = (float)(d_lon * deg2rad * EARTH_RADIUS * cos_lat0);
    }
}

/* ========================================== */
/* 功能说明：实时更新当前 GPS 的局部 XY 坐标      */
/* 调用时机：寻迹模式 6 的寻线巡线中调用           */
/* ========================================== */
void Update_GPS_Now_XY(void)
{
    /* 预计算弧度转换系数 */
    double deg2rad = PI / 180.0;

    /* 1. 使用近似模式6时记录的基准纬度，计算纬度的 cos 值 */
    double cos_lat0 = cos(Reference_GPS.latitude * deg2rad);

    /* 2. 计算当前实时 GPS 与基准原点的经纬度差值 */
    /*    这里是 gnss 底层传上来的 GPS 数据的结构体 */
    double d_lat = gnss.latitude - Reference_GPS.latitude;
    double d_lon = gnss.longitude - Reference_GPS.longitude;

    /* 3. 转换为局部坐标系 X 和 Y 坐标（单位：米，正北朝上，地图中朝下） */
    GPS_Y_Now = (float)(d_lat * deg2rad * EARTH_RADIUS);
    GPS_X_Now = (float)(d_lon * deg2rad * EARTH_RADIUS * cos_lat0);
}

void IMU660RC_GetData(void)
{
    imu660rc_get_acc();
    imu660rc_get_gyro();

    /* 1. --- 零偏校准（硬编码原始值） --- */
    imu660rc_gyro_x += 6;
    imu660rc_gyro_y -= 4;
    imu660rc_gyro_z -= 6;
    if(imu660rc_gyro_y <= 3 && imu660rc_gyro_y >= -3) imu660rc_gyro_y = 0;
    if(imu660rc_gyro_x <= 3 && imu660rc_gyro_x >= -3) imu660rc_gyro_x = 0;
    if(imu660rc_gyro_z <= 3 && imu660rc_gyro_z >= -3) imu660rc_gyro_z = 0;
    else   imu660rc_gyro_z-=1;

    /* 2. --- 一阶低通滤波 --- */
    /* 滤波系数 alpha 取值范围 0~1 */
    /* 越小，滤波效果越强，曲线越平滑，但响应有延迟 */
    /* 越大，响应越快，但滤波效果越差 */
    float acc_alpha = 0.5f;  /* 加速度计受电机影响极端了，这里激进一点（只保留10%的新数据） */
    float gyro_alpha = 0.9f; /* 陀螺仪本身频率响应较好，这里温和一点保留（保留40%的新数据） */

    /* 加速度计滤波 */
    lpf_acc_x = acc_alpha * imu660rc_acc_x + (1.0f - acc_alpha) * lpf_acc_x;
    lpf_acc_y = acc_alpha * imu660rc_acc_y + (1.0f - acc_alpha) * lpf_acc_y;
    lpf_acc_z = acc_alpha * imu660rc_acc_z + (1.0f - acc_alpha) * lpf_acc_z;

    /* 陀螺仪滤波 */
    lpf_gyro_x = gyro_alpha * imu660rc_gyro_x + (1.0f - gyro_alpha) * lpf_gyro_x;
    lpf_gyro_y = gyro_alpha * imu660rc_gyro_y + (1.0f - gyro_alpha) * lpf_gyro_y;
    lpf_gyro_z = gyro_alpha * imu660rc_gyro_z + (1.0f - gyro_alpha) * lpf_gyro_z;
}
/* 记录上一次的周期计数值 */
static uint32_t last_cycle_count = 0;
void updateAttitude_rc(void)
{
    /* 1. 获取当前的高精度周期计数（确保之前已经在初始化中调用 DWT_Init） */
    uint32_t current_cycle_count = DWT->CYCCNT;

    /* 2. 计算周期数（利用 32 位无符号整数的天然溢出特性，安全可靠） */
    uint32_t cycle_diff = current_cycle_count - last_cycle_count;

    /* 3. 更新时间戳 */
    last_cycle_count = current_cycle_count;

    /* 4. 计算极高精度的 dt（单位：秒） */
    float dt = (float)cycle_diff / CPU_FREQ_HZ;

    /* 5. 异常保护：防止第一次计算或者系统重启后 dt 爆炸 */
    if (dt > 0.05f || dt <= 0.0f) {
        dt = 0.005f; /* 默认回退到保守的 5ms (200Hz) */
    }

    float gx = imu660rc_gyro_transition(lpf_gyro_x) * DEG_TO_RAD;
    float gy = imu660rc_gyro_transition(lpf_gyro_y) * DEG_TO_RAD;
    float gz = imu660rc_gyro_transition(lpf_gyro_z) * DEG_TO_RAD;
    // float dt = SAMPLE_TIME_MS / 1000.0f;
    float q0 = attitude.q0, q1 = attitude.q1, q2 = attitude.q2, q3 = attitude.q3;

    /* 注意这里！把 ax, ay, az 的值改为 lpf_acc_x 等 */
    float ax = lpf_acc_x;
    float ay = lpf_acc_y;
    float az = lpf_acc_z;

    float norm_acc = sqrtf(ax*ax + ay*ay + az*az);
    if (norm_acc > 0.0f) {
        ax /= norm_acc;
        ay /= norm_acc;
        az /= norm_acc;
        float dynamic_KP = KP; /* 默认使用原始的常量参数 */
        float dynamic_KI = KI; /* 默认使用原始的常量参数 */
        /* 2. 从当前四元数中提取重力方向 */
        float vx = 2.0f * (q1*q3 - q0*q2);
        float vy = 2.0f * (q0*q1 + q2*q3);
        float vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;

        /* 3. 计算误差（叉积） */
        float ex = (ay*vz - az*vy);
        float ey = (az*vx - ax*vz);
        /* 4. 积分误差（带限幅） */
        integralFBx += dynamic_KI * ex * dt;
        integralFBy += dynamic_KI * ey * dt;
        if (norm_acc > 4500||norm_acc < 3500)
        { /* 如果加速度计数据异常，说明车身在剧烈振动，此时暂时不要参考加速度计的方向判断，P 和 I 参数暂停，等振动过去后再恢复 */
            dynamic_KP = 0.0f;  /* 完全不参考加速度计，关闭 P 反馈 */
            dynamic_KI = 0.0f;  /* 关闭 I 积分，防止短时的大量累积把陀螺仪漂移拉偏 */
        }
        /* 积分限幅 */
        if (integralFBx > INTEGRAL_LIMIT) integralFBx = INTEGRAL_LIMIT;
        if (integralFBx < -INTEGRAL_LIMIT) integralFBx = -INTEGRAL_LIMIT;
        if (integralFBy > INTEGRAL_LIMIT) integralFBy = INTEGRAL_LIMIT;
        if (integralFBy < -INTEGRAL_LIMIT) integralFBy = -INTEGRAL_LIMIT;
        if (integralFBz > INTEGRAL_LIMIT) integralFBz = INTEGRAL_LIMIT;
        if (integralFBz < -INTEGRAL_LIMIT) integralFBz = -INTEGRAL_LIMIT;

        /* 5. 应用比例和积分反馈到陀螺仪数据 */
        gx += dynamic_KP * ex + integralFBx;
        gy += dynamic_KP * ey + integralFBy;
    }

    /* Mahony互补滤波算法核心 ================================ */

    /* 使用修正后的陀螺仪数据进行四元数积分更新 */
    attitude.q0 += (-q1*gx - q2*gy - q3*gz) * 0.5f * dt;
    attitude.q1 += ( q0*gx - q3*gy + q2*gz) * 0.5f * dt;
    attitude.q2 += ( q3*gx + q0*gy - q1*gz) * 0.5f * dt;
    attitude.q3 += (-q2*gx + q1*gy + q0*gz) * 0.5f * dt;

    /* 四元数归一化 */
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
    /* 转换为欧拉角 */
    attitude.roll = atan2f(2*(attitude.q0*attitude.q1 + attitude.q2*attitude.q3),
                          1 - 2*(attitude.q1*attitude.q1 + attitude.q2*attitude.q2)) * RAD_TO_DEG;
    attitude.pitch = asinf(2*(attitude.q0*attitude.q2 - attitude.q3*attitude.q1)) * RAD_TO_DEG;
attitude.yaw = -atan2f(2*(attitude.q0*attitude.q3 + attitude.q1*attitude.q2),
                          1 - 2*(attitude.q2*attitude.q2 + attitude.q3*attitude.q3)) * RAD_TO_DEG;
}

/**
 * @brief  强制重置 IMU 的偏航角 (Yaw)
 * @param  new_yaw_deg  按照参考系设置的偏航角 (单位: 度, 0~360 或 -180~180)
 * @note   在 GPS 返回初始位置时调用此函数进行一次校准
 */
void IMU_Force_Reset_Yaw(float new_yaw_deg)
{
    /* 1. 保留当前的欧拉角(Roll, Pitch)，仅将 Yaw 转换为弧度 */
    float roll_rad  = attitude.roll  * DEG_TO_RAD; /* 保留当前的 Roll 分量 (不影响水平) */
    float pitch_rad = attitude.pitch * DEG_TO_RAD; /* 保留当前的 Pitch 分量 */
    float yaw_rad   = -new_yaw_deg    * DEG_TO_RAD; /* 替换为传入的新的 Yaw */

    /* 2. 计算各半角的三角函数 */
    float cy = cosf(yaw_rad * 0.5f);
    float sy = sinf(yaw_rad * 0.5f);
    float cp = cosf(pitch_rad * 0.5f);
    float sp = sinf(pitch_rad * 0.5f);
    float cr = cosf(roll_rad * 0.5f);
    float sr = sinf(roll_rad * 0.5f);

    /* 3. 欧拉角转四元数 (标准 ZYX 旋转顺序) */
    /*    这一步直接更新 Mahony 滤波器的底层变量！ */
    attitude.q0 = cr * cp * cy + sr * sp * sy;
    attitude.q1 = sr * cp * cy - cr * sp * sy;
    attitude.q2 = cr * sp * cy + sr * cp * sy;
    attitude.q3 = cr * cp * sy - sr * sp * cy;

    /* 4. 同步更新欧拉角的表示 */
    attitude.yaw = new_yaw_deg;

    /* (可选) 如果你发现强制赋值后滤波器出现一次抖动，可以把下面一行解开 */
    /* extern float integralFBx, integralFBy, integralFBz; // 如果这些变量在外部没有定义 extern，请自己手动加 */
    /* integralFBx = 0.0f; integralFBy = 0.0f; integralFBz = 0.0f; */
}

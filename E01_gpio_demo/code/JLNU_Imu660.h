#ifndef JLNU_IMU660_H
#define JLNU_IMU660__H

#include "zf_common_headfile.h"
#include <math.h>

/* 数学常量 */
#ifndef M_PI
#define M_PI 3.1415926535f
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD (M_PI / 180.0f)
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG (180.0f / M_PI)
#endif

#ifndef SAMPLE_TIME_MS
#define SAMPLE_TIME_MS 5.0f  // 默认采样时间5ms
#endif

/* CPU 频率 250MHz */
#define CPU_FREQ_HZ  250000000.0f

/* 姿态结构体定义 */
typedef struct {
    float q0, q1, q2, q3;    /* 四元数 */
    float roll, pitch, yaw;   /* 欧拉角（角度） */
} Attitude_t;
/* 全局变量声明 */
extern Attitude_t attitude;
/* 一阶低通滤波后的 IMU 数据 */
extern float lpf_acc_x, lpf_acc_y, lpf_acc_z;
extern float lpf_gyro_x, lpf_gyro_y, lpf_gyro_z;
/* 函数声明 */
void DWT_Init(void);

void IMU660_Init();
void IMU660_GetData(void);
void updateAttitude(void);
void Convert_GPS_To_XY(void);
void Update_GPS_Now_XY(void);

void IMU660RC_GetData(void);
void updateAttitude_rc(void);
void IMU_Force_Reset_Yaw(float new_yaw_deg);
#endif

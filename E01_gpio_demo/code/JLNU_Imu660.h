#ifndef JLNU_IMU660_H
#define JLNU_IMU660__H

#include "zf_common_headfile.h"
#include <math.h>

// 常量定义
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
#define SAMPLE_TIME_MS 5.0f  // 默认采样时间10ms
#endif

// 姿态结构体定义
typedef struct {
    float q0, q1, q2, q3;    // 四元数
    float roll, pitch, yaw;   // 欧拉角（度）
} Attitude_t;
// 全局变量声明
extern Attitude_t attitude;
// 函数声明
void IMU660_Init();
void IMU660_GetData(void);
void updateAttitude(void);

#endif
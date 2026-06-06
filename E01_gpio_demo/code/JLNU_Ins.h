/**
 * 惯性导航模块头文件 - 固定5ms版本
 */

#ifndef JLNU_INS_H
#define JLNU_INS_H

#include "zf_common_headfile.h"

/* 导航状态结构体 */
typedef struct {
    /* 位置 (ENU坐标系，单位：米) */
    float position_x;  /* 北向位置 */
    float position_y;  /* 东向位置 */
    float position_z;  /* 天向位置 */

    /* 速度 (ENU坐标系，单位：米/秒) */
    float velocity_x;  /* 北向速度 */
    float velocity_y;  /* 东向速度 */
    float velocity_z;  /* 天向速度 */

    /* 导航系加速度 */
    float accel_nav_x;
    float accel_nav_y;
    float accel_nav_z;

    /* 状态标志 */
    uint8_t is_moving;
    uint8_t motion_counter;
} INS_NavState_t;

/* 加速度计校准结构体 */
typedef struct {
    float offset_x, offset_y, offset_z;
    float scale_x, scale_y, scale_z;
} AccelCalib_t;

/* 固定时间步长定义 */
#define INS_FIXED_DT 0.005f  /* 5ms */

/* 函数声明 */
void INS_Init(void);
void INS_Reset(void);
void INS_Update(void);
void INS_CalibrateAccelerometer(void);
void INS_SetAccelCalibration(float offset_x, float offset_y, float offset_z,
                            float scale_x, float scale_y, float scale_z);

INS_NavState_t* INS_GetNavState(void);
void INS_GetPosition(float* x, float* y, float* z);
void INS_SetPosition(float x, float y, float z);
void INS_GetVelocity(float* vx, float* vy, float* vz);
void INS_SetVelocity(float vx, float vy, float vz);
void INS_GetEulerAngles(float* roll, float* pitch, float* yaw);
void INS_GetNavigationAccel(float* ax, float* ay, float* az);

uint8_t INS_IsMoving(void);
float INS_GetTotalDistance(void);
float INS_GetSpeed(void);
float INS_GetHorizontalDistance(void);
float INS_GetHorizontalSpeed(void);
void INS_GetStats(uint32_t* update_count, float* total_distance);

#endif /* INS_NAVIGATION_H */

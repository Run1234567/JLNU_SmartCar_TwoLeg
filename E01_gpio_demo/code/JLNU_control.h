#ifndef JLNU_CONTROL_H
#define JLNU_CONTROL_H
#include "zf_common_headfile.h"
extern PIDController PID_Angular_V;
extern PIDController PID_Angular;
extern PIDController PID_Speed;
extern PIDController PID_Angle;
extern PIDController PID_High;
extern int TimerTime;
extern int16 Speed_Left;
extern int16 Speed_Right;
extern int16 Speed_Forward; 
extern int16 Speed_Goal;
extern int8 Moter_Flag;

extern int Speed_Sum;
extern float Robot_Pos_X;
extern float Robot_Pos_Y;

typedef struct {
    double latitude;  // 纬度
    double longitude; // 经度
} GPS_Point_t;
extern GPS_Point_t Route_Points[100];   // 航点数组（最多存100个点）
extern uint8 current_point_count;  // 当前已经采了多少个点

#define PI              3.141592653589793
#define EARTH_RADIUS    6371000.0 // 地球平均半径，单位：米
// ========== 新的结构体与数组 ==========
// 局部平面坐标系，单位：米
typedef struct {
    float x; // X轴坐标（正东方向）
    float y; // Y轴坐标（正北方向）
} Local_Point_t;
extern Local_Point_t XY_Points[100]; // 转换后的平面坐标数组
extern Local_Point_t XY_Points[100];  // 转换后的平面坐标数组
extern uint8 current_point_count; // 当前已经采了多少个点

extern Local_Point_t XY_Points_used[100];  // 转换后的平面坐标数组
extern uint8 current_point_count_used; // 当前已经采了多少个点
//===========imu+编码器===========

typedef struct {
    float x; // X轴坐标（正东方向）
    float y; // Y轴坐标（正北方向）
} IMU_Point_t;
extern IMU_Point_t IMU_Points[100]; // 转换后的平面坐标数组

extern uint8 current_IMU_point_count ;  // 当前已经采了多少个点
extern uint8 current_IMU_point_count_used ;  // 当前已经采了多少个点
extern IMU_Point_t IMU_Points_used[100];

//===========
extern IMU_Point_t IMU_Points_KM2[100]; // 转换后的平面坐标数组
extern uint8 current_IMU_point_count_KM2 ;  // 当前已经采了多少个点
extern uint8 current_IMU_point_count_used_KM2 ;  // 当前已经采了多少个点
extern IMU_Point_t IMU_Points_used_KM2[100];
extern int8_t KM2_Turn_Flag; // 当前追踪的目标点序号 (0~3)
extern int32_t KM2_Turn_Out;  // 当前追踪的目标点序号 (0~3)

extern float GPS_X_Now;
extern float GPS_Y_Now;

extern uint8_t Target_Index; // 当前追踪的目标点序号 (0~3)

extern GPS_Point_t Reference_GPS;         // 记录第一次进入模式6时的 GPS 参考点
extern uint8 Mode6_First_Enter_Flag;  // 1表示还没进过，0表示已经记录过了
extern float Mechanical_Zero_Point; // 机械零点
#define Angular_V_P 0.6//0.5//0.6
#define Angular_V_I 0
#define Angular_V_D 0
#define Angular_P 600//500//600
#define Angular_I 0
#define Angular_D 0
#define Speed_P 0.1//0.1
#define Speed_I 0
#define Speed_D 0.01
#define Angle_PID_P 0.015
#define Angle_PID_I 0
#define Angle_PID_D 0.002
#define High_P 3
#define High_I 0.02
#define High_D 0.2

#define Angular_V_P_High 0.5//0.6
#define Angular_V_I_High 0
#define Angular_V_D_High 0
#define Angular_P_High 500//600
#define Angular_I_High 0
#define Angular_D_High  0

#define PULSE_TO_METER  0.0000360973f  
#define DEG_TO_RAD      0.0174532925f  // (π/180)
    
void PWM_SET(int16 PWM_L,int16 PWM_R);
void PID_Init_Angular_V();
void PID_Init_Angular();
void PID_Init_Speed();
void Angular_V_Calculate();
void Angular_Calculate();
void Speed_Calculate();
void Angle_Differential_Control();
void Isr_Control();
void PID_Angle_Init();
void PID_Init_All();
#endif
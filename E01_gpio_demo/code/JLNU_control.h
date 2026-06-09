#ifndef JLNU_CONTROL_H
#define JLNU_CONTROL_H
#include "zf_common_headfile.h"

/* PID控制器外部声明 */
extern PIDController PID_Angular_V;   // 角速度环PID控制器(最内环)
extern PIDController PID_Angular;     // 角度环PID控制器(中间环)
extern PIDController PID_Speed;       // 速度环PID控制器(最外环)
extern PIDController PID_Angle;       // 转向角度PID控制器
extern PIDController PID_High;        // 高度PID控制器

/* 计时器与蜂鸣器 */
extern int TimerTime;                 // 中断计时器(毫秒)
extern int16 Buzzer_Time;             // 蜂鸣器持续时间

/* 电机速度相关变量 */
extern int16 Speed_Left;              // 左轮实际速度(编码器读取)
extern int16 Speed_Right;             // 右轮实际速度(编码器读取)
extern int16 Speed_Forward;           // 左右轮平均速度
extern int16 Speed_Goal;              // 速度环目标值(默认0)
extern int8 Moter_Flag;               // 电机使能标志位

/* 角度目标 */
extern float Angle_Goal;              // 转向目标角度

/* 累计里程 */
extern int Speed_Sum;                 // 速度累加值

/* 机器人在坐标系中的位置(基于编码器里程计) */
extern float Robot_Pos_X;             // 机器人坐标系 X (米)
extern float Robot_Pos_Y;             // 机器人坐标系 Y (米)

/* ========== GPS坐标点结构体 ========== */
typedef struct {
    double latitude;  // 纬度
    double longitude; // 经度
} GPS_Point_t;
extern GPS_Point_t Route_Points[100];   // GPS路线点数组(最多100个点)

extern GPS_Point_t Start_GPS_Array[GPS_SAMPLE_TARGET]; // 遥控采集的GPS坐标数组

extern uint8 current_point_count;  // 当前已经采集了多少个点

#define PI              3.141592653589793
#define EARTH_RADIUS    6371000.0 // 地球平均半径(单位:米)

/* ========== 局部平面坐标系结构体(单位:米) ========== */
typedef struct {
    float x; // X坐标(单位:米)
    float y; // Y坐标(单位:米)
} Local_Point_t;
extern Local_Point_t XY_Points[100];       // 转换后的平面坐标点数组
extern Local_Point_t XY_Points[100];       // 转换后的平面坐标点数组(重复声明)
extern uint8 current_point_count;          // 当前已经采集了多少个点

extern Local_Point_t XY_Points_used[100];  // 转换后的平面坐标点数组(实际使用)
extern uint8 current_point_count_used;     // 当前已经采集了多少个点

/* ========== IMU坐标结构体 ========== */

typedef struct {
    float x; // X坐标(单位:米)
    float y; // Y坐标(单位:米)
} IMU_Point_t;
extern IMU_Point_t IMU_Points[100];           // IMU坐标点采集数组

extern uint8 current_IMU_point_count ;        // 当前已经采集了多少个IMU点
extern uint8 current_IMU_point_count_used ;   // 当前已经使用的IMU点数
extern IMU_Point_t IMU_Points_used[100];      // IMU坐标点使用数组

/* ========== KM2模式IMU坐标 ========== */
extern IMU_Point_t IMU_Points_KM2[100];            // KM2模式IMU坐标点数组
extern uint8 current_IMU_point_count_KM2 ;         // KM2模式当前已采集点数
extern uint8 current_IMU_point_count_used_KM2 ;    // KM2模式当前已使用点数
extern IMU_Point_t IMU_Points_used_KM2[100];       // KM2模式IMU坐标使用数组
extern int8_t KM2_Turn_Flag;                        // KM2原地转向标志 (0:不转向, 1:转向中)
extern int32_t KM2_Turn_Out;                        // KM2转向累计陀螺仪积分值

/* ========== GPS+IMU融合坐标 ========== */
extern uint8 current_IMU_GPS_Num ;          // 当前已采集的GPS+IMU融合点数
extern uint8 current_IMU_GPS_Num_Used ;     // 当前已使用的GPS+IMU融合点数
extern IMU_Point_t IMU_GPS[100];            // GPS+IMU融合坐标采集数组
extern IMU_Point_t IMU_GPS_Used[100];       // GPS+IMU融合坐标使用数组

/* 当前GPS转换坐标 */
extern float GPS_X_Now;                     // 当前GPS X坐标(米)
extern float GPS_Y_Now;                     // 当前GPS Y坐标(米)

extern uint8_t Target_Index;                // 当前跟踪的目标点索引

extern GPS_Point_t Reference_GPS;           // 记录第一次进入模式6时的GPS参考点
extern uint8 Mode6_First_Enter_Flag;        // 1:表示还没记录过, 0:表示已经记录过了
extern float Mechanical_Zero_Point;         // 机械零点(静态平衡角度)
extern float Fused_X;                       // GPS+IMU融合后X坐标
extern float Fused_Y;                       // GPS+IMU融合后Y坐标

extern uint8 GPS_Get_Angle_Flag;            // 为1时开始采集GPS坐标并计算航向角

extern int16_t YaoKong_Test_Mode;           // 遥控测试模式标志

extern uint8 GPS_XY_Flag;                   // GPS坐标更新标志

/* ========== 角速度环PID参数 ========== */
#define Angular_V_P 1.2
#define Angular_V_I 0
#define Angular_V_D 0

/* ========== 角度环PID参数 ========== */
#define Angular_P 1400
#define Angular_I 0
#define Angular_D 0

/* ========== 速度环PID参数 ========== */
#define Speed_P 0.2
#define Speed_I 0.0001
#define Speed_D 0.01


/* ========== 角速度环PID参数 ========== */
#define DianBo_Angular_V_P 0.6
#define DianBo_Angular_V_I 0
#define DianBo_Angular_V_D 0

/* ========== 角度环PID参数 ========== */
#define DianBAngular_P 800
#define DianBAngular_I 0
#define DianBAngular_D 0

/* ========== 速度环PID参数 ========== */
#define DianBoSpeed_P 0.15
#define DianBoSpeed_I 0.0004
#define DianBoSpeed_D 0.005


/* ========== 转向角度PID参数 ========== */
#define Angle_PID_P 0.015
#define Angle_PID_I 0
#define Angle_PID_D 0.002

/* ========== 高度PID参数 ========== */
#define High_P 3
#define High_I 0.02
#define High_D 0.2

/* ========== 高位角速度环PID参数(降速时使用) ========== */
#define Angular_V_P_High 0.5
#define Angular_V_I_High 0
#define Angular_V_D_High 0
#define Angular_P_High 500
#define Angular_I_High 0
#define Angular_D_High  0

/* ========== 单位转换常量 ========== */
#define PULSE_TO_METER  0.0000406094625f   // 编码器脉冲转米的系数
#define DEG_TO_RAD      0.0174532925f      // 度转弧度(PI/180)

/* ========== 函数声明 ========== */
void PWM_SET(int16 PWM_L,int16 PWM_R);            // 设置左右电机PWM值
void PID_Init_Angular_V();                          // 初始化角速度环PID
void PID_Init_Angular();                            // 初始化角度环PID
void PID_Init_Speed();                              // 初始化速度环PID
void Angular_V_Calculate();                         // 角速度环PID计算
void Angular_Calculate();                           // 角度环PID计算
void Speed_Calculate();                             // 速度环PID计算
void Angle_Differential_Control();                  // 转向角度差速控制
void Isr_Control();                                 // 主中断控制函数
void PID_Angle_Init();                              // 初始化转向角度PID
void PID_Init_All();                                // 初始化所有PID控制器
#endif

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



#define Angular_V_P 0.6
#define Angular_V_I 0
#define Angular_V_D 0
#define Angular_P 600
#define Angular_I 0
#define Angular_D 0
#define Speed_P 0.1
#define Speed_I 0
#define Speed_D 0.01
#define Angle_PID_P 0.01
#define Angle_PID_I 0
#define Angle_PID_D 0.002
#define High_P 5
#define High_I 0.1
#define High_D 0.5


#define PULSE_TO_METER  0.0000360973f  
#define DEG_TO_RAD      0.0174532925f  // (¦Ð/180)

    
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
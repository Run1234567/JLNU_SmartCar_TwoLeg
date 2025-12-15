/*
 * JLNU_PID.h
 *
 *  Created on: 2025年9月10日
 *      Author: RUN
 */

#ifndef CODE_JLNU_PID_H_
#define CODE_JLNU_PID_H_

typedef struct {
    float Kp;    //比例系数
    float Ki;   //积分系数
    float Kd;      //微分系数
    float SetPoint;  //目标值
    float ProcessVariable; //过程变量 当前速度
    float ErrorSum;  //误差和
    float LastError;//上一次的误差
    float Output;  //PID输出
} PIDController;
void PID_Init(PIDController *pid, float kp, float ki, float kd, float setPoint);
float PID_Calculate(PIDController *pid, float processVariable,float setPoint);
float PID_Calculate_Angle(PIDController *pid, float processVariable, float setPoint);

#endif /* CODE_JLNU_PID_H_ */

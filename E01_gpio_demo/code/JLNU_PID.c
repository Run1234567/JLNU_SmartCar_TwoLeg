/*
 * JLNU_PID.c
 *
 *  Created on: 2025年9月10日
 *      Author: RUN
 */
#include "zf_common_headfile.h"  // 逐飞通用头文件

//------------------------------------------------------------------------------------------------------------------
// 函数名称     PID控制器初始化函数
// 参数说明     pid         PID控制器结构体指针
// 参数说明     kp          比例系数
// 参数说明     ki          积分系数
// 参数说明     kd          微分系数
// 参数说明     setPoint    目标设定值
// 返回参数     void
// 使用示例     PID_Init(&PID_Angular, 1.0, 0.1, 0.01, 0.0);
// 备注信息     初始化PID控制器的各个参数,包括比例、积分、微分系数及目标值等状态量
//-------------------------------------------------------------------------------------------------------------------
void PID_Init(PIDController *pid, float kp, float ki, float kd, float setPoint)
{
    pid->Kp = kp;                   // 设置比例系数
    pid->Ki = ki;                   // 设置积分系数
    pid->Kd = kd;                   // 设置微分系数
    pid->SetPoint = setPoint;       // 设置目标设定值
    pid->ProcessVariable = 0.0f;    // 初始化过程变量
    pid->ErrorSum = 0.0f;           // 初始化误差累积
    pid->LastError = 0.0f;          // 初始化上一次误差
    pid->Output = 0.0f;             // 初始化输出值
}

//-------------------------------------------------------------------------------------------------------------------
// 函数名称     PID控制器输出计算函数
// 参数说明     pid               PID控制器结构体指针
// 参数说明     processVariable   当前过程变量值
// 参数说明     setPoint          目标设定值
// 返回参数     float             计算得到的PID输出值
// 使用示例     float output = PID_Calculate(&PID_Angular, current_angle, target_angle);
// 备注信息     根据当前过程变量和目标设定值进行PID计算,包含比例、积分和微分三项运算
//-------------------------------------------------------------------------------------------------------------------
float PID_Calculate(PIDController *pid, float processVariable, float setPoint)
{
    float error;                    // 定义误差变量

    pid->SetPoint = setPoint;       // 更新目标设定值
    error = pid->SetPoint - processVariable; // 计算当前误差

    pid->ErrorSum += error;         // 误差累加(用于积分项)

    // 计算PID输出: 比例项 + 积分项 + 微分项
    pid->Output = (pid->Kp * error) + (pid->Ki * pid->ErrorSum) + (pid->Kd * (error - pid->LastError));

    pid->LastError = error;         // 保存上一次误差(用于下一次微分计算)
    pid->ProcessVariable = processVariable; // 更新过程变量

    return pid->Output;             // 返回PID输出值
}

//-------------------------------------------------------------------------------------------------------------------
// 函数名称     角度PID计算函数(带正负180度边界处理)
// 参数说明     pid               PID控制器结构体指针
// 参数说明     processVariable   当前角度值
// 参数说明     setPoint          目标角度设定值
// 返回参数     float             计算得到的PID输出值
// 使用示例     float output = PID_Calculate_Angle(&PID_Angle, current_yaw, target_yaw);
// 备注信息     适用于航向角等需要处理角度跨越正负180度边界的情况
//-------------------------------------------------------------------------------------------------------------------
float PID_Calculate_Angle(PIDController *pid, float processVariable, float setPoint)
{
    float error;

    pid->SetPoint = setPoint;

    //==================== 角度误差计算(跨越180度边界) ====================
    error = pid->SetPoint - processVariable;

    if (error > 180.0f)
        error -= 360.0f;
    else if (error < -180.0f)
        error += 360.0f;
    //===================================================================

    pid->ErrorSum += error;     // 误差累积

    // PID 输出
    pid->Output = (pid->Kp * error)
                + (pid->Ki * pid->ErrorSum)
                + (pid->Kd * (error - pid->LastError));

    pid->LastError = error;
    pid->ProcessVariable = processVariable;

    return pid->Output;
}

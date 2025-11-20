/*
 * JLNU_Imu660.h
 *
 *  Created on: 2025年9月10日
 *      Author: RUN
 */

#ifndef CODE_JLNU_IMU660_H_
#define CODE_JLNU_IMU660_H_

// 一阶互补滤波结构体定义
typedef struct {
    float gyro_ration;          // 角速度置信度系数（通常设置为4）
    float acc_ration;           // 加速度置信度系数（通常设置为4）
    float angle_temp;           // 临时角度变量（用于内部计算）
    float call_cycle;           // 调用周期/采样时间（秒）
    float mechanical_zero;      // 机械零点偏移量
    float filtering_angle;      // 最终滤波后的角度输出
} cascade_common_value_struct;
extern cascade_common_value_struct Angle_Forward;

void IMU660_Debug();
void IMU660_GetData();
void Angle_Calculation();

#endif /* CODE_JLNU_IMU660_H_ */

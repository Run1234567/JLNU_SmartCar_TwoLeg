/*
 * JLNU_Servo.h
 *
 *  Created on: 2025年9月12日
 *      Author: 20461
 */
#include "zf_common_headfile.h"  // 包含必要的头文件
#ifndef CODE_JLNU_SERVO_H_
#define CODE_JLNU_SERVO_H_

void Servo_Four_Init();
void Servo_Leg_Control(int16 Left_Height, int16 Right_Height, int16 Leg_Pitch);

extern int16 pwm_l;
extern int16 pwm_r;

extern int16 High_Right_Point;
extern int16 High_Left_Point;

#endif /* CODE_JLNU_SERVO_H_ */

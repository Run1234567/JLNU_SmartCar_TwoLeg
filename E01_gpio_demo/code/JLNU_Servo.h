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
void Servo_Left_On(int16 Height);//-300到700;
void Servo_Right_On(int16 Height);

extern int16 pwm_l;
extern int16 pwm_r;

#endif /* CODE_JLNU_SERVO_H_ */

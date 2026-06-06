/*
 * JLNU_Servo.h
 *
 *  创建于: 2025年9月12日
 *      作者: 20461
 */
#include "zf_common_headfile.h"  // 包含逐飞库的核心头文件
#ifndef CODE_JLNU_SERVO_H_
#define CODE_JLNU_SERVO_H_

/* 四路舵机PWM初始化 */
void Servo_Four_Init();
/* 腿部高度与俯仰控制函数 */
void Servo_Leg_Control(int16 Left_Height, int16 Right_Height, int16 Leg_Pitch);

extern int16 pwm_l;   /* 左腿当前PWM高度值 */
extern int16 pwm_r;   /* 右腿当前PWM高度值 */

extern int16 High_Right_Point;  /* 右腿目标高度点 */
extern int16 High_Left_Point;   /* 左腿目标高度点 */

#endif /* CODE_JLNU_SERVO_H_ */

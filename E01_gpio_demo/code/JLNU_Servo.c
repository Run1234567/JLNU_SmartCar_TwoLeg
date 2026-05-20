/*
 * JLNU_Servo.c
 *
 *  Created on: 2025年9月12日
 *      Author: 20461
 */
#include "zf_common_headfile.h"  // 包含必要的头文件

int16 pwm_r = 300;
int16 pwm_l = 300;
int16 High_Left_Point = 200;
int16 High_Right_Point = 200;
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     四路舵机初始化函数
// 参数说明     void
// 返回参数     void
// 使用示例     Servo_Four_Init();
// 备注信息     初始化四个舵机PWM通道，设置频率为50Hz，周期为5000（对应20ms周期）
//-------------------------------------------------------------------------------------------------------------------
void Servo_Four_Init()
{
    pwm_init(TCPWM_CH25_P09_1, 50, 5000);  // 初始化原子0通道4，引脚P02_4，频率50Hz，周期5000
    pwm_init(TCPWM_CH24_P09_0, 50, 5000);  // 初始化原子0通道5，引脚P02_5，频率50Hz，周期5000
    pwm_init(TCPWM_CH31_P10_3, 50, 5000);  // 初始化原子0通道6，引脚P02_6，频率50Hz，周期5000
    pwm_init(TCPWM_CH30_P10_2, 50, 5000);  // 初始化原子0通道7，引脚P02_7，频率50Hz，周期5000
   
    Servo_Leg_Control(pwm_l, pwm_r,0);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     底盘双腿高度（舵机）同步控制函数
// 参数说明     Left_Height   左侧腿高度值，范围 -300 到 700
//              Right_Height  右侧腿高度值，范围 -300 到 700
// 返回参数     void
// 使用示例     Servo_Leg_Control(300, 300); // 左右腿同步伸长到 300
//             Servo_Leg_Control(500, 100); // 左腿高，右腿低（可用于抵抗离心力或过单边桥）
// 备注信息     统一调度4个舵机，确保机械腿连杆的对称与协调运动
//-------------------------------------------------------------------------------------------------------------------
void Servo_Leg_Control(int16 Left_Height, int16 Right_Height, int16 Leg_Pitch)
{
    // 定义机械连杆的绝对死区边界 (根据你的 550/950 基础值和 700/-300 范围推算)
    #define SERVO_PWM_MAX 1250
    #define SERVO_PWM_MIN 250

    // 1. 先计算出最终要喂给舵机的目标 PWM 值
    int16 left_front_pwm  = 550 + Left_Height + Leg_Pitch;
    int16 left_rear_pwm   = 950 - Left_Height + Leg_Pitch;
    
    int16 right_front_pwm = 950 - Right_Height - Leg_Pitch;
    int16 right_rear_pwm  = 550 + Right_Height - Leg_Pitch;

    // 2. 真正的保护机制：不管你前面怎么加减组合，最终 PWM 绝不允许越界！
    if(left_front_pwm > SERVO_PWM_MAX) left_front_pwm = SERVO_PWM_MAX;
    if(left_front_pwm < SERVO_PWM_MIN) left_front_pwm = SERVO_PWM_MIN;

    if(left_rear_pwm > SERVO_PWM_MAX) left_rear_pwm = SERVO_PWM_MAX;
    if(left_rear_pwm < SERVO_PWM_MIN) left_rear_pwm = SERVO_PWM_MIN;

    if(right_front_pwm > SERVO_PWM_MAX) right_front_pwm = SERVO_PWM_MAX;
    if(right_front_pwm < SERVO_PWM_MIN) right_front_pwm = SERVO_PWM_MIN;

    if(right_rear_pwm > SERVO_PWM_MAX) right_rear_pwm = SERVO_PWM_MAX;
    if(right_rear_pwm < SERVO_PWM_MIN) right_rear_pwm = SERVO_PWM_MIN;

    // 3. 安全输出给底层寄存器
    pwm_set_duty(TCPWM_CH25_P09_1, left_front_pwm);
    pwm_set_duty(TCPWM_CH24_P09_0, left_rear_pwm);
    pwm_set_duty(TCPWM_CH31_P10_3, right_front_pwm);
    pwm_set_duty(TCPWM_CH30_P10_2, right_rear_pwm);
}

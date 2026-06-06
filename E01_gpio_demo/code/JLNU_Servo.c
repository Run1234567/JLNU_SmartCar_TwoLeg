/*
 * JLNU_Servo.c
 *
 *  创建于: 2025年9月12日
 *  作者: 20461
 *  修改说明: 输入范围改为 0~1000，高度方向取反。
 */
#include "zf_common_headfile.h"  // 包含逐飞库的核心头文件

/* 全局修改：输入范围统一为 0~1000，初始默认高度为 400，中等高度，较安全 */
int16 pwm_r = 400;
int16 pwm_l = 400;
int16 High_Left_Point = 400;
int16 High_Right_Point = 400;

//-------------------------------------------------------------------------------------------------------------------
//  函数名     四路舵机初始化函数
//  参数说明   void
//  返回参数   void
//  使用示例   Servo_Four_Init();
//  备注信息   初始化四个舵机的PWM通道，控制频率为50Hz，周期为5000(对应20ms周期)
//-------------------------------------------------------------------------------------------------------------------
void Servo_Four_Init()
{
    pwm_init(TCPWM_CH25_P09_1, 50, 5000);  // 初始化左前舵机，引脚P09_1，频率50Hz，周期5000
    pwm_init(TCPWM_CH24_P09_0, 50, 5000);  // 初始化左后舵机，引脚P09_0，频率50Hz，周期5000
    pwm_init(TCPWM_CH31_P10_3, 50, 5000);  // 初始化右前舵机，引脚P10_3，频率50Hz，周期5000
    pwm_init(TCPWM_CH30_P10_2, 50, 5000);  // 初始化右后舵机，引脚P10_2，频率50Hz，周期5000

    Servo_Leg_Control(pwm_l, pwm_r, 0);
}

//-------------------------------------------------------------------------------------------------------------------
//  函数名     控制双腿高度及俯仰的协调控制函数
//  参数说明   Left_Height   左腿高度值，范围 0 ~ 1000，0为最低/收腿，依据机械结构而定
//             Right_Height  右腿高度值，范围 0 ~ 1000
//             Leg_Pitch     俯仰补偿值，正值为前倾，负值为后仰
//  返回参数   void
//  使用示例   Servo_Leg_Control(500, 500, 0); // 双腿同时设定为中间高度
//  备注信息   统一调度4个舵机的腿部运动，保证机器人双腿的对称协调运动
//-------------------------------------------------------------------------------------------------------------------
void Servo_Leg_Control(int16 Left_Height, int16 Right_Height, int16 Leg_Pitch)
{
    /* 定义机械臂的安全上下边界 */
    #define SERVO_PWM_MAX 1250
    #define SERVO_PWM_MIN 250

    /* 1. 全局修改：计算四个舵机的目标PWM值，Height 的方向全部取反，输入范围对应 0 ~ 1000 */
    int16 left_front_pwm  = 1250 - Left_Height + Leg_Pitch;   /* 左前腿PWM */
    int16 left_rear_pwm   = 250 + Left_Height + Leg_Pitch;    /* 左后腿PWM */

    int16 right_front_pwm = 250 + Right_Height - Leg_Pitch;   /* 右前腿PWM */
    int16 right_rear_pwm  = 1250 - Right_Height - Leg_Pitch;  /* 右后腿PWM */

    /* 2. 完善的限幅保护，防止前后腿的PWM值超出安全范围导致机械损坏 */
    if(left_front_pwm > SERVO_PWM_MAX) left_front_pwm = SERVO_PWM_MAX;
    if(left_front_pwm < SERVO_PWM_MIN) left_front_pwm = SERVO_PWM_MIN;

    if(left_rear_pwm > SERVO_PWM_MAX) left_rear_pwm = SERVO_PWM_MAX;
    if(left_rear_pwm < SERVO_PWM_MIN) left_rear_pwm = SERVO_PWM_MIN;

    if(right_front_pwm > SERVO_PWM_MAX) right_front_pwm = SERVO_PWM_MAX;
    if(right_front_pwm < SERVO_PWM_MIN) right_front_pwm = SERVO_PWM_MIN;

    if(right_rear_pwm > SERVO_PWM_MAX) right_rear_pwm = SERVO_PWM_MAX;
    if(right_rear_pwm < SERVO_PWM_MIN) right_rear_pwm = SERVO_PWM_MIN;

    /* 3. 安全写入底部的驱动代码 */
    pwm_set_duty(TCPWM_CH25_P09_1, left_front_pwm);   /* 设置左前舵机占空比 */
    pwm_set_duty(TCPWM_CH24_P09_0, left_rear_pwm);    /* 设置左后舵机占空比 */
    pwm_set_duty(TCPWM_CH31_P10_3, right_front_pwm);  /* 设置右前舵机占空比 */
    pwm_set_duty(TCPWM_CH30_P10_2, right_rear_pwm);   /* 设置右后舵机占空比 */
}
// /*
//  * JLNU_task0.c
//  *
//  * 任务模块实现文件
//  * 包含双足机器人的平衡、运动和转向控制逻辑
//  *
//  *  Created on: 2025年10月21日
//  *      Author: Run
//  */
// #include "zf_common_headfile.h"  /* 包含必要的头文件 */

// uint32 Timer_Time=0; /* 定时器计时分频标志位 */
// /* ==================== 系统配置和结构体定义 ==================== */
// #define MIN_LEG_LENGTH 0.04f        /* 最小腿部长度（米） */
// #define MAX_LEG_LENGTH 0.1f         /* 最大腿部长度（米） */


// /* PID限幅参数结构体 */
// typedef struct {
//     float kp_max, kp_min;
//     float ki_max, ki_min;
//     float kd_max, kd_min;
// } pid_limit_t;

// /* ==================== 全局变量定义 ==================== */
// /* 控制输出 */
// float balance_output = 0.0f;        /* 平衡控制输出 */
// float motion_output = 0.0f;          /* 运动控制输出 */
// int16_t turn_output = 0;             /* 转向控制输出 */
// int16_t final_left_duty = 0;        /* 左电机占空比 */
// int16_t final_right_duty = 0;       /* 右电机占空比 */
// int16_t angular_speed_output=0;     /* 角速度控制输出 */

// int16 Speed_Right=0;                /* 右电机实际速度 */
// int16 Speed_Left=0;                 /* 左电机实际速度 */
// int16 speed_sign = 0;               /* 速度符号标志 */
// /* 目标控制量 */
// int16_t target_speed = 0;            /* 目标速度 */
// int16_t target_turn = 0;             /* 目标转向 */

// /* ============ 空中检测相关的变量定义 ============ */
// float target_motor_Stand = 2.2f;       /* 目标站立角度（缺失的定义） */
// int16_t target_motor_angle = 0;        /* 目标腿部角度 */

// /* 空中检测的PID控制参数结构体 */
// PIDController air_roll_pid = {0};        /* 空中横滚PID */
// PIDController motor_direction = {0};    /* 电机方向PID */

// /* 系统状态 */
// float current_velocity = 0.0f;        /* 当前速度 */
// float current_angle = 0.0f;          /* 当前角度 */
// float current_gyro = 0.0f;             /* 当前角速度 */

// uint8_t go_falag = 1;                 /* 行走标志位 */


// /* ==================== 辅助函数实现 ==================== */



// /* 根据腿部高度调整PID参数 */
// void adjust_pid_based_on_leg_height(float current_leg_height) {
//     /* 计算腿部高度比例（归一化到0~1之间） */
//     float leg_ratio = (current_leg_height - MIN_LEG_LENGTH) / (MAX_LEG_LENGTH - MIN_LEG_LENGTH);
//     leg_ratio = fmaxf(fminf(leg_ratio, 1.0f), 0.0f);

//     /* 根据腿部高度调整站立目标角度 */
//     static float original_stand = 2.2f;
//     target_motor_Stand = original_stand * (1.0f + 0.4f * leg_ratio);

//     /* 调整角度环的增益参数 */
//     /* 此处使用新的角度环PID结构 */
//     // PID_Angular_Left.Kp = 6.0f * (1.0f - 0.3f * leg_ratio);
//     PID_Angular_Left.Kd = PID_Angular_Left.Kd * (1.0f + 0.6f * leg_ratio);
// }

// /* 空中检测函数 */
// bool is_airborne() {
//     const float threshold = 0.5f;
//     /* 检测Z轴加速度是否偏离重力加速度 */
//     return fabs(imu660ra_acc_z - 1.0f) > threshold;
// }

// /* 空中横滚控制 */
// void air_control() {
//     static float last_roll_error = 0.0f;

//     /* 获取当前姿态 */
//    float current_roll = 0; /* Angle_Forward.filtering_angle; */

//     /* 计算横滚误差 */
//     float roll_error = 0.0f - current_roll;
//     float roll_d_error = roll_error - last_roll_error;
//     last_roll_error = roll_error;

//     /* 计算控制量 */
//     float roll_control = -air_roll_pid.Kp * roll_error -
//                      air_roll_pid.Ki * roll_error -
//                      air_roll_pid.Kd * roll_d_error;

//     /* 限制输出范围 */
//     roll_control = limit_value_float(roll_control, -800.0f, 800.0f);

//     /* 应用控制量 */
//     final_left_duty = (int16_t)roll_control;
//     final_right_duty = (int16_t)roll_control;
// }

// /* 改进的转向控制函数 */
// float turn_control(float target_angle, float current_gyro) {
//     static float previous_error = 0.0f;
//     float error = target_angle - current_gyro;
//     float derivative = error - previous_error;
//     previous_error = error;

//     /* 使用非线性增量增强转向响应（参考之前的控制参考） */
//     float control_output = -motor_direction.Kp * error -
//                           error * fabs(error) * motor_direction.Ki -
//                           motor_direction.Kd * derivative;

//     return limit_value_float(control_output, -2000.0f, 2000.0f);
// }

// /* ==================== 初始化函数 ==================== */
// /* ==================== 初始化函数 ==================== */
// void control_system_init(void) {
//     /* 初始化空中横滚PID参数 */
//     air_roll_pid.Kp = 45.0f;
//     air_roll_pid.Ki = 0.0f;
//     air_roll_pid.Kd = 2.0f;

//     /* 初始化电机方向PID参数 */
//     motor_direction.Kp = 0.044f;
//     motor_direction.Ki = 0.00086f;
//     motor_direction.Kd = 0.85f;
// }



// /* ==================== 主控制任务 ==================== */
// void task0(void)
// {
//     static float speed_integral = 0.0f;
//     static float last_speed_error = 0.0f;
//     static float last_angle_error = 0.0f;
//     static float speed_up = 0.0f;
//     Timer_Time++;
//     IMU660_GetData();

//     /* 20ms时间片 - 速度和转向控制 */
//     if(Timer_Time % 20 == 0)
//     {
//         Speed_Left = motor_value.receive_left_speed_data;
//         Speed_Right = -motor_value.receive_right_speed_data;

//         /* 计算当前速度 */
//         current_velocity = (Speed_Left + Speed_Right) / 2.0f;

//         /* 速度误差计算 */
//         float a = speed_sign - current_velocity;
//         float speed_error = target_speed - current_velocity;
//         float speed_d_error = speed_error - last_speed_error;
//         last_speed_error = speed_error;

//         /* 失步控制 */
//         if(go_falag)
//         {
//             /* 加速阶段 */
//             if(fabs(a) < 5) {
//                 speed_integral += speed_error * 0.02f; /* 20ms作为dt进行积分 */
//                 speed_integral = limit_value_float(speed_integral, -200.0f, 200.0f);
//                 speed_up = 0.0f;
//                 /* 逼近目标时，速度平滑 */
//                 target_speed = smooth_motion(target_speed, speed_sign);
//             }
//             /* 减速 */
//             else if(fabs(a) > 5 && fabs(a) < 10) {
//               if(a > 0) {
//                 target_speed -= 0.5;
//               } else {
//                 target_speed += 0.5;
//               }
//             }
//             else if(fabs(a) > 10 && fabs(a) < 20) {
//               if(a > 0) {
//                 target_speed -= 1;
//               } else {
//                 target_speed += 1;
//               }
//             }
//             else if(fabs(a) > 20 && fabs(a) < 50) {
//               if(a > 0) {
//                 target_speed -= 1.5;
//               } else {
//                 target_speed += 1.5;
//               }
//             }
//             else if(fabs(a) > 50 && fabs(a) < 100) {
//               if(a > 0) {
//                 target_speed -= 2;
//               } else {
//                 target_speed += 2;
//               }
//             }
//             else if(fabs(a) > 100 && fabs(a) < 300) {
//               if(a > 0) {
//                 target_speed -= 5;
//               } else {
//                 target_speed += 5;
//               }
//             }
//             else if(fabs(a) > 300) {
//               if(a > 0) {
//                 target_speed -= 10;
//               } else {
//                 target_speed += 10;
//               }
//             }
//         }
//         if(target_speed > 1000 || target_speed < -1000)
//         {
//           if(target_speed > 0) target_speed = 1000;
//           else target_speed = -1000;
//         }
//         /* 加速阶段 */
// //        else
// //        {
// //            speed_up = 0.0f;
// //            if(speed_error < 30) go_falag = 1;
// //        }
//         motion_output = PID_Speed_All_Left.Kp * speed_error +
//                        PID_Speed_All_Left.Ki * speed_integral + speed_up;



//         wireless_uart_send_decimal(a);
//         wireless_uart_send_string(",");
//         wireless_uart_send_decimal(target_speed);
//         wireless_uart_send_string(",");
//         wireless_uart_send_decimal(motion_output);
//         wireless_uart_send_string(",");
//         wireless_uart_send_decimal(speed_integral);

//         wireless_uart_send_string("\n");

//         /* 转换为角度偏移量 */
//         motion_output = limit_value_float(motion_output, -600.0f, 500.0f);


//     }

//     /* 5ms时间片 - 姿态控制 */
//     if(Timer_Time % 5 == 0)
//     {
//         Angle_Calculation();
//       //  current_angle = Angle_Forward.filtering_angle;
//         current_gyro = imu660ra_gyro_x;

//         /* 角度控制 */
//         float angle_error = motion_output - current_angle;
//         float angle_d_error = angle_error - last_angle_error;
//         last_angle_error = angle_error;

//         /* 平衡控制 */
//         balance_output = PID_Calculate(&PID_Angular_Left, current_angle, motion_output + 300.0f); /* 270为角度补偿 */

//     }

//     /* 1ms时间片 - 角速度环 */
//      angular_speed_output = (int16_t)PID_Calculate(&PID_Angular_Speed_Left, current_gyro, balance_output);

//     /* 控制量合成（改进版本） */
//     final_left_duty = -angular_speed_output - turn_output;
//     final_right_duty = angular_speed_output - turn_output;

//     /* 改进的限幅逻辑，限制输出范围 */
//     final_left_duty = limit_value(final_left_duty, -3000, 3000);
//     final_right_duty = limit_value(final_right_duty, -3000, 3000);

//     small_driver_set_duty(final_left_duty, final_right_duty);
// }

// /* 改进的限幅逻辑，修改逻辑修正 */
// float limit_value_float(float value, float min_val, float max_val)
// {
//     if(value > max_val) return max_val;
//     if(value < min_val) return min_val;
//     return value;
// }

// int16_t limit_value(int16_t value, int16_t min_val, int16_t max_val)
// {
//     if(value > max_val) return max_val;
//     if(value < min_val) return min_val;
//     return value;
// }

// /* 安全的目标运动设置函数 */
// void set_target_motion(int16_t speed, int16_t turn)
// {
//     static float target_speed_smooth = 0.0f;
//     speed_sign = speed;
//     /* 速度渐变（防止突变） */
//     target_speed_smooth = target_speed_smooth * 0.8f + speed * 0.2f;
//     target_speed = limit_value_float(target_speed_smooth, -3000, 3000);

//     turn_output = limit_value(turn, -500, 500);
// }

// /* 平滑函数 - 改进版本 */
// float smooth_motion(float current_target, float new_target)
// {
//     static float smooth_target = 0.0f;

//     /* 如果是第一次调用，直接设为当前目标 */
//     if (smooth_target == 0.0f && current_target != 0.0f) {
//         smooth_target = current_target;
//     }

//     /* 平滑滤波（0.7-0.9范围调整平滑度，值越大越平滑） */
//     float smoothing_factor = 0.9f;  /* 可调整的平滑系数 */
//     smooth_target = smooth_target * smoothing_factor + new_target * (1.0f - smoothing_factor);

//     return smooth_target;
// }

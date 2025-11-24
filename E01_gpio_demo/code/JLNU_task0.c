/*
 * JLNU_task0.c
 *
 *  Created on: 2025年10月21日
 *      Author: 凯
 */
#include "zf_common_headfile.h"  // 包含必要的头文件

uint32 Timer_Time=0;//定时器切分标志位
// ==================== 系统配置和结构定义 ====================
#define MIN_LEG_LENGTH 0.04f        // 最小腿部长度
#define MAX_LEG_LENGTH 0.1f         // 最大腿部长度


// PID限幅参数结构
typedef struct {
    float kp_max, kp_min;
    float ki_max, ki_min;
    float kd_max, kd_min;
} pid_limit_t;

// ==================== 全局变量声明 ====================
// 控制输出
float balance_output = 0.0f;        // 平衡控制输出
float motion_output = 0.0f;          // 运动控制输出
int16_t turn_output = 0;             // 转向控制输出
int16_t final_left_duty = 0;        // 最终左轮占空比
int16_t final_right_duty = 0;       // 最终右轮占空比
int16_t angular_speed_output=0;

int16 Speed_Right=0;
int16 Speed_Left=0;
int16 speed_sign = 0;
// 目标控制量
int16_t target_speed = 0;            // 目标速度
int16_t target_turn = 0;             // 目标转向

// ============ 添加缺失的变量定义 ============
float target_motor_Stand = 2.2f;       // 目标电机站立角度（缺失的定义）
int16_t target_motor_angle = 0;        // 目标电机角度

// 新增缺失的PID控制器定义
// 新增缺失的PID控制器定义
PIDController air_roll_pid = {0};        // 空中控制器PID
PIDController motor_direction = {0};    // 方向控制PID

// 系统状态
float current_velocity = 0.0f;        // 当前速度
float current_angle = 0.0f;          // 当前角度
float current_gyro = 0.0f;             // 当前角速度

uint8_t go_falag = 1;


// ==================== 核心函数实现 ====================



// 腿部高度自适应PID调整
void adjust_pid_based_on_leg_height(float current_leg_height) {
    // 计算腿部高度比例（0到1之间）
    float leg_ratio = (current_leg_height - MIN_LEG_LENGTH) / (MAX_LEG_LENGTH - MIN_LEG_LENGTH);
    leg_ratio = fmaxf(fminf(leg_ratio, 1.0f), 0.0f);

    // 根据腿部高度调整平衡目标角度
    static float original_stand = 2.2f;
    target_motor_Stand = original_stand * (1.0f + 0.4f * leg_ratio);

    // 调整角度环和陀螺仪环参数
    // 这里使用您实际的角度环PID结构体
    // PID_Angular_Left.Kp = 6.0f * (1.0f - 0.3f * leg_ratio);
    PID_Angular_Left.Kd = PID_Angular_Left.Kd * (1.0f + 0.6f * leg_ratio);
}

// 腾空检测
bool is_airborne() {
    const float threshold = 0.5f;
    // 检测Z轴加速度是否偏离重力加速度
    return fabs(imu660ra_acc_z - 1.0f) > threshold;
}

// 空中控制器
void air_control() {
    static float last_roll_error = 0.0f;

    // 获取当前姿态
    float current_roll = Angle_Forward.filtering_angle;

    // 计算误差
    float roll_error = 0.0f - current_roll;
    float roll_d_error = roll_error - last_roll_error;
    last_roll_error = roll_error;

    // 计算控制输出
    float roll_control = -air_roll_pid.Kp * roll_error -
                     air_roll_pid.Ki * roll_error -
                     air_roll_pid.Kd * roll_d_error;

    // 限制输出范围
    roll_control = limit_value_float(roll_control, -800.0f, 800.0f);

    // 应用控制
    final_left_duty = (int16_t)roll_control;
    final_right_duty = (int16_t)roll_control;
}

// 改进的转向控制
float turn_control(float target_angle, float current_gyro) {
    static float previous_error = 0.0f;
    float error = target_angle - current_gyro;
    float derivative = error - previous_error;
    previous_error = error;

    // 使用非线性增强转向响应（仿照参考代码）
    float control_output = -motor_direction.Kp * error -
                          error * fabs(error) * motor_direction.Ki -
                          motor_direction.Kd * derivative;

    return limit_value_float(control_output, -2000.0f, 2000.0f);
}

// ==================== 初始化函数 ====================
// ==================== 初始化函数 ====================
void control_system_init(void) {
    // 初始化空中控制PID参数
    air_roll_pid.Kp = 45.0f;
    air_roll_pid.Ki = 0.0f;
    air_roll_pid.Kd = 2.0f;

    // 初始化方向控制PID参数
    motor_direction.Kp = 0.044f;
    motor_direction.Ki = 0.00086f;
    motor_direction.Kd = 0.85f;
}



// ==================== 主控制任务 ====================
void task0(void)
{
    static float speed_integral = 0.0f;
    static float last_speed_error = 0.0f;
    static float last_angle_error = 0.0f;
    static float speed_up = 0.0f;

    Timer_Time++;
    IMU660_GetData();

    // 20ms片段 - 速度和转向控制
    if(Timer_Time % 20 == 0)
    {
        Speed_Left = motor_value.receive_left_speed_data;
        Speed_Right = -motor_value.receive_right_speed_data;

        // 计算当前速度
        current_velocity = (Speed_Left + Speed_Right) / 2.0f;



        // 速度误差计算
        float a = speed_sign -current_velocity;
        float speed_error = target_speed - current_velocity;
        float speed_d_error = speed_error - last_speed_error;
        last_speed_error = speed_error;

        // 失速控制
        if(go_falag)
        {

          //正常情况
            if(fabs(a) < 5) {
                speed_integral += speed_error * 0.02f; // 20ms 的 dt（积分）
                speed_integral = limit_value_float(speed_integral, -200.0f, 200.0f);
                speed_up =0.0f;
                //渐变目标，减少抖动
                target_speed  =smooth_motion(target_speed,speed_sign);
            }
            //超速
            else if(fabs(a) > 5 && fabs(a)< 10) {
              if(a > 0)
              {
                target_speed -= 0.5;
              }
              else
              {
                target_speed += 0.5;
              }

            }
            else if(fabs(a) > 10 && fabs(a)< 20) {
              if(a > 0)
              {
                target_speed -= 1;
              }
              else
              {
                target_speed += 1;
              }
            }
              else if(fabs(a) > 20 && fabs(a)< 50) {
              if(a > 0)
              {
                target_speed -= 1.5;
              }
              else
              {
                target_speed += 1.5;
              }
              }
             else if(fabs(a) > 50 && fabs(a)< 100) {
              if(a > 0)
              {
                target_speed -= 2;
              }
              else
              {
                target_speed += 2;
              }
              }
            else if(fabs(a) > 100 && fabs(a)< 300) {
              if(a > 0)
              {
                target_speed -= 5;
              }
              else
              {
                target_speed += 5;
              }
              }
             else if(fabs(a) > 300 ) {
              if(a > 0)
              {
                target_speed -= 10;
              }
              else
              {
                target_speed += 10;
              }
              }
            }
        if(target_speed >1000 || target_speed < -1000)
        {
          if(target_speed>0)target_speed = 1000;
          else target_speed = -1000;
        }




        //启动阶段
//        else
//        {
//            speed_up =0.0f;
//            if(speed_error < 30)go_falag = 1;
//        }



        motion_output = PID_Speed_All_Left.Kp * speed_error +
                       PID_Speed_All_Left.Ki * speed_integral+ speed_up  ;



        wireless_uart_send_decimal(a);
        wireless_uart_send_string(",");
        wireless_uart_send_decimal(target_speed);
        wireless_uart_send_string(",");
        wireless_uart_send_decimal(motion_output);
        wireless_uart_send_string(",");
        wireless_uart_send_decimal(speed_integral);

        wireless_uart_send_string("\n");

        // 转换为角度偏置
        motion_output = limit_value_float(motion_output, -600.0f, 500.0f);


    }

    // 5ms片段 - 姿态控制
    if(Timer_Time % 5 == 0)
    {
        Angle_Calculation();
        current_angle = Angle_Forward.filtering_angle;
        current_gyro = imu660ra_gyro_x;

        // 角度误差计算
        float angle_error = motion_output- current_angle;
        float angle_d_error = angle_error - last_angle_error;
        last_angle_error = angle_error;

        // 平衡控制
        balance_output = PID_Calculate(&PID_Angular_Left, current_angle, motion_output + 300.0f);/*270为角度补偿*/

    }

    // 1ms片段 - 角速度环
     angular_speed_output = (int16_t)PID_Calculate(&PID_Angular_Speed_Left, current_gyro, balance_output);

    // 控制量合成（改进版本）
    final_left_duty = -angular_speed_output - turn_output;
    final_right_duty = angular_speed_output - turn_output;

    // 改进的限幅函数（修复错误）
    final_left_duty = limit_value(final_left_duty, -3000, 3000);
    final_right_duty = limit_value(final_right_duty, -3000, 3000);

    small_driver_set_duty(final_left_duty, final_right_duty);
}

// 改进的限幅函数（修复逻辑错误）
float limit_value_float(float value, float min_val, float max_val)
{
    if(value > max_val) return max_val;
    if(value < min_val) return min_val;
    return value;
}

int16_t limit_value(int16_t value, int16_t min_val, int16_t max_val)
{
    if(value > max_val) return max_val;
    if(value < min_val) return min_val;
    return value;
}

// 安全目标设置函数
void set_target_motion(int16_t speed, int16_t turn)
{
    static float target_speed_smooth = 0.0f;
    speed_sign = speed;
    // 速度渐变（避免突变）
    target_speed_smooth = target_speed_smooth * 0.8f + speed * 0.2f;
    target_speed = limit_value_float(target_speed_smooth, -3000, 3000);

    turn_output = limit_value(turn, -500, 500);
}



// 渐变函数 - 基本版本
float smooth_motion(float current_target, float new_target)
{
    static float smooth_target = 0.0f;

    // 如果是第一次调用，直接设为当前目标
    if (smooth_target == 0.0f && current_target != 0.0f) {
        smooth_target = current_target;
    }

    // 平滑过渡：0.7-0.9控制平滑度，值越大越平滑
    float smoothing_factor = 0.9f;  // 可调节的平滑系数
    smooth_target = smooth_target * smoothing_factor + new_target * (1.0f - smoothing_factor);

    return smooth_target;
}


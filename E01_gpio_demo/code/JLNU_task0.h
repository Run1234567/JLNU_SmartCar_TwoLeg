// /*
//  * JLNU_task0.h
//  *
//  * 任务模块头文件
//  * 定义了控制任务相关的函数声明和全局变量
//  *
//  *  Created on: 2025年10月21日
//  *      Author: Run
//  */

// #ifndef CODE_JLNU_TASK0_H_
// #define CODE_JLNU_TASK0_H_

// /* 任务函数声明 */
// void task0(void);                                     /* 主控制任务（平衡+运动+转向） */
// void task1(void);                                     /* 辅助任务 */
// void set_target_motion(int16_t speed, int16_t turn);  /* 设置目标运动参数 */
// float limit_value_float(float value, float min_val, float max_val);  /* 浮点限幅函数 */
// int16_t limit_value(int16_t value, int16_t min_val, int16_t max_val); /* 整数限幅函数 */
// /* 平滑函数 */
// float smooth_motion(float current_target, float new_target);  /* 运动目标平滑函数 */

// /* 全局变量声明 */
// extern int16 Speed_Left;            /* 左电机实际速度 */
// extern int16 Speed_Right;           /* 右电机实际速度 */
// extern int16 Moter_L;               /* 左电机输出 */
// extern int16 Moter_R;               /* 右电机输出 */
// extern float motion_output;         /* 运动控制输出 */

// extern int16_t turn_output;         /* 转向控制输出 */

// extern int16 Angle_Out_Left;        /* 左侧角度输出 */
// extern float balance_output;        /* 平衡控制输出 */
// extern int16_t final_left_duty;     /* 左电机最终占空比 */
// extern int16_t final_right_duty;    /* 右电机最终占空比 */
// #endif /* CODE_JLNU_TASK0_H_ */

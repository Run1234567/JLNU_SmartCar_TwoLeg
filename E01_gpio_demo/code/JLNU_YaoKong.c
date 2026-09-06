/*
 * JLNU_YaoKong.c
 *
 *  遥控器模块实现
 *  包含遥控器通道值到速度/角度的映射，以及SWA/SWB/CH5/CH6开关状态检测
 */

#include "zf_common_headfile.h"

/**
 * 速度转换函数（带安全限幅）
 * 确保输出速度值严格限制在 -400 到 400 之间
 * 输入：遥控器原始通道值(通常为1000~2000)
 * 输出：限幅后的速度值(-400~+400)
 */
int speed_convert_clamped(int x) {
    int speed = (x >> 1) - 500;  /* 将通道值右移1位后减去500，映射到速度范围 */

    /* 限制最大倒车/负向速度 */
    if (speed < -800) {
        return -800;
    }
    /* 限制最大前进/正向速度 */
    if (speed > 800) {
        return 800;
    }

    return speed;
}

/**
 * 角度转换函数（带安全限幅，输出浮点数）
 * 将通道值 [184, 1784] 映射到 [-1.0, 1.0] 度
 * 1000 对应 0.0（中位）
 * 输入：遥控器原始通道
 * 输出：限幅后的角度值(-1.0~+1.0)
 */
float angle_convert_clamped(int x) {
    /* 核心计算：先减去中心偏移，再除以800.0f进行缩放 */
    float angle = (x - 1000) / 800.0f;

    /* 限制最小角度 */
    if (angle < -1.0f) {
        return -1.0f;
    }
    /* 限制最大角度 */
    if (angle > 1.0f) {
        return 1.0f;
    }

    return angle;
}

/**
 * SWA三档开关状态变化检测函数 (对应 channel[2])
 * 只要状态发生翻转就返回1，否则返回0
 * 使用 -1 作为"未初始化"的标志，因为通道有效值只有 192 和 1792
 */
int SWA_Down(void) {
    static int last_state = -1;  /* 静态变量，记录上一次SWA的状态 */

    /* 获取当前通道值 */
    int current_state = uart_receiver.channel[2];

    /* 1. 如果是强制第一次进入该函数，同步初始状态，直接返回0，不算变化 */
    if (last_state == -1) {
        last_state = current_state;
        return 0;
    }

    /* 2. 之后的判断中，如果当前值和上一次的值不一样，说明开关被拨动了 */
    if (current_state != last_state) {
        last_state = current_state; /* 重新记录 */
        return 1;                   /* 返回1，有变化 */
    }

    /* 3. 状态没变化 */
    return 0; /* 返回0，无变化 */
}

/**
 * 获取SWB三档开关的当前档位
 * 对应 channel[3]
 * 返回值：0=下档(192)，1=中档(992)，2=上档(1792)，-1=未知
 */
int get_SWB_state(void)
{
    int val = uart_receiver.channel[3];

    if (val == 192) {
        return 0;  /* 下档 */
    } else if (val == 992) {
        return 1;  /* 中档 */
    } else if (val == 1792) {
        return 2;  /* 上档 */
    }

    /* 如果数据不在预期的固定值范围内，返回 -1 表示异常 */
    return -1;
}

/**
 * CH5 通道状态变化检测 (对应 channel[4])
 * 只要状态发生翻转就返回1，否则返回0
 */
int CH5_Down(void) {
    static int last_state = -1;  /* 静态变量，只用来记录CH5的上一次状态 */

    /* 获取当前通道值 */
    int current_state = uart_receiver.channel[4];

    /* 首次调用中，同步初始状态，不算变化 */
    if (last_state == -1) {
        last_state = current_state;
        return 0;
    }

    /* 状态发生了变化 */
    if (current_state != last_state) {
        last_state = current_state; /* 重新记录 */
        return 1;                   /* 有变化 */
    }

    /* 状态无变化 */
    return 0;
}

/**
 * CH6 通道状态变化检测 (对应 channel[5])
 * 只要状态发生翻转就返回1，否则返回0
 */
int CH6_Down(void) {
    static int last_state = -1;  /* 静态变量，只用来记录CH6的上一次状态 */

    /* 获取当前通道值 */
    int current_state = uart_receiver.channel[5];

    /* 首次调用中，同步初始状态，不算变化 */
    if (last_state == -1) {
        last_state = current_state;
        return 0;
    }

    /* 状态发生了变化 */
    if (current_state != last_state) {
        last_state = current_state; /* 重新记录 */
        return 1;                   /* 有变化 */
    }

    /* 状态无变化 */
    return 0;
}
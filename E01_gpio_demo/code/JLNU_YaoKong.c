#include "zf_common_headfile.h"
/**
 * 速度转换函数（带安全边界）
 * 确保输出速度严格控制在 -400 到 400 之间
 */
int speed_convert_clamped(int x) {
    int speed = (x >> 1) - 492;
    
    // 限制最大倒车/反向速度
    if (speed < -400) {
        return -400;
    }
    // 限制最大前进/正向速度
    if (speed > 400) {
        return 400;
    }
    
    return speed;
}

/**
 * 角度转换函数（带安全边界，浮点输出）
 * 将输入值 [184, 1784] 映射到 [-2.0, 2.0]
 * 984 对应 0.0
 */
float angle_convert_clamped(int x) {
    // 核心计算：先减去中位偏移，然后除以 400.0f
    float angle = (x - 984) / 800.0f;
    
    // 限制最小输出
    if (angle < -1.0f) {
        return -1.0f;
    }
    // 限制最大输出
    if (angle > 1.0f) {
        return 1.0f;
    }
    
    return angle;
}
int SWA_Down(void) {
    // 使用 -1 作为“未初始化”的标志，因为通道有效值只有 192 和 1792
    static int last_state = -1; 
    
    // 获取当前最新值
    int current_state = uart_receiver.channel[2];
    
    // 1. 如果是开机第一次运行该函数，先同步初始状态，直接返回 0（不触发）
    if (last_state == -1) {
        last_state = current_state;
        return 0; 
    }
    
    // 2. 之后的正常运行：如果当前值和上一次的值不一样，说明被拨动了
    if (current_state != last_state) {
        last_state = current_state; // 更新记录
        return 1;                   // 返回 1（有变化）
    }
    
    // 3. 状态没变化
    return 0; // 返回 0（无变化）
}
int get_SWB_state(void)
{
    int val = uart_receiver.channel[3];
    
    if (val == 192) {
        return 0;
    } else if (val == 992) {
        return 1;
    } else if (val == 1792) {
        return 2;
    }
    
    // 如果数据不是这三个固定值，返回 -1 报错
    return -1; 
}

/**
 * CH5 拨杆状态变化检测 (对应 channel[4])
 * 只要状态发生翻转就返回 1，否则返回 0
 */
int CH5_Down(void) {
    // 独立的静态变量，只负责记录 CH5 的上一次状态
    static int last_state = -1; 
    
    // 获取当前最新值
    int current_state = uart_receiver.channel[4];
    
    // 开机首次运行，同步初始状态，不触发
    if (last_state == -1) {
        last_state = current_state;
        return 0; 
    }
    
    // 状态发生变化
    if (current_state != last_state) {
        last_state = current_state; // 更新记录
        return 1;                   // 触发
    }
    
    // 状态无变化
    return 0; 
}

/**
 * CH6 拨杆状态变化检测 (对应 channel[5])
 * 只要状态发生翻转就返回 1，否则返回 0
 */
int CH6_Down(void) {
    // 独立的静态变量，只负责记录 CH6 的上一次状态
    static int last_state = -1; 
    
    // 获取当前最新值
    int current_state = uart_receiver.channel[5];
    
    // 开机首次运行，同步初始状态，不触发
    if (last_state == -1) {
        last_state = current_state;
        return 0; 
    }
    
    // 状态发生变化
    if (current_state != last_state) {
        last_state = current_state; // 更新记录
        return 1;                   // 触发
    }
    
    // 状态无变化
    return 0; 
}

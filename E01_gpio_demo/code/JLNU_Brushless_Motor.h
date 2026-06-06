/*
 * JLNU_Brushless_Motor.h
 *
 *  Created on: 2025年9月9日
 *      Author: RUN
 */

#ifndef CODE_JLNU_BRUSHLESS_MOTOR_H_
#define CODE_JLNU_BRUSHLESS_MOTOR_H_



#include "zf_common_headfile.h"


#define SMALL_DRIVER_UART                       (UART_4        )       /* 无刷电机驱动串口号 */

#define SMALL_DRIVER_BAUDRATE                   (460800        )       /* 无刷电机驱动波特率 */

#define SMALL_DRIVER_RX                         (UART4_RX_P14_0)      /* 无刷电机驱动串口接收引脚 */

#define SMALL_DRIVER_TX                         (UART4_TX_P14_1)      /* 无刷电机驱动串口发送引脚 */

typedef struct
{
    uint8 send_data_buffer[7];                  // 发送缓冲区

    uint8 receive_data_buffer[7];               // 接收缓冲区

    uint8 receive_data_count;                   // 接收计数

    uint8 sum_check_data;                       // 校验位

    int16 receive_left_speed_data;              // 接收到的左电机速度数据

    int16 receive_right_speed_data;             // 接收到的右电机速度数据

}small_device_value_struct;

extern small_device_value_struct motor_value;   /* 电机通信数据结构体实例 */



void uart_control_callback(void);                                   // 无刷电机 串口接收回调函数

void small_driver_set_duty(int16 left_duty, int16 right_duty);      // 无刷电机 设置占空比

void small_driver_get_speed(void);                                  // 无刷电机 获取速度信息

void small_driver_uart_init(void);                                  // 无刷电机 通信初始化


#endif /* CODE_JLNU_BRUSHLESS_MOTOR_H_ */

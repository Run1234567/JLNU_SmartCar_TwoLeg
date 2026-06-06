/*
 * JLNU_Brushless_Moter.c
 *
 *  Created on: 2025年9月9日
 *      Author: Run
 */
#include "zf_common_headfile.h"

small_device_value_struct motor_value;      // 电机通信数据结构体
//-------------------------------------------------------------------------------------------------------------------
// 函数名     无刷电机 串口接收回调函数
// 参数说明   void
// 返回参数   void
// 使用示例   uart_control_callback();
// 备注信息   串口接收到的速度数据  该函数需要在对应的串口接收中断中调用
//-------------------------------------------------------------------------------------------------------------------
void uart_control_callback(void)
{
    uint8 receive_data;                                                                     // 接收临时变量

    if(uart_query_byte(SMALL_DRIVER_UART, &receive_data))                                   // 查询接收数据
    {
        if(receive_data == 0xA5 && motor_value.receive_data_buffer[0] != 0xA5)              // 判断是否收到帧头 并且 当前缓冲区内是否正确的帧头
        {
            motor_value.receive_data_count = 0;                                             // 未收到帧头或未正确收到帧头，重新计数
        }

        motor_value.receive_data_buffer[motor_value.receive_data_count ++] = receive_data;  // 存储串口数据

        if(motor_value.receive_data_count >= 7)                                             // 判断是否接收到指定长度的数据
        {
            if(motor_value.receive_data_buffer[0] == 0xA5)                                  // 判断帧头是否正确
            {

                motor_value.sum_check_data = 0;                                             // 清空校验位变量

                for(int i = 0; i < 6; i ++)
                {
                    motor_value.sum_check_data += motor_value.receive_data_buffer[i];       // 重新计算校验位
                }

                if(motor_value.sum_check_data == motor_value.receive_data_buffer[6])        // 校验数据准确
                {

                    if(motor_value.receive_data_buffer[1] == 0x02)                          // 判断是否正确接收到 速度数据 功能码
                    {
                        motor_value.receive_left_speed_data  = (((int)motor_value.receive_data_buffer[2] << 8) | (int)motor_value.receive_data_buffer[3]);  // 左电机转速数据

                        motor_value.receive_right_speed_data = (((int)motor_value.receive_data_buffer[4] << 8) | (int)motor_value.receive_data_buffer[5]);  // 右电机转速数据
                    }

                    motor_value.receive_data_count = 0;                                     // 清空接收数据计数值

                    memset(motor_value.receive_data_buffer, 0, 7);                          // 清空接收数据缓冲区
                }
                else
                {
                    motor_value.receive_data_count = 0;                                     // 清空接收数据计数值

                    memset(motor_value.receive_data_buffer, 0, 7);                          // 清空接收数据缓冲区
                }
            }
            else
            {
                motor_value.receive_data_count = 0;                                         // 清空接收数据计数值

                memset(motor_value.receive_data_buffer, 0, 7);                              // 清空接收数据缓冲区
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数名     无刷电机 设置占空比
// 参数说明   left_duty       左电机占空比  范围 -10000 ~ 10000  负数为反转
// 参数说明   right_duty      右电机占空比  范围 -10000 ~ 10000  负数为反转
// 返回参数   void
// 使用示例   small_driver_set_duty(1000, -1000);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void small_driver_set_duty(int16 left_duty, int16 right_duty)
{
    motor_value.send_data_buffer[0] = 0xA5;                                         // 设置帧头

    motor_value.send_data_buffer[1] = 0X01;                                         // 设置功能码

    motor_value.send_data_buffer[2] = (uint8)((left_duty & 0xFF00) >> 8);           // 填充 左电机占空比 的高八位

    motor_value.send_data_buffer[3] = (uint8)(left_duty & 0x00FF);                  // 填充 左电机占空比 的低八位

    motor_value.send_data_buffer[4] = (uint8)((right_duty & 0xFF00) >> 8);          // 填充 右电机占空比 的高八位

    motor_value.send_data_buffer[5] = (uint8)(right_duty & 0x00FF);                 // 填充 右电机占空比 的低八位

    motor_value.send_data_buffer[6] = 0;                                            // 清校验和数据

    for(int i = 0; i < 6; i ++)
    {
        motor_value.send_data_buffer[6] += motor_value.send_data_buffer[i];         // 计算校验位
    }

    uart_write_buffer(SMALL_DRIVER_UART, motor_value.send_data_buffer, 7);                     // 发送设置占空比的 字节包 数据
}

//-------------------------------------------------------------------------------------------------------------------
// 函数名     无刷电机 获取速度信息
// 参数说明   void
// 返回参数   void
// 使用示例   small_driver_get_speed();
// 备注信息   需要发一次 电机会通过串口发送速度信息(默认10ms)
//-------------------------------------------------------------------------------------------------------------------
void small_driver_get_speed(void)
{
    motor_value.send_data_buffer[0] = 0xA5;                                         // 设置帧头

    motor_value.send_data_buffer[1] = 0X02;                                         // 设置功能码

    motor_value.send_data_buffer[2] = 0x00;                                         // 数据位清零

    motor_value.send_data_buffer[3] = 0x00;                                         // 数据位清零

    motor_value.send_data_buffer[4] = 0x00;                                         // 数据位清零

    motor_value.send_data_buffer[5] = 0x00;                                         // 数据位清零

    motor_value.send_data_buffer[6] = 0xA7;                                         // 预设校验位

    uart_write_buffer(SMALL_DRIVER_UART, motor_value.send_data_buffer, 7);                     // 发送获取转速数据的 字节包 数据
}


//-------------------------------------------------------------------------------------------------------------------
// 函数名     无刷电机 结构体初始化
// 参数说明   void
// 返回参数   void
// 使用示例   small_driver_init();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void small_driver_init(void)
{
    memset(motor_value.send_data_buffer, 0, 7);                             // 清空发送数据缓冲区

    memset(motor_value.receive_data_buffer, 0, 7);                          // 清空接收数据缓冲区

    motor_value.receive_data_count          = 0;

    motor_value.sum_check_data              = 0;

    motor_value.receive_right_speed_data    = 0;

    motor_value.receive_left_speed_data     = 0;
}


//-------------------------------------------------------------------------------------------------------------------
// 函数名     无刷电机 通信初始化
// 参数说明   void
// 返回参数   void
// 使用示例   small_driver_uart_init();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void small_driver_uart_init(void)
{
    uart_init(SMALL_DRIVER_UART, SMALL_DRIVER_BAUDRATE, SMALL_DRIVER_RX, SMALL_DRIVER_TX);      // 串口初始化

    uart_rx_interrupt(SMALL_DRIVER_UART, 1);                                                    // 使能串口接收中断

    small_driver_init();                                                                        // 结构体数据初始化

    small_driver_set_duty(0, 0);                                                                // 设置0占空比

    small_driver_get_speed();                                                                   // 获取实时速度数据
}





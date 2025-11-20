/*
 * JLNU_UART_Wireless.c
 *
 *  Created on: 2025年9月8日
 *      Author: RUN
 */
#include "zf_common_headfile.h"

uint8 data_buffer[32];
uint8 data_len;
//-------------------------------------------------------------------------------------------------------------------
// 函数简介       串口初始化函数，引脚修改 DEBUG_UART_INDEX DEBUG_UART_TX_PIN  DEBUG_UART_RX_PIN
// 参数说明       void
// 返回参数       void
// 使用示例       UART_Init();
//-------------------------------------------------------------------------------------------------------------------
void UART_Wireless_Init()
{
    while(wireless_uart_init())                                                    // 判断是否通过初始化
    {
        system_delay_ms(100);                                               // 短延时快速闪灯表示异常
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       定时器内调用
// 参数说明       void
// 返回参数       void
// 使用示例      Wireless_UART_PIT();
//-------------------------------------------------------------------------------------------------------------------

void Wireless_UART_PIT()
{
        data_len = (uint8)wireless_uart_read_buffer(data_buffer, 32);             // 查看是否有消息 默认缓冲区是 WIRELESS_UART_BUFFER_SIZE 总共 64 字节
        if(data_len != 0)                                                       // 收到了消息 读取函数会返回实际读取到的数据个数
        {
            wireless_uart_send_buffer(data_buffer, data_len);                     // 将收到的消息发送回去

            memset(data_buffer, 0, 32);
            func_uint_to_str((char *)data_buffer, data_len);
            data_len=0;
//            wireless_uart_send_string("\r\ndata len:");                                 // 显示实际收到的数据信息
//            wireless_uart_send_buffer(data_buffer, strlen((const char *)data_buffer));    // 显示收到的数据个数
//            wireless_uart_send_string(".\r\n");
        }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     无线转串口模块 发送十进制数字
// 参数说明     number          要发送的整数值
// 返回参数     uint32          剩余发送长度
// 使用示例     wireless_uart_send_decimal(12345);  // 发送十进制数12345
// 备注信息     仅支持十进制格式，支持正负数
//-------------------------------------------------------------------------------------------------------------------
uint32 wireless_uart_send_decimal(int32 number)
{
    char num_str[12]; // 足够存储10位数字+符号+结束符
    char *ptr = &num_str[sizeof(num_str)-1];
    uint32 abs_value;
    uint8 negative = 0;

    *ptr = '\0'; // 字符串结束符

    if (number < 0)
    {
        negative = 1;
        abs_value = (uint32)(-number);
    }
    else
    {
        abs_value = (uint32)number;
    }

    // 转换数字为字符串
    do {
        *--ptr = '0' + (abs_value % 10);
        abs_value /= 10;
    } while (abs_value > 0);

    // 添加负号（如果是负数）
    if (negative)
    {
        *--ptr = '-';
    }

    return wireless_uart_send_string(ptr);
}


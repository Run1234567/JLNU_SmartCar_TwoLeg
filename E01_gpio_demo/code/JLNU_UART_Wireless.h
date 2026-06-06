/*
 * JLNU_UART_Wireless.h
 *
 *  创建于: 2025年9月8日
 *      作者: RUN
 */

#include "zf_common_headfile.h"

#ifndef CODE_JLNU_UART_WIRELESS_H_
#define CODE_JLNU_UART_WIRELESS_H_

extern uint8 data_buffer[32];  /* 无线串口数据接收缓冲区 */
extern uint8 data_len;         /* 接收到的数据长度 */

void UART_Wireless_Init();                                           /* 无线串口初始化 */
void Wireless_UART_PIT();                                            /* 无线串口定时接收处理 */
uint32 wireless_uart_send_decimal(int32 number);                     /* 发送十进制整数 */
uint32 wireless_uart_send_float  (float number, uint8 decimal_places); /* 发送浮点数 */
#endif /* CODE_JLNU_UART_WIRELESS_H_ */

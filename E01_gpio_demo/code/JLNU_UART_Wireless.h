/*
 * JLNU_UART_Wireless.h
 *
 *  Created on: 2025Äê9ÔÂ8ÈÕ
 *      Author: RUN
 */

#include "zf_common_headfile.h"

#ifndef CODE_JLNU_UART_WIRELESS_H_
#define CODE_JLNU_UART_WIRELESS_H_

extern uint8 data_buffer[32];
extern uint8 data_len;

void UART_Wireless_Init();
void Wireless_UART_PIT();
uint32 wireless_uart_send_decimal(int32 number);

#endif /* CODE_JLNU_UART_WIRELESS_H_ */

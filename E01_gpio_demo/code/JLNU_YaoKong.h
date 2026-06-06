/*
 * JLNU_YaoKong.h
 *
 *  遥控器模块头文件
 *  声明了遥控器通道值到速度/角度的转换函数，以及开关状态检测函数
 */

#ifndef JLNU_YAOKONG_H
#define JLNU_YAOKONG_H

int speed_convert_clamped(int x);   /* 遥控器通道值转换为速度，限幅在±400 */
float angle_convert_clamped(int x); /* 遥控器通道值转换为角度，限幅在±1.0度 */
int SWA_Down(void);                 /* 检测SWA三档开关状态是否发生变化 */
int get_SWB_state(void);            /* 获取SWB三档开关的当前档位(0/1/2) */
int CH5_Down(void);                 /* 检测CH5通道状态是否发生变化 */
int CH6_Down(void);                 /* 检测CH6通道状态是否发生变化 */

#endif

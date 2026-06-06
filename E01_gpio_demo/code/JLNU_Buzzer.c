/*
 * JLNU_Buzzer.c
 *
 *  蜂鸣器模块实现
 *  使用P19_4引脚控制蜂鸣器
 */

#include "zf_common_headfile.h"

/* 蜂鸣器初始化函数 */
void Buzzer_Init(void)
{
    gpio_init(P19_4, GPO, 0, GPO_PUSH_PULL); // 初始化蜂鸣器引脚，设为输出默认高电平，推挽输出模式
}

/* 蜂鸣器开启函数 */
void Buzzer_On(void)
{
    int Timer_Now=TimerTime;

    gpio_set_level(P19_4, 1); // 蜂鸣器引脚置低，蜂鸣器响
}

/* 蜂鸣器关闭函数 */
void Buzzer_Off(void)
{
    gpio_set_level(P19_4, 0); // 蜂鸣器引脚置高，蜂鸣器不响
}

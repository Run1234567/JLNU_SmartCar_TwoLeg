#include "zf_common_headfile.h"

void Buzzer_Init(void)
{
    gpio_init(P19_4, GPO, 0, GPO_PUSH_PULL); // 初始化蜂鸣器引脚为输出，默认高电平，推挽输出模式
}
void Buzzer_On(void)
{   int Timer_Now=TimerTime;
    
    gpio_set_level(P19_4, 1); // 将蜂鸣器引脚拉低，蜂鸣器响
}
void Buzzer_Off(void)
{
    gpio_set_level(P19_4, 0); // 将蜂鸣器引脚拉高，蜂鸣器不响
}
#include "zf_common_headfile.h"


#define LED1                    (P19_0)

#define KEY1                    (P20_0)
#define KEY2                    (P20_1)
#define KEY3                    (P20_2)
#define KEY4                    (P20_3)

#define SWITCH1                 (P21_5)
#define SWITCH2                 (P21_6)

uint16 delay_time = 0;
uint8 led_state = 0;

void KEY_INIT(void)
{
      gpio_init(LED1, GPO, GPIO_LOW, GPO_PUSH_PULL);          // 初始化 LED1 输出 默认高电平 推挽输出模式

    gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY1 输入 默认高电平 上拉输入
    gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY2 输入 默认高电平 上拉输入
    gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY3 输入 默认高电平 上拉输入
    gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY4 输入 默认高电平 上拉输入

    gpio_init(SWITCH1, GPI, GPIO_HIGH, GPI_PULL_UP);        // 初始化 SWITCH1 输入 默认高电平 上拉输入
    gpio_init(SWITCH2, GPI, GPIO_HIGH, GPI_PULL_UP);        // 初始化 SWITCH2 输入 默认高电平 上拉输入
}

void KEY_SCAN(void)
{
  
        if( !gpio_get_level(KEY1) || !gpio_get_level(KEY2) || !gpio_get_level(KEY3) || !gpio_get_level(KEY4) )         // 获取 KEYx 电平为低
        {
            delay_time = 500;
        }
        else
        {
            delay_time = 1000;
        }
        if(gpio_get_level(SWITCH1)) delay_time /= 2;
        if(gpio_get_level(SWITCH2)) delay_time /= 2;
        led_state = !led_state;
        system_delay_ms(delay_time);
        gpio_set_level(LED1, led_state);
}



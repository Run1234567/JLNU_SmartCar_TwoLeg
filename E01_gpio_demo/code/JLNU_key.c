#include "zf_common_headfile.h"


#define LED1                    (P19_0)

#define KEY1                    (P20_0)
#define KEY2                    (P20_1)
#define KEY3                    (P20_2)
#define KEY4                    (P20_3)

#define SWITCH1                 (P21_5)
#define SWITCH2                 (P21_6)

uint8 Key_Now = 0;
uint8 Key_Before = 0;
uint8 Key_Down = 0;

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

uint8 KEY_SCAN(void)
{
  if(gpio_get_level(KEY1)==0)
  {
    return 1;
  }
  if(gpio_get_level(KEY2)==0)
  {
    return 2;
  }
  if(gpio_get_level(KEY3)==0)
  {
    return 3;
  }
  if(gpio_get_level(KEY4)==0)
  {
    return 4;
  }
  return 0;
}

void Key_ISR(void)
{
    Key_Before=Key_Now;
    Key_Now=KEY_SCAN();
    Key_Down=Key_Now&(Key_Now^Key_Before);
    if(Key_Down)
    {
        if(Key_Down==1)
        {
            Moter_Flag=0;
        }
        else if(Key_Down==2)
        {
            Moter_Flag=1;
        }
    }
}

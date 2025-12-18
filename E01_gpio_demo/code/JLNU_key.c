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

/**
  * @brief  按键中断服务函数
  * @note   处理所有按键事件，根据当前菜单级别执行相应的菜单导航逻辑
  *         支持上下导航、进入子菜单和返回上级菜单操作
  *         菜单结构：
  *         一级菜单 (level 1): 4个主选项
  *         二级菜单 (level 2): 每个主选项下有不同的子选项
  *         三级菜单 (level 3): 参数调节界面
  * @retval None
  */
void Key_ISR(void)
{
  /* 保存上次按键状态 */
  Key_Before = Key_Now;
  
  /* 读取当前按键状态 */
  Key_Now = KEY_SCAN();
  
  /* 检测按键下降沿（按下事件）：当前按下且上次未按下 */
  Key_Down = Key_Now & (Key_Now ^ Key_Before);
  
  /* 如果有按键按下事件 */
  if(Key_Down)
  {
    /* 清屏操作：每次按键后刷新显示 */
    tft180_clear();
    
    /* 处理"向上"按键事件 */
    if(Key_Down == KEY_UPWARD)
    {
      /* 一级菜单处理：循环递增菜单项（1-4循环） */
      if(menu_level == 1)
      {
        menu_serial_number_One++;
        if(menu_serial_number_One > menu_serial_number_1_Max)
          menu_serial_number_One = 1;  /* 循环到第一个菜单项 */
      }
      /* 二级菜单处理：循环递增菜单项（1-6循环），根据一级菜单选项确定最大项数 */
      else if(menu_level == 2)
      {
        menu_serial_number_Two++;
        /* 如果一级菜单选择的是第一项 */
        if(menu_serial_number_One==1)
        {
          if(menu_serial_number_Two > menu_serial_number_1_1_Max)
            menu_serial_number_Two = 1;  /* 循环到第一个菜单项 */
        }
        /* 如果一级菜单选择的是第二项 */
        else if(menu_serial_number_One==2)
        {
          if(menu_serial_number_Two > menu_serial_number_1_2_Max)
            menu_serial_number_Two = 1;  /* 循环到第一个菜单项 */
        }
      }
      /* 三级菜单处理：参数调节界面 */
      else if(menu_level==3)
      {
        /* 只有在一级菜单第二项下才有三级菜单参数调节功能 */
        if(menu_serial_number_One==2)
        {
          /* 根据二级菜单选项调节不同的PID参数 */
          if(menu_serial_number_Two==1)
          {
            PID_Angular_V.Kp += 0.05;  /* 角速度环P参数增加 */
          }
          else if(menu_serial_number_Two==2)
          {
            PID_Angular.Kp += 10;      /* 角度环P参数增加 */
          }
          else if(menu_serial_number_Two==3)
          {
            PID_Speed.Kp += 0.002;     /* 速度环P参数增加 */
          }
          else if(menu_serial_number_Two==4)
          {
            PID_Speed.Ki += 0.0001;    /* 速度环I参数增加 */
          }
          else if(menu_serial_number_Two==5)
          {
            PID_Speed.Kd += 0.001;     /* 速度环D参数增加 */
          }
        }
      }
    }
    /* 处理"向下"按键事件 */
    else if(Key_Down == KEY_DOWNWARD)
    {
      /* 一级菜单处理：循环递减菜单项（4-1循环） */
      if(menu_level == 1)
      {
        menu_serial_number_One--;
        if(menu_serial_number_One <= 0)
          menu_serial_number_One = 4;  /* 循环到最后一个菜单项 */
      }
      /* 二级菜单处理：循环递减菜单项（6-1循环），根据一级菜单选项确定最大项数 */
      else if(menu_level == 2)
      {
        menu_serial_number_Two--;
        /* 如果一级菜单选择的是第一项 */
        if(menu_serial_number_One==1)
        {
          if(menu_serial_number_Two <= 0)
            menu_serial_number_Two = menu_serial_number_1_1_Max;  /* 循环到最后一个菜单项 */
        }
        /* 如果一级菜单选择的是第二项 */
        else if(menu_serial_number_One==2)
        {
          if(menu_serial_number_Two <= 0)
            menu_serial_number_Two = menu_serial_number_1_2_Max;  /* 循环到最后一个菜单项 */
        }
      }
      /* 三级菜单处理：参数调节界面 */
      else if(menu_level==3)
      {
        /* 只有在一级菜单第二项下才有三级菜单参数调节功能 */
        if(menu_serial_number_One==2)
        {
          /* 根据二级菜单选项调节不同的PID参数 */
          if(menu_serial_number_Two==1)
          {
            PID_Angular_V.Kp -= 0.05;  /* 角速度环P参数减少 */
          }
          else if(menu_serial_number_Two==2)
          {
            PID_Angular.Kp -= 10;      /* 角度环P参数减少 */
          }
          else if(menu_serial_number_Two==3)
          {
            PID_Speed.Kp -= 0.002;     /* 速度环P参数减少 */
          }
          else if(menu_serial_number_Two==4)
          {
            PID_Speed.Ki -= 0.0001;    /* 速度环I参数减少 */
          }
          else if(menu_serial_number_Two==5)
          {
            PID_Speed.Kd -= 0.001;     /* 速度环D参数减少 */
          }
        }
      }
    }
    /* 处理"确认/进入"按键事件 */
    else if(Key_Down == KEY_ENTER)
    {
      /* 从一级菜单进入二级菜单 */
      if(menu_level == 1)
      {
        menu_level = 2;                 /* 更新菜单级别为二级菜单 */
        menu_serial_number_Two = 1;     /* 进入二级菜单默认选择第一项 */
      }
      /* 从二级菜单进入三级菜单 */
      else if(menu_level == 2)
      {
        menu_level = 3;                 /* 更新菜单级别为三级菜单 */
        menu_serial_number_Three = 1;   /* 进入三级菜单默认选择第一项 */
      }
      /* 三级菜单中的确认键通常用于保存设置或确认修改，这里未实现具体功能 */
    }
    /* 处理"退出/返回"按键事件 */
    else if(Key_Down == KEY_EXIT)
    {
      /* 从二级菜单返回一级菜单 */
      if(menu_level == 2)
      {
        menu_level = 1;                 /* 更新菜单级别为一级菜单 */
        menu_serial_number_Two = 0;     /* 重置二级菜单选择 */
      }
      /* 从三级菜单返回二级菜单 */
      else if(menu_level == 3)
      {
        menu_level = 2;                 /* 更新菜单级别为二级菜单 */
        menu_serial_number_Three = 0;   /* 重置三级菜单选择 */
        
        /* 如果一级菜单的第一项被选中（且从三级菜单返回），停止电机 */
        if(menu_serial_number_One == 1)
        {
          Moter_Flag = 0;  /* 关闭电机标志位，停止电机运行 */
        }
      }
    }
  }
}
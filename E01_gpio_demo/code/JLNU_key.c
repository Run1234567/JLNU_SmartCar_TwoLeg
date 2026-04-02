#include "zf_common_headfile.h"

#define LED1 (P19_0)

#define KEY1 (P20_0)
#define KEY2 (P20_1)
#define KEY3 (P20_2)
#define KEY4 (P20_3)

#define SWITCH1 (P21_5)
#define SWITCH2 (P21_6)

uint8 Key_Now = 0;
uint8 Key_Before = 0;
uint8 Key_Down = 0;

void KEY_INIT(void)
{
  gpio_init(LED1, GPO, GPIO_LOW, GPO_PUSH_PULL);   // 初始化 LED1 输出 默认高电平 推挽输出模式
  gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);    // 初始化 KEY1 输入 默认高电平 上拉输入
  gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);    // 初始化 KEY2 输入 默认高电平 上拉输入
  gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);    // 初始化 KEY3 输入 默认高电平 上拉输入
  gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP);    // 初始化 KEY4 输入 默认高电平 上拉输入
  gpio_init(SWITCH1, GPI, GPIO_HIGH, GPI_PULL_UP); // 初始化 SWITCH1 输入 默认高电平 上拉输入
  gpio_init(SWITCH2, GPI, GPIO_HIGH, GPI_PULL_UP); // 初始化 SWITCH2 输入 默认高电平 上拉输入
}

uint8 KEY_SCAN(void)
{
  if (gpio_get_level(KEY1) == 0)
  {
    return 1;
  }
  if (gpio_get_level(KEY2) == 0)
  {
    return 2;
  }
  if (gpio_get_level(KEY3) == 0)
  {
    return 3;
  }
  if (gpio_get_level(KEY4) == 0)
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
  if (Key_Down)
  {
    /* 清屏操作：每次按键后刷新显示 */
    tft180_clear();

    /* 处理"向上"按键事件 */
    if (Key_Down == KEY_UPWARD)
    {
      /* 一级菜单处理：循环递增菜单项（1-4循环） */
      if (menu_level == 1)
      {
        menu_serial_number_One++;
        if (menu_serial_number_One > menu_serial_number_1_Max)
          menu_serial_number_One = 1; /* 循环到第一个菜单项 */
      }
      /* 二级菜单处理：循环递增菜单项（1-6循环），根据一级菜单选项确定最大项数 */
      else if (menu_level == 2)
      {
        menu_serial_number_Two++;
        /* 如果一级菜单选择的是第一项 */
        if (menu_serial_number_One == 1)
        {
          if (menu_serial_number_Two > menu_serial_number_1_1_Max)
            menu_serial_number_Two = 1; /* 循环到第一个菜单项 */
        }
        /* 如果一级菜单选择的是第二项 */
        else if (menu_serial_number_One == 2)
        {
          if (menu_serial_number_Two > menu_serial_number_1_2_Max)
            menu_serial_number_Two = 1; /* 循环到第一个菜单项 */
        }
        else if (menu_serial_number_One == 3)
        {
          if (menu_serial_number_Two > menu_serial_number_1_3_Max)
            menu_serial_number_Two = 1; /* 循环到第一个菜单项 */
        }
        else if (menu_serial_number_One == 4)
        {
          if (menu_serial_number_Two > menu_serial_number_1_4_Max)
            menu_serial_number_Two = 1; /* 循环到第一个菜单项 */
        }
        else if (menu_serial_number_One == 4)
        {
          if (menu_serial_number_Two > menu_serial_number_1_4_Max)
            menu_serial_number_Two = 1; /* 循环到第一个菜单项 */
        }
        else if (menu_serial_number_One == 5)
        {
          if (menu_serial_number_Two > menu_serial_number_1_5_Max)
            menu_serial_number_Two = 1; /* 循环到第一个菜单项 */
        }
      }
      /* 三级菜单处理：参数调节界面 */
      else if (menu_level == 3)
      {
        /* 只有在一级菜单第二项下才有三级菜单参数调节功能 */
        if (menu_serial_number_One == 2)
        {
          /* 根据二级菜单选项调节不同的PID参数 */
          if (menu_serial_number_Two == 1)
          {
            PID_Angular_V.Kp += 0.05; /* 角速度环P参数增加 */
          }
          else if (menu_serial_number_Two == 2)
          {
            PID_Angular.Kp += 10; /* 角度环P参数增加 */
          }
          else if (menu_serial_number_Two == 3)
          {
            PID_Speed.Kp += 0.002; /* 速度环P参数增加 */
          }
          else if (menu_serial_number_Two == 4)
          {
            PID_Speed.Ki += 0.0001; /* 速度环I参数增加 */
          }
          else if (menu_serial_number_Two == 5)
          {
            PID_Speed.Kd += 0.001; /* 速度环D参数增加 */
          }
        }
        else if (menu_serial_number_One == 3)
        {
          if (menu_serial_number_Two == 2 || menu_serial_number_Two == 4 || menu_serial_number_Two == 6)
          {
            menu_serial_number_Three++;
          }
        }
        else if (menu_serial_number_One == 4)
        {
          if (TFT_XY_Flag == 1)
          {
            if (XiuGai_XY == 0)
            {
              if (menu_serial_number_Two == 6)
              {
                IMU_GPS_Used[selected_index].x += 0.03;
              }
              else if (menu_serial_number_Two == 2)
              {
                IMU_Points_used[selected_index].x += 0.03;
              }
            }
            else
            {
              if (menu_serial_number_Two == 6)
              {
                IMU_GPS_Used[selected_index].y += 0.03;
              }
              else if (menu_serial_number_Two == 2)
              {
                IMU_Points_used[selected_index].y += 0.03;
              }
            }
          }
          else
          {
            selected_index++;
            if (menu_serial_number_Two == 2)
            {
              if (selected_index >= current_IMU_point_count_used)
              {
                selected_index = 0;
              }
            }
            if (menu_serial_number_Two == 6)
            {
              if (selected_index >= current_IMU_GPS_Num_Used)
              {
                selected_index = 0;
              }
            }
          }
        }
      }
    }
    /* 处理"向下"按键事件 */
    else if (Key_Down == KEY_DOWNWARD)
    {
      /* 一级菜单处理：循环递减菜单项（4-1循环） */
      if (menu_level == 1)
      {
        menu_serial_number_One--;
        if (menu_serial_number_One <= 0)
          menu_serial_number_One = menu_serial_number_1_Max; /* 循环到最后一个菜单项 */
      }
      /* 二级菜单处理：循环递减菜单项（6-1循环），根据一级菜单选项确定最大项数 */
      else if (menu_level == 2)
      {
        menu_serial_number_Two--;
        /* 如果一级菜单选择的是第一项 */
        if (menu_serial_number_One == 1)
        {
          if (menu_serial_number_Two <= 0)
            menu_serial_number_Two = menu_serial_number_1_1_Max; /* 循环到最后一个菜单项 */
        }
        /* 如果一级菜单选择的是第二项 */
        else if (menu_serial_number_One == 2)
        {
          if (menu_serial_number_Two <= 0)
            menu_serial_number_Two = menu_serial_number_1_2_Max; /* 循环到最后一个菜单项 */
        }
        else if (menu_serial_number_One == 3)
        {
          if (menu_serial_number_Two <= 0)
            menu_serial_number_Two = menu_serial_number_1_3_Max; /* 循环到最后一个菜单项 */
        }
        else if (menu_serial_number_One == 4)
        {
          if (menu_serial_number_Two <= 0)
            menu_serial_number_Two = menu_serial_number_1_4_Max; /* 循环到最后一个菜单项 */
        }
        else if (menu_serial_number_One == 5)
        {
          if (menu_serial_number_Two <= 0)
            menu_serial_number_Two = menu_serial_number_1_5_Max; /* 循环到最后一个菜单项 */  
        }   
      }
      /* 三级菜单处理：参数调节界面 */
      else if (menu_level == 3)
      {
        /* 只有在一级菜单第二项下才有三级菜单参数调节功能 */
        if (menu_serial_number_One == 2)
        {
          /* 根据二级菜单选项调节不同的PID参数 */
          if (menu_serial_number_Two == 1)
          {
            PID_Angular_V.Kp -= 0.05; /* 角速度环P参数减少 */
          }
          else if (menu_serial_number_Two == 2)
          {
            PID_Angular.Kp -= 10; /* 角度环P参数减少 */
          }
          else if (menu_serial_number_Two == 3)
          {
            PID_Speed.Kp -= 0.002; /* 速度环P参数减少 */
          }
          else if (menu_serial_number_Two == 4)
          {
            PID_Speed.Ki -= 0.0001; /* 速度环I参数减少 */
          }
          else if (menu_serial_number_Two == 5)
          {
            PID_Speed.Kd -= 0.001; /* 速度环D参数减少 */
          }
        }
        else if (menu_serial_number_One == 3)
        {
          if (menu_serial_number_Two == 2 || menu_serial_number_Two == 4 || menu_serial_number_Two == 6)
          {
            menu_serial_number_Three--;
            if (menu_serial_number_Three <= 1)
            {
              menu_serial_number_Three = 1;
            }
          }
        }
        else if (menu_serial_number_One == 4)
        {
          if (TFT_XY_Flag == 1)
          {
            if (XiuGai_XY == 0)
            {
              if (menu_serial_number_Two == 6)
              {
                IMU_GPS_Used[selected_index].x -= 0.03;
              }
              else if (menu_serial_number_Two == 2)
              {
                IMU_Points_used[selected_index].x -= 0.03;
              }
            }
            else
            {
              if (menu_serial_number_Two == 6)
              {
                IMU_GPS_Used[selected_index].y -= 0.03;
              }
              else if (menu_serial_number_Two == 2)
              {
                IMU_Points_used[selected_index].y -= 0.03;
              }
            }
          }
          else
          {
          if (selected_index == 0)
          {
            if (menu_serial_number_Two == 6)
            {
              selected_index = current_IMU_GPS_Num_Used - 1;
            }
            if (menu_serial_number_Two == 2)
            {
              selected_index = current_IMU_point_count_used - 1;
            }
          }
          else
            selected_index--;
        }
        }
      }
    }
    /* 处理"确认/进入"按键事件 */
    else if (Key_Down == KEY_ENTER)
    {
      /* 从一级菜单进入二级菜单 */
      if (menu_level == 1)
      {
        if (menu_serial_number_One ==5)
        {
          tft180_set_dir(TFT180_CROSSWISE);
          tft180_init();
        }
        menu_level = 2;             /* 更新菜单级别为二级菜单 */
        menu_serial_number_Two = 1; /* 进入二级菜单默认选择第一项 */

      }
      else if (menu_level == 3)
      {
        if (menu_serial_number_One == 3)
        {
          if (menu_serial_number_Two == 2)
          {
            // 提前算好光标对应的数组下标，代码看起来更清爽
            int cursor_idx = menu_serial_number_Three - 1;

            // 【合并情况 1 和 2】：光标指向当前空位（或空位之后），执行【新增点】逻辑
            if (cursor_idx >= current_point_count)
            {
              if (current_point_count < 100) // 顺手加个防爆护盾，防止数组越界死机
              {
                Route_Points[current_point_count].latitude = gnss.latitude;   /* 记录当前纬度 */
                Route_Points[current_point_count].longitude = gnss.longitude; /* 记录当前经度 */

                current_point_count++; /* 已采集点数量增加 */
                // 统一把光标强制锁定在最新的空位上（等同于你之前的 ++ 和 =count+1）
                menu_serial_number_Three = current_point_count + 1;
              }
            }
            // 【情况 3】：光标指向已有数据的行，执行【删除并递补】逻辑
            else
            {
              // 执行递补循环：从要删除的位置开始，把后面的数据依次往前挪一个位置
              for (int i = cursor_idx; i < current_point_count - 1; i++)
              {
                Route_Points[i] = Route_Points[i + 1];
              }
              // 总数量减 1，丢弃最后一个冗余的数据
              Route_Points[current_point_count - 1].latitude = 0;
              Route_Points[current_point_count - 1].longitude = 0;
              current_point_count--;
            }

            // ? 【极其重要】：只要数组发生了任何改变（不管增还是删），都统一重新计算一次 XY 坐标！
            Convert_GPS_To_XY();
          }
          else if (menu_serial_number_Two == 4)
          {
            // 提前算好光标对应的数组下标，代码看起来更清爽
            int cursor_idx = menu_serial_number_Three - 1;
            // 【合并情况 1 和 2】：光标指向当前空位（或空位之后），执行【新增点】逻辑
            if (cursor_idx >= current_IMU_point_count)
            {
              if (current_IMU_point_count < 100) // 顺手加个防爆护盾，防止数组越界死机
              {
                IMU_Points[current_IMU_point_count].x = Robot_Pos_X; /* 记录当前纬度 */
                IMU_Points[current_IMU_point_count].y = Robot_Pos_Y; /* 记录当前经度 */

                current_IMU_point_count++; /* 已采集点数量增加 */

                // 统一把光标强制锁定在最新的空位上（等同于你之前的 ++ 和 =count+1）
                menu_serial_number_Three = current_IMU_point_count + 1;
              }
            }
            // 【情况 3】：光标指向已有数据的行，执行【删除并递补】逻辑
            else
            {
              // 执行递补循环：从要删除的位置开始，把后面的数据依次往前挪一个位置
              for (int i = cursor_idx; i < current_IMU_point_count - 1; i++)
              {
                IMU_Points[i] = IMU_Points[i + 1];
              }
              // 总数量减 1，丢弃最后一个冗余的数据
              IMU_Points[current_IMU_point_count - 1].x = 0;
              IMU_Points[current_IMU_point_count - 1].y = 0;
              current_IMU_point_count--;
            }
          }
          else if (menu_serial_number_Two == 6)
          {
            // 提前算好光标对应的数组下标，代码看起来更清爽
            int cursor_idx = menu_serial_number_Three - 1;
            // 【合并情况 1 和 2】：光标指向当前空位（或空位之后），执行【新增点】逻辑
            if (cursor_idx >= current_IMU_point_count_KM2)
            {
              if (current_IMU_point_count_KM2 < 100) // 顺手加个防爆护盾，防止数组越界死机
              {
                IMU_Points_KM2[current_IMU_point_count_KM2].x = Robot_Pos_X; /* 记录当前纬度 */
                IMU_Points_KM2[current_IMU_point_count_KM2].y = Robot_Pos_Y; /* 记录当前经度 */

                current_IMU_point_count_KM2++; /* 已采集点数量增加 */

                // 统一把光标强制锁定在最新的空位上（等同于你之前的 ++ 和 =count+1）
                menu_serial_number_Three = current_IMU_point_count_KM2 + 1;
              }
            }
            // 【情况 3】：光标指向已有数据的行，执行【删除并递补】逻辑
            else
            {
              // 执行递补循环：从要删除的位置开始，把后面的数据依次往前挪一个位置
              for (int i = cursor_idx; i < current_IMU_point_count_KM2 - 1; i++)
              {
                IMU_Points_KM2[i] = IMU_Points_KM2[i + 1];
              }
              // 总数量减 1，丢弃最后一个冗余的数据
              IMU_Points_KM2[current_IMU_point_count_KM2 - 1].x = 0;
              IMU_Points_KM2[current_IMU_point_count_KM2 - 1].y = 0;
              current_IMU_point_count_KM2--;
            }
          }
        }
        else if (menu_serial_number_One == 4)
        {
          if(TFT_XY_Flag==1)
          {
            if(XiuGai_XY==0)
            {
              XiuGai_XY=1;
            }
            else
            {
              XiuGai_XY=0;
            }
          }
          if(TFT_XY_Flag==0)
          {
            TFT_XY_Flag=1;
          }

        }
      }
      /* 从二级菜单进入三级菜单 */
      else if (menu_level == 2)
      {
        menu_level = 3;               /* 更新菜单级别为三级菜单 */
        menu_serial_number_Three = 1; /* 进入三级菜单默认选择第一项 */
      }
    }
    /* 处理"退出/返回"按键事件 */
    else if (Key_Down == KEY_EXIT)
    {
      /* 从二级菜单返回一级菜单 */
      if (menu_level == 2)
      {
        if (menu_serial_number_One ==5)
        {
          tft180_set_dir(TFT180_PORTAIT); 
          tft180_init();
        }
        menu_level = 1;             /* 更新菜单级别为一级菜单 */
        menu_serial_number_Two = 0; /* 重置二级菜单选择 */
      }
      /* 从三级菜单返回二级菜单 */
      else if (menu_level == 3)
      {
        if (menu_serial_number_One == 4)
        {
          if(TFT_XY_Flag==1)
          {
            TFT_XY_Flag=0;
            if(menu_serial_number_Two == 6)
            Flash_Save_Array(90, IMU_GPS_Used, sizeof(IMU_Point_t), current_IMU_GPS_Num_Used, 100);
            return;
          }
        }
        if (menu_serial_number_One == 3 && menu_serial_number_Two == 4)
        {
          Save_IMU_To_Flash();
        }
        if (menu_serial_number_One == 3 && menu_serial_number_Two == 6)
          Save_IMU_KM2_To_Flash();
        if (menu_serial_number_One == 3 && (menu_serial_number_Two == 2 || menu_serial_number_Two == 3))
          Save_XY_To_Flash();
        menu_level = 2;               /* 更新菜单级别为二级菜单 */
        menu_serial_number_Three = 0; /* 重置三级菜单选择 */

        /* 如果一级菜单的第一项被选中（且从三级菜单返回），停止电机 */
        if (menu_serial_number_One == 1)
        {
          Moter_Flag = 0; /* 关闭电机标志位，停止电机运行 */
        }
      }
    }
  }
}
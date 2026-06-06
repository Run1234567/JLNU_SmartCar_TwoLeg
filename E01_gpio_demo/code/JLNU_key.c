#include "zf_common_headfile.h"

/* GPIO引脚定义 */
#define LED1     (P19_0)      /* LED1引脚 */
#define KEY1     (P20_0)      /* 按键1引脚 */
#define KEY2     (P20_1)      /* 按键2引脚 */
#define KEY3     (P20_2)      /* 按键3引脚 */
#define KEY4     (P20_3)      /* 按键4引脚 */
#define SWITCH1  (P21_5)      /* 拨动开关1引脚 */
#define SWITCH2  (P21_6)      /* 拨动开关2引脚 */

/* 按键状态全局变量 */
uint8 Key_Now = 0;            /* 当前按键状态 */
uint8 Key_Before = 0;         /* 上一次按键状态 */
uint8 Key_Down = 0;           /* 按键按下边沿检测结果 */

/**
 * @brief  按键及LED的GPIO初始化函数
 * @note   初始化LED1为推挽输出，默认低电平
 *         初始化4个按键和2个拨动开关为上拉输入，默认高电平
 */
void KEY_INIT(void)
{
  gpio_init(LED1, GPO, GPIO_LOW, GPO_PUSH_PULL);     /* 初始化LED1引脚，默认低电平，推挽输出模式 */
  gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);       /* 初始化KEY1引脚，默认高电平，上拉输入 */
  gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);       /* 初始化KEY2引脚，默认高电平，上拉输入 */
  gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);       /* 初始化KEY3引脚，默认高电平，上拉输入 */
  gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP);       /* 初始化KEY4引脚，默认高电平，上拉输入 */
  gpio_init(SWITCH1, GPI, GPIO_HIGH, GPI_PULL_UP);    /* 初始化SWITCH1引脚，默认高电平，上拉输入 */
  gpio_init(SWITCH2, GPI, GPIO_HIGH, GPI_PULL_UP);    /* 初始化SWITCH2引脚，默认高电平，上拉输入 */
}

/**
 * @brief  按键扫描函数
 * @note   轮询4个按键的GPIO电平，低电平表示按下
 * @retval 返回值: 0=无按键按下, 1=KEY1, 2=KEY2, 3=KEY3, 4=KEY4
 */
uint8 KEY_SCAN(void)
{
  if (gpio_get_level(KEY1) == 0)
  {
    return 1;   /* KEY1按下 */
  }
  if (gpio_get_level(KEY2) == 0)
  {
    return 2;   /* KEY2按下 */
  }
  if (gpio_get_level(KEY3) == 0)
  {
    return 3;   /* KEY3按下 */
  }
  if (gpio_get_level(KEY4) == 0)
  {
    return 4;   /* KEY4按下 */
  }
  return 0;     /* 无按键按下 */
}

/**
 * @brief  按键中断服务函数
 * @note   处理所有按键事件，根据当前菜单级别执行对应的菜单导航逻辑。
 *         支持上下滚动、进入子菜单、返回上级菜单的操作。
 *         菜单结构为：
 *         一级菜单 (level 1): 5个主选项
 *         二级菜单 (level 2): 每个主选项下的不同子选项
 *         三级菜单 (level 3): 具体参数调节页面
 * @retval 无
 */
void Key_ISR(void)
{
  /* 保存上一次按键状态 */
  Key_Before = Key_Now;

  /* 获取当前按键状态 */
  Key_Now = KEY_SCAN();

  /* 检测按键下降沿（捕获当前按下且上次未按下的瞬间状态） */
  Key_Down = Key_Now & (Key_Now ^ Key_Before);

  /* 判断是否有按键按下事件 */
  if (Key_Down)
  {
    /* 为了刷新显示，每次按键先清除屏幕 */
    tft180_clear();

    /* 处理 "向上" 按键事件 */
    if (Key_Down == KEY_UPWARD)
    {
      /* 一级菜单下，向上循环滚动菜单项（1-5循环） */
      if (menu_level == 1)
      {
        menu_serial_number_One++;
        if (menu_serial_number_One > menu_serial_number_1_Max)
          menu_serial_number_One = 1; /* 循环回到第一个菜单项 */
      }
      /* 二级菜单下，循环向上滚动子菜单项（最大值取决于一级菜单的选项） */
      else if (menu_level == 2)
      {
        menu_serial_number_Two++;
        /* 若一级菜单选中第一个 */
        if (menu_serial_number_One == 1)
        {
          if (menu_serial_number_Two > menu_serial_number_1_1_Max)
            menu_serial_number_Two = 1; /* 循环回到第一个菜单项 */
        }
        /* 若一级菜单选中第二个 */
        else if (menu_serial_number_One == 2)
        {
          if (menu_serial_number_Two > menu_serial_number_1_2_Max)
            menu_serial_number_Two = 1; /* 循环回到第一个菜单项 */
        }
        else if (menu_serial_number_One == 3)
        {
          if (menu_serial_number_Two > menu_serial_number_1_3_Max)
            menu_serial_number_Two = 1; /* 循环回到第一个菜单项 */
        }
        else if (menu_serial_number_One == 4)
        {
          if (menu_serial_number_Two > menu_serial_number_1_4_Max)
            menu_serial_number_Two = 1; /* 循环回到第一个菜单项 */
        }
        else if (menu_serial_number_One == 4)
        {
          if (menu_serial_number_Two > menu_serial_number_1_4_Max)
            menu_serial_number_Two = 1; /* 循环回到第一个菜单项 */
        }
        else if (menu_serial_number_One == 5)
        {
          if (menu_serial_number_Two > menu_serial_number_1_5_Max)
            menu_serial_number_Two = 1; /* 循环回到第一个菜单项 */
        }
      }
      /* 三级菜单下，进行具体参数值的增加 */
      else if (menu_level == 3)
      {
        /* 只有一级菜单为第二个时才进行参数调节（即PID参数页面） */
        if (menu_serial_number_One == 2)
        {
          /* 根据二级菜单选中的不同PID参数进行参数递增 */
          if (menu_serial_number_Two == 1)
          {
            PID_Angular_V.Kp += 0.05;   /* 角速度环 P 参数递增 */
          }
          else if (menu_serial_number_Two == 2)
          {
            PID_Angular.Kp += 10;        /* 角度环 P 参数递增 */
          }
          else if (menu_serial_number_Two == 3)
          {
            PID_Speed.Kp += 0.002;       /* 速度环 P 参数递增 */
          }
          else if (menu_serial_number_Two == 4)
          {
            PID_Speed.Ki += 0.0001;      /* 速度环 I 参数递增 */
          }
          else if (menu_serial_number_Two == 5)
          {
            PID_Speed.Kd += 0.001;       /* 速度环 D 参数递增 */
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
                IMU_GPS_Used[selected_index].x += 0.03;    /* 修改GPS融合点X坐标 */
              }
              else if (menu_serial_number_Two == 2)
              {
                IMU_Points_used[selected_index].x += 0.03; /* 修改IMU路径点X坐标 */
              }
            }
            else
            {
              if (menu_serial_number_Two == 6)
              {
                IMU_GPS_Used[selected_index].y += 0.03;    /* 修改GPS融合点Y坐标 */
              }
              else if (menu_serial_number_Two == 2)
              {
                IMU_Points_used[selected_index].y += 0.03; /* 修改IMU路径点Y坐标 */
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
                selected_index = 0;  /* 循环回到第一个点 */
              }
            }
            if (menu_serial_number_Two == 6)
            {
              if (selected_index >= current_IMU_GPS_Num_Used)
              {
                selected_index = 0;  /* 循环回到第一个点 */
              }
            }
          }
        }
      }
    }
    /* 处理 "向下" 按键事件 */
    else if (Key_Down == KEY_DOWNWARD)
    {
      /* 一级菜单下，向下循环递减菜单项（5-1循环） */
      if (menu_level == 1)
      {
        menu_serial_number_One--;
        if (menu_serial_number_One <= 0)
          menu_serial_number_One = menu_serial_number_1_Max; /* 循环回到最后一个菜单项 */
      }
      /* 二级菜单下，循环向下递减子菜单项（最大值取决于一级菜单的选项） */
      else if (menu_level == 2)
      {
        menu_serial_number_Two--;
        /* 若一级菜单选中第一个 */
        if (menu_serial_number_One == 1)
        {
          if (menu_serial_number_Two <= 0)
            menu_serial_number_Two = menu_serial_number_1_1_Max; /* 循环回到最后一个菜单项 */
        }
        /* 若一级菜单选中第二个 */
        else if (menu_serial_number_One == 2)
        {
          if (menu_serial_number_Two <= 0)
            menu_serial_number_Two = menu_serial_number_1_2_Max; /* 循环回到最后一个菜单项 */
        }
        else if (menu_serial_number_One == 3)
        {
          if (menu_serial_number_Two <= 0)
            menu_serial_number_Two = menu_serial_number_1_3_Max; /* 循环回到最后一个菜单项 */
        }
        else if (menu_serial_number_One == 4)
        {
          if (menu_serial_number_Two <= 0)
            menu_serial_number_Two = menu_serial_number_1_4_Max; /* 循环回到最后一个菜单项 */
        }
        else if (menu_serial_number_One == 5)
        {
          if (menu_serial_number_Two <= 0)
            menu_serial_number_Two = menu_serial_number_1_5_Max; /* 循环回到最后一个菜单项 */
        }
      }
      /* 三级菜单下，进行具体参数值的减少 */
      else if (menu_level == 3)
      {
        /* 只有一级菜单为第二个时才进行参数调节（即PID参数页面） */
        if (menu_serial_number_One == 2)
        {
          /* 根据二级菜单选中的不同PID参数进行参数递减 */
          if (menu_serial_number_Two == 1)
          {
            PID_Angular_V.Kp -= 0.05;   /* 角速度环 P 参数递减 */
          }
          else if (menu_serial_number_Two == 2)
          {
            PID_Angular.Kp -= 10;        /* 角度环 P 参数递减 */
          }
          else if (menu_serial_number_Two == 3)
          {
            PID_Speed.Kp -= 0.002;       /* 速度环 P 参数递减 */
          }
          else if (menu_serial_number_Two == 4)
          {
            PID_Speed.Ki -= 0.0001;      /* 速度环 I 参数递减 */
          }
          else if (menu_serial_number_Two == 5)
          {
            PID_Speed.Kd -= 0.001;       /* 速度环 D 参数递减 */
          }
        }
        else if (menu_serial_number_One == 3)
        {
          if (menu_serial_number_Two == 2 || menu_serial_number_Two == 4 || menu_serial_number_Two == 6)
          {
            menu_serial_number_Three--;
            if (menu_serial_number_Three <= 1)
            {
              menu_serial_number_Three = 1;  /* 限制最小值为1 */
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
                IMU_GPS_Used[selected_index].x -= 0.03;    /* 修改GPS融合点X坐标 */
              }
              else if (menu_serial_number_Two == 2)
              {
                IMU_Points_used[selected_index].x -= 0.03; /* 修改IMU路径点X坐标 */
              }
              else if (menu_serial_number_Two == 4)
              {
                IMU_Points_used_KM2[selected_index].x -= 0.03; /* 修改KM2路径点X坐标 */
              }
            }
            else
            {
              if (menu_serial_number_Two == 6)
              {
                IMU_GPS_Used[selected_index].y -= 0.03;    /* 修改GPS融合点Y坐标 */
              }
              else if (menu_serial_number_Two == 2)
              {
                IMU_Points_used[selected_index].y -= 0.03; /* 修改IMU路径点Y坐标 */
              }
              else if (menu_serial_number_Two == 4)
              {
                IMU_Points_used_KM2[selected_index].y -= 0.03; /* 修改KM2路径点Y坐标 */
              }
            }
          }
          else
          {
            if (selected_index == 0)
            {
              if (menu_serial_number_Two == 6)
              {
                selected_index = current_IMU_GPS_Num_Used - 1;    /* 循环到最后一个GPS点 */
              }
              if (menu_serial_number_Two == 2)
              {
                selected_index = current_IMU_point_count_used - 1; /* 循环到最后一个IMU点 */
              }
            }
            else
              selected_index--;  /* 选择上一个点 */
          }
        }
      }
    }
    /* 处理 "确认/进入" 按键事件 */
    else if (Key_Down == KEY_ENTER)
    {
      /* 一级菜单进入二级菜单 */
      if (menu_level == 1)
      {
        if (menu_serial_number_One == 5)
        {
          tft180_set_dir(TFT180_CROSSWISE);  /* 切换为横屏显示 */
          tft180_init();
        }
        else if (menu_serial_number_One == 1)
        {
          Moter_Flag=2;                        /* 设置电机标志位 */
        }
        menu_level = 2;                        /* 更新菜单级别为二级菜单 */
        menu_serial_number_Two = 1;            /* 设置二级菜单默认选中第一项 */

      }
      else if (menu_level == 3)
      {
        if (menu_serial_number_One == 3)
        {
          if (menu_serial_number_Two == 2)
          {
            /* 当前光标对应的菜单序号，减1得到数组下标 */
            int cursor_idx = menu_serial_number_Three - 1;

            /* 若光标指向最后一个之后（即空位位置），执行【添加点】逻辑 */
            if (cursor_idx >= current_point_count)
            {
              if (current_point_count < 100)  /* 顺序追加功能，防止数组越界 */
              {
                Route_Points[current_point_count].latitude = gnss.latitude;   /* 记录当前纬度 */
                Route_Points[current_point_count].longitude = gnss.longitude; /* 记录当前经度 */

                current_point_count++;  /* 已采集的点数量递增 */
                /* 统一把光标压到下一个空位上，等同于之前的 ++ 操作 =count+1 */
                menu_serial_number_Three = current_point_count + 1;
              }
            }
            /* 若光标指向已有数据行，执行【删除点】的操作逻辑 */
            else
            {
              /* 执行前移循环：从需要删除的空位开始，将后面的元素逐个向前挪一位 */
              for (int i = cursor_idx; i < current_point_count - 1; i++)
              {
                Route_Points[i] = Route_Points[i + 1];
              }
              /* 总数减 1，清零最后一个多余的数据 */
              Route_Points[current_point_count - 1].latitude = 0;
              Route_Points[current_point_count - 1].longitude = 0;
              current_point_count--;
            }

            /* 只要数组发生任何变化（无论是添加还是删除），统一重新计算一次 XY 坐标！ */
            Convert_GPS_To_XY();
          }
          else if (menu_serial_number_Two == 4)
          {
            /* 当前光标对应的菜单序号，减1得到数组下标 */
            int cursor_idx = menu_serial_number_Three - 1;
            /* 若光标指向最后一个之后（即空位位置），执行【添加点】逻辑 */
            if (cursor_idx >= current_IMU_point_count)
            {
              if (current_IMU_point_count < 100)  /* 顺序追加功能，防止数组越界 */
              {
                IMU_Points[current_IMU_point_count].x = Robot_Pos_X; /* 记录当前 X 坐标 */
                IMU_Points[current_IMU_point_count].y = Robot_Pos_Y; /* 记录当前 Y 坐标 */

                current_IMU_point_count++;  /* 已采集的点数量递增 */

                /* 统一把光标压到下一个空位上，等同于之前的 ++ 操作 =count+1 */
                menu_serial_number_Three = current_IMU_point_count + 1;
              }
            }
            /* 若光标指向已有数据行，执行【删除点】的操作逻辑 */
            else
            {
              /* 执行前移循环：从需要删除的空位开始，将后面的元素逐个向前挪一位 */
              for (int i = cursor_idx; i < current_IMU_point_count - 1; i++)
              {
                IMU_Points[i] = IMU_Points[i + 1];
              }
              /* 总数减 1，清零最后一个多余的数据 */
              IMU_Points[current_IMU_point_count - 1].x = 0;
              IMU_Points[current_IMU_point_count - 1].y = 0;
              current_IMU_point_count--;
            }
          }
          else if (menu_serial_number_Two == 6)
          {
            /* 当前光标对应的菜单序号，减1得到数组下标 */
            int cursor_idx = menu_serial_number_Three - 1;
            /* 若光标指向最后一个之后（即空位位置），执行【添加点】逻辑 */
            if (cursor_idx >= current_IMU_point_count_KM2)
            {
              if (current_IMU_point_count_KM2 < 100)  /* 顺序追加功能，防止数组越界 */
              {
                IMU_Points_KM2[current_IMU_point_count_KM2].x = Robot_Pos_X; /* 记录当前 X 坐标 */
                IMU_Points_KM2[current_IMU_point_count_KM2].y = Robot_Pos_Y; /* 记录当前 Y 坐标 */

                current_IMU_point_count_KM2++;  /* 已采集的点数量递增 */

                /* 统一把光标压到下一个空位上，等同于之前的 ++ 操作 =count+1 */
                menu_serial_number_Three = current_IMU_point_count_KM2 + 1;
              }
            }
            /* 若光标指向已有数据行，执行【删除点】的操作逻辑 */
            else
            {
              /* 执行前移循环：从需要删除的空位开始，将后面的元素逐个向前挪一位 */
              for (int i = cursor_idx; i < current_IMU_point_count_KM2 - 1; i++)
              {
                IMU_Points_KM2[i] = IMU_Points_KM2[i + 1];
              }
              /* 总数减 1，清零最后一个多余的数据 */
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
              XiuGai_XY=1;   /* 切换到修改Y坐标模式 */
            }
            else
            {
              XiuGai_XY=0;   /* 切换到修改X坐标模式 */
            }
          }
          if(TFT_XY_Flag==0)
          {
            TFT_XY_Flag=1;   /* 进入修改模式 */
          }

        }
      }
      /* 从二级菜单进入三级菜单 */
      else if (menu_level == 2)
      {
        menu_level = 3;                   /* 更新菜单级别为三级菜单 */
        menu_serial_number_Three = 1;     /* 设置三级菜单默认选中第一项 */
      }
    }
    /* 处理 "退出/返回" 按键事件 */
    else if (Key_Down == KEY_EXIT)
    {
      /* 从二级菜单返回一级菜单 */
      if (menu_level == 2)
      {
        if (menu_serial_number_One == 5)
        {
          tft180_set_dir(TFT180_PORTAIT);   /* 切换回竖屏显示 */
          tft180_init();
        }
        else if (menu_serial_number_One == 1)
        {
          Moter_Flag=0;                       /* 清除电机标志位 */
        }
        menu_level = 1;                       /* 更新菜单级别为一级菜单 */
        menu_serial_number_Two = 0;           /* 清空二级菜单选项计数 */
      }
      /* 从三级菜单返回二级菜单 */
      else if (menu_level == 3)
      {
        if (menu_serial_number_One == 4)
        {
          if(TFT_XY_Flag==1)
          {
            TFT_XY_Flag=0;   /* 退出修改模式 */
            if(menu_serial_number_Two == 6)
              Flash_Save_Array(90, IMU_GPS_Used, sizeof(IMU_Point_t), current_IMU_GPS_Num_Used, 100); /* 保存GPS融合点到Flash */
            return;
          }
        }
        if (menu_serial_number_One == 3 && menu_serial_number_Two == 4)
        {
          Save_IMU_To_Flash();    /* 保存IMU路径点到Flash */
        }
        if (menu_serial_number_One == 3 && menu_serial_number_Two == 6)
          Save_IMU_KM2_To_Flash();   /* 保存KM2路径点到Flash */
        if (menu_serial_number_One == 3 && (menu_serial_number_Two == 2 || menu_serial_number_Two == 3))
          Save_XY_To_Flash();         /* 保存XY坐标点到Flash */
        menu_level = 2;               /* 更新菜单级别为二级菜单 */
        menu_serial_number_Three = 0; /* 清空三级菜单选项计数 */

        /* 若一级菜单的第一项被选中，且从二级菜单返回，关闭电机 */
        if (menu_serial_number_One == 1)
        {
          Moter_Flag = 0;  /* 关闭电机标志位，停止电机运行 */
        }
      }
    }
  }
}

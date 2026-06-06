/*
 * JLNU_menu.c
 *
 * 菜单模块实现文件
 * 基于TFT180屏幕的三级菜单显示系统
 *
 *  Created on: 2025年10月21日
 *      Author: Run
 */

#include "zf_common_headfile.h"

/* 菜单状态全局变量 */
uint8 menu_serial_number_One = 1;   /* 一级菜单项序号，初始选中第1项 */
uint8 menu_serial_number_Two = 0;   /* 二级菜单项序号，0表示未进入二级菜单 */
uint8 menu_serial_number_Three = 0; /* 三级菜单项序号，0表示未进入三级菜单 */
uint8 menu_level = 1;               /* 当前菜单级别，初始为一级菜单 */

uint8 selected_index = 0;           /* 路径点列表中当前选中的点索引 */
uint8 TFT_XY_Flag = 0;             /* XY坐标修改模式标志: 0=展示模式, 1=修改模式 */
uint8 XiuGai_XY = 0;               /* 修改X或Y坐标的切换标志: 0=修改X, 1=修改Y */

/**
 * @brief  菜单初始化函数
 * @note   初始化TFT180屏幕为竖屏模式，清屏
 */
void menu_init(void)
{
    tft180_set_dir(TFT180_PORTAIT);
    tft180_init();
    system_delay_ms(50);
    tft180_clear();
}

//-------------------------------------------------------------------------------------------------------------------
// 函数名称      屏幕菜单显示主函数
// 函数参数      void
// 返回参数      void
// 使用示例      tft_show();
// 备注信息      根据当前菜单级别和序号显示对应的菜单页面，支持三级菜单嵌套结构
//-------------------------------------------------------------------------------------------------------------------
void tft_show(void)
{
    /* 判断当前菜单级别，根据各级菜单序号是否为0来确定所在菜单级别
     * 注意：判断顺序很重要，优先判断高层数 */
    if (menu_serial_number_Three != 0)
        menu_level = 3; /* 三级菜单序号非0，说明进入了三级菜单 */
    else if (menu_serial_number_Two != 0)
        menu_level = 2; /* 二级菜单序号非0，说明进入了二级菜单 */
    else if (menu_serial_number_One != 0)
        menu_level = 1; /* 一级菜单序号非0，说明在一级菜单 */
    /* 注意：当所有序号都为0时，menu_level不会为0，不显示任何菜单页面 */

    /* 根据计算后的菜单级别执行对应的菜单显示逻辑 */
    switch (menu_level)
    {
    case 1: /* 一级菜单显示逻辑 */
        /* 根据一级菜单序号显示对应的页面 */
        switch (menu_serial_number_One)
        {
        case 1:
            Page_One();     /* 显示页面一：Moter_Mode闪烁 */
            break;
        case 2:
            Page_Two();     /* 显示页面二：PID闪烁 */
            break;
        case 3:
            Page_Three();   /* 显示页面三：GPS闪烁 */
            break;
        case 4:
            Page_Four();    /* 显示页面四：Point_Line闪烁 */
            break;
        case 5:
            Page_Five();    /* 显示页面五：Camera闪烁 */
            break;
        case 6:
            tft180_show_string(0, 0, "Menu 6");
            break;
        case 7:
            tft180_show_string(0, 0, "Menu 7");
            break;
        default:
            /* 默认处理：不显示无效的一级菜单序号
             * 可在此处添加错误处理代码，如显示错误信息或返回主菜单 */
            break;
        }
        break; /* 结束case 1 */

    case 2: /* 二级菜单显示逻辑 */
        /* 先根据一级菜单序号确定所属大类，再根据二级序号确定页面 */
        switch (menu_serial_number_One)
        {
        case 1: /* 一级菜单选中第1项（Moter_Mode）下的二级菜单 */
            switch (menu_serial_number_Two)
            {
            case 1:
                Page_One_1();   /* 显示Motor_Zero */
                break;
            case 2:
                Page_One_2();   /* 显示Motor_IMU_Test */
                break;
            case 3:
                Page_One_3();   /* 显示Motor_Speed=0 */
                break;
            case 4:
                Page_One_4();   /* 显示Motor_KeMu_1 */
                break;
            case 5:
                Page_One_5();   /* 显示Motor_KM_2 */
                break;
            case 6:
                Page_One_6();   /* 显示Motor_YaoKong */
                break;
            case 7:
                Page_One_7();   /* 显示Motor_GPS */
                break;
            case 8:
                Page_One_8();   /* 显示Motor_GPS_KM1 */
                break;
                /* 注意：如需在此处添加更多case以扩展二级菜单项 */
            }
            break; /* 结束switch(menu_serial_number_Two) */

        case 2: /* 一级菜单选中第2项（PID）下的二级菜单 */
            switch (menu_serial_number_Two)
            {
            case 1:
                Page_Two_1();   /* 显示Angle_V_P */
                break;
            case 2:
                Page_Two_2();   /* 显示Angle_P */
                break;
            case 3:
                Page_Two_3();   /* 显示Speed_P */
                break;
            case 4:
                Page_Two_4();   /* 显示Speed_I */
                break;
            case 5:
                Page_Two_5();   /* 显示Speed_D */
                break;
                /* 注意：如需在此处添加更多case以扩展二级菜单项 */
            }
            break; /* 结束switch(menu_serial_number_Two) */
        case 3: /* 一级菜单选中第3项（GPS）下的二级菜单 */
            switch (menu_serial_number_Two)
            {
            case 1:
                Page_Three_1(); /* 显示GPS-Show */
                break;
            case 2:
                Page_Three_2(); /* 显示GPS-Point_Get */
                break;
            case 3:
                Page_Three_3(); /* 显示GPS-PointNum */
                break;
            case 4:
                Page_Three_4(); /* 显示IMU_KM1 */
                break;
            case 5:
                Page_Three_5(); /* 显示IMU_KM1-Show */
                break;
            case 6:
                Page_Three_6(); /* 显示IMU_KM2 */
                break;
            case 7:
                Page_Three_7(); /* 显示IMU_KM2-Show */
                break;
            case 8:
                Page_Three_8(); /* 显示GPS-Show（最后一项） */
                break;
            /* 如有更多页面，请在此处添加 case */
            default:
                break;
            }
            break;
            /* 注意：如需在此处添加更多case以扩展一级菜单下的二级菜单 */
        case 4: /* 一级菜单选中第4项（Point_Line）下的二级菜单 */
            switch (menu_serial_number_Two)
            {
            case 1:
                Page_Four_1();  /* 显示IMU_Points */
                break;
            case 2:
                Page_Four_2();  /* 显示IMU_Points_Used */
                break;
            case 3:
                Page_Four_3();  /* 显示IMU_Points_KM2 */
                break;
            case 4:
                Page_Four_4();  /* 显示IMU_Points_KM2_used */
                break;
            case 5:
                Page_Four_5();  /* 显示IMU_GPS */
                break;
            case 6:
                Page_Four_6();  /* 显示IMU_GPS_used */
                break;
            /* 如果需要增加更多页面（如到8），请在此处添加更多 case 4, case 5... */
            default:
                break;
            }
            break; /* 结束 case 4 */
        case 5: /* 一级菜单选中第5项（Camera）下的二级菜单 */
            switch (menu_serial_number_Two)
            {
            case 1:
                Page_Five_1();  /* 显示摄像头图像页面 */
                break;
            /* 如果需要增加更多页面（如到8），请在此处添加更多 case 4, case 5... */
            default:
            }
                break;
        }
        break; /* 结束case 2 */

    case 3: /* 三级菜单显示逻辑 */
        /* 三级菜单主要根据二级序号确定显示页面：一级->二级->三级 */
        switch (menu_serial_number_One)
        {
        case 1:                             /* 一级菜单选中第1项 */
            switch (menu_serial_number_Two) /* 二级菜单选项 */
            {
            case 1:                               /* 二级菜单第1项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_One_1_1();   /* 显示电机模式1 */
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-1-2");
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */

            case 2:                               /* 二级菜单第2项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_One_2_1();   /* 显示电机模式2 */
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-2-2");
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break;                            /* 结束switch(menu_serial_number_Three) */
            case 3:                               /* 二级菜单第3项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_One_3_1();   /* 显示电机模式3 */
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-3-2");
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break;                            /* 结束switch(menu_serial_number_Three) */
            case 4:                               /* 二级菜单第4项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_One_4_1();   /* 显示电机模式4 */
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-4-2");
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break;                            /* 结束switch(menu_serial_number_Three) */
            case 5:                               /* 二级菜单第5项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_One_5_1();   /* 显示电机模式5 */
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-5-2");
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break;                            /* 结束switch(menu_serial_number_Three) */
            case 6:                               /* 二级菜单第6项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_One_6_1();   /* 显示电机模式6 */
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-6-2");
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break;                            /* 结束switch(menu_serial_number_Three) */
            case 7:                               /* 二级菜单第7项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_One_7_1();   /* 显示电机模式7 */
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-7-2");
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */
            case 8:                               /* 二级菜单第8项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_One_8_1();   /* 显示电机模式8 */
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-8-2");
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */
                /* 注意：如需在此处添加更多case以扩展三级菜单项 */
            }
            break; /* 结束switch(menu_serial_number_Two) */

        case 2:                             /* 一级菜单选中第2项（PID） */
            switch (menu_serial_number_Two) /* 二级菜单选项 */
            {
            case 1:                               /* 二级菜单第1项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Two_1_1();   /* 显示PID参数调节 - 角速度P */
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */

            case 2:                               /* 二级菜单第2项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Two_2_1();   /* 显示PID参数调节 - 角度P */
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */

            case 3:                               /* 二级菜单第3项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Two_3_1();   /* 显示PID参数调节 - 速度P */
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */

            case 4:                               /* 二级菜单第4项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Two_4_1();   /* 显示PID参数调节 - 速度I */
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */

            case 5:                               /* 二级菜单第5项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Two_5_1();   /* 显示PID参数调节 - 速度D */
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */
                /* 注意：如需在此处添加更多case以扩展三级菜单项 */
            }
            break; /* 结束switch(menu_serial_number_Two) */

        case 3:                             /* 一级菜单选中第3项（GPS） */
            switch (menu_serial_number_Two) /* 二级菜单选项 */
            {
            case 1:                               /* 二级菜单第1项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Three_1_1(); /* 显示GPS数据详情 */
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */

            case 2:               /* 二级菜单第2项 (GPS-Point_Get) */
                Page_Three_2_1(); /* 非阻塞功能的特殊处理，无需switch包裹刷新 */
                break;

            case 3:                               /* 二级菜单第3项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Three_3_1(); /* 显示GPS XY坐标点列表 */
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */

            case 4: /* 二级菜单第4项 */
                Page_Three_4_1();   /* 显示IMU路径点列表 */
                break;
                /* 注意：如需在此处添加更多case以扩展三级菜单项 */

            case 5:                               /* 二级菜单第5项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Three_5_1(); /* 显示IMU使用路径点列表 */
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */
            case 6:    /* 二级菜单第6项 */
                Page_Three_6_1();   /* 显示IMU KM2路径点列表 */
                break;              /* 结束switch(menu_serial_number_Three) */
            case 7:                               /* 二级菜单第7项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Three_7_1(); /* 显示IMU KM2使用路径点列表 */
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break;                            /* 结束switch(menu_serial_number_Three) */
            case 8:                               /* 二级菜单第8项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Three_8_1(); /* 显示GPS融合坐标点列表 */
                    break;
                    /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */
                /* 注意：如需在此处添加更多case以扩展三级菜单项 */
            }
            break; /* 结束switch(menu_serial_number_Two) */

        case 4:                               /* 一级菜单选中第4项（Point_Line） */
            switch (menu_serial_number_Two)   /* 二级菜单选项 */
            {
            case 1:                               /* 二级菜单第1项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Four_1_1();  /* 显示GPS轨迹绘制 */
                    break;
                case 2:
                    // Page_Four_1_2();
                    break;
                /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */

            case 2:                               /* 二级菜单第2项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Four_2_1();  /* 显示IMU使用点轨迹绘制 */
                    break;
                case 2:
                    // Page_Four_2_2();
                    break;
                /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */

            case 3:                               /* 二级菜单第3项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Four_3_1();  /* 显示IMU KM2轨迹绘制 */
                    break;
                case 2:
                    // Page_Four_3_2();
                    break;
                /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */
            case 4:                               /* 二级菜单第4项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Four_4_1();  /* 显示IMU KM2使用点轨迹绘制 */
                    break;
                case 2:
                    // Page_Four_3_2();
                    break;
                /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */
            case 5:                               /* 二级菜单第5项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Four_5_1();  /* 显示GPS轨迹绘制 */
                    break;
                case 2:
                    // Page_Four_3_2();
                    break;
                /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */
            case 6:                               /* 二级菜单第6项 */
                switch (menu_serial_number_Three) /* 三级菜单选项 */
                {
                case 1:
                    Page_Four_6_1();  /* 显示GPS融合轨迹绘制 */
                    break;
                case 2:
                    // Page_Four_3_2();
                    break;
                /* 注意：如需在此处添加更多case以扩展三级菜单项 */
                }
                break; /* 结束switch(menu_serial_number_Three) */

            /* 注意：若一级菜单4下还有更多二级菜单项，请在此处添加 case 4, case 5，依此类推 */
            }
            break; /* 结束 case 4 (menu_serial_number_One 判断) */

        /* 注意：如需在此处添加更多case以扩展一级菜单项（如 case 5） */
        }
        break; /* 结束 case 3 (menu_level 判断) */

    /* 注意：如需在此处添加case 0的处理逻辑，例如所有序号都为0时的处理 */
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数名称      页面显示函数集合
// 函数参数      void
// 返回参数      void
// 使用示例      Page_One(); Page_Two(); Page_Three(); Page_Four(); Page_One_1(); Page_One_2();
// 备注信息      这些函数是各个菜单页面的具体实现，当前为空函数，需要根据实际需求编写具体的显示逻辑
//-------------------------------------------------------------------------------------------------------------------

/* 一级菜单第一页：显示菜单列表（Moter_Mode闪烁） */
void Page_One(void)
{
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 0, "Moter_Mode");   /* Moter_Mode项闪烁 */
    else
        tft180_show_string(0, 0, "            ");
    tft180_show_string(0, 20, "PID");
    tft180_show_string(0, 40, "GPS");
    tft180_show_string(0, 60, "Point_Line");
    tft180_show_string(0, 80, "Camera");
}

/* 一级菜单第二页：显示菜单列表（PID闪烁） */
void Page_Two(void)
{
    tft180_show_string(0, 0, "Moter_Mode");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 20, "PID");          /* PID项闪烁 */
    else
        tft180_show_string(0, 20, "            ");
    tft180_show_string(0, 40, "GPS");
    tft180_show_string(0, 60, "Point_Line");
    tft180_show_string(0, 80, "Camera");
}

/* 一级菜单第三页：显示菜单列表（GPS闪烁） */
void Page_Three(void)
{
    tft180_show_string(0, 0, "Moter_Mode");
    tft180_show_string(0, 20, "PID");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 40, "GPS");          /* GPS项闪烁 */
    else
        tft180_show_string(0, 40, "            ");
    tft180_show_string(0, 60, "Point_Line");
    tft180_show_string(0, 80, "Camera");
}

/* 一级菜单第四页：显示菜单列表（Point_Line闪烁） */
void Page_Four(void)
{
    tft180_show_string(0, 0, "Moter_Mode");
    tft180_show_string(0, 20, "PID");
    tft180_show_string(0, 40, "GPS");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 60, "Point_Line");   /* Point_Line项闪烁 */
    else
        tft180_show_string(0, 60, "            ");
    tft180_show_string(0, 80, "Camera");
}

/* 一级菜单第五页：显示菜单列表（Camera闪烁） */
void Page_Five(void)
{
    tft180_show_string(0, 0, "Moter_Mode");
    tft180_show_string(0, 20, "PID");
    tft180_show_string(0, 40, "GPS");
    tft180_show_string(0, 60, "Point_Line");

    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 80, "Camera");       /* Camera项闪烁 */
    else
        tft180_show_string(0, 80, "            ");
}

/* 二级菜单页：显示Moter_Mode子菜单第1项（Motor_Zero闪烁） */
void Page_One_1(void)
{
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 0, "Motor_Zero");
    else
        tft180_show_string(0, 0, "            ");
    tft180_show_string(0, 20, "Motor_IMU_Test");
    tft180_show_string(0, 40, "Motor_Speed=0");
    tft180_show_string(0, 60, "Motor_KeMu_1");
    tft180_show_string(0, 80, "Motor_KM_2");
    tft180_show_string(0, 100, "Motor_YaoKong");
    tft180_show_string(0, 110, "Motor_GPS");
    tft180_show_string(0, 120, "Motor_GPS_KM1");
}

/* 二级菜单页：显示Moter_Mode子菜单第2项（Motor_IMU_Test闪烁） */
void Page_One_2(void)
{
    tft180_show_string(0, 0, "Motor_Zero");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 20, "Motor_IMU_Test");
    else
        tft180_show_string(0, 20, "            ");
    tft180_show_string(0, 40, "Motor_Speed=0");
    tft180_show_string(0, 60, "Motor_KeMu_1");
    tft180_show_string(0, 80, "Motor_KM_2");
    tft180_show_string(0, 100, "Motor_YaoKong");
    tft180_show_string(0, 110, "Motor_GPS");
    tft180_show_string(0, 120, "Motor_GPS_KM1");
}

/* 二级菜单页：显示Moter_Mode子菜单第3项（Motor_Speed=0闪烁） */
void Page_One_3(void)
{
    tft180_show_string(0, 0, "Motor_Zero");
    tft180_show_string(0, 20, "Motor_IMU_Test");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 40, "Motor_Speed=0");
    else
        tft180_show_string(0, 40, "            ");
    tft180_show_string(0, 60, "Motor_KeMu_1");
    tft180_show_string(0, 80, "Motor_KM_2");
    tft180_show_string(0, 100, "Motor_YaoKong");
    tft180_show_string(0, 110, "Motor_GPS");
    tft180_show_string(0, 120, "Motor_GPS_KM1");
}

/* 二级菜单页：显示Moter_Mode子菜单第4项（Motor_KeMu_1闪烁） */
void Page_One_4(void)
{
    tft180_show_string(0, 0, "Motor_Zero");
    tft180_show_string(0, 20, "Motor_IMU_Test");
    tft180_show_string(0, 40, "Motor_Speed=0");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 60, "Motor_KeMu_1");
    else
        tft180_show_string(0, 60, "            ");
    tft180_show_string(0, 80, "Motor_KM_2");
    tft180_show_string(0, 100, "Motor_YaoKong");
    tft180_show_string(0, 110, "Motor_GPS");
    tft180_show_string(0, 120, "Motor_GPS_KM1");
}

/* 二级菜单页：显示Moter_Mode子菜单第5项（Motor_KM_2闪烁） */
void Page_One_5(void)
{
    tft180_show_string(0, 0, "Motor_Zero");
    tft180_show_string(0, 20, "Motor_IMU_Test");
    tft180_show_string(0, 40, "Motor_Speed=0");
    tft180_show_string(0, 60, "Motor_KeMu_1");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 80, "Motor_KM_2");
    else
        tft180_show_string(0, 80, "            ");
    tft180_show_string(0, 100, "Motor_YaoKong");
    tft180_show_string(0, 110, "Motor_GPS");
    tft180_show_string(0, 120, "Motor_GPS_KM1");
}

/* 二级菜单页：显示Moter_Mode子菜单第6项（Motor_YaoKong闪烁） */
void Page_One_6(void)
{
    tft180_show_string(0, 0, "Motor_Zero");
    tft180_show_string(0, 20, "Motor_IMU_Test");
    tft180_show_string(0, 40, "Motor_Speed=0");
    tft180_show_string(0, 60, "Motor_KeMu_1");
    tft180_show_string(0, 80, "Motor_KM_2");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 100, "Motor_YaoKong");
    else
        tft180_show_string(0, 100, "            ");
    tft180_show_string(0, 110, "Motor_GPS");
    tft180_show_string(0, 120, "Motor_GPS_KM1");
}

/* 二级菜单页：显示Moter_Mode子菜单第7项（Motor_GPS闪烁） */
void Page_One_7(void)
{
    tft180_show_string(0, 0, "Motor_Zero");
    tft180_show_string(0, 20, "Motor_IMU_Test");
    tft180_show_string(0, 40, "Motor_Speed=0");
    tft180_show_string(0, 60, "Motor_KeMu_1");
    tft180_show_string(0, 80, "Motor_KM_2");
    tft180_show_string(0, 100, "Motor_YaoKong");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 110, "Motor_GPS");
    else
        tft180_show_string(0, 110, "            ");
        tft180_show_string(0, 120, "Motor_GPS_KM1");
}

/* 二级菜单页：显示Moter_Mode子菜单第8项（Motor_GPS_KM1闪烁） */
void Page_One_8(void)
{
    tft180_show_string(0, 0, "Motor_Zero");
    tft180_show_string(0, 20, "Motor_IMU_Test");
    tft180_show_string(0, 40, "Motor_Speed=0");
    tft180_show_string(0, 60, "Motor_KeMu_1");
    tft180_show_string(0, 80, "Motor_KM_2");
    tft180_show_string(0, 100, "Motor_YaoKong");
    tft180_show_string(0, 110, "Motor_GPS");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 120, "Motor_GPS_KM1");
    else
        tft180_show_string(0, 120, "            ");
}

/* 三级菜单页面：显示Moter_Mode模式1 */
void Page_One_1_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 1");
    Moter_Flag = 0;   /* 设置电机模式0 */
}

/* 三级菜单页面：显示Moter_Mode模式2 */
void Page_One_2_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 2");
    Moter_Flag = 1;   /* 设置电机模式1 */
}

/* 三级菜单页面：显示Moter_Mode模式3 */
void Page_One_3_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 3");
    Moter_Flag = 2;   /* 设置电机模式2 */
}

/* 三级菜单页面：显示Moter_Mode模式4（含姿态、坐标、目标点信息） */
void Page_One_4_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 4");
    tft180_show_string(0, 8, "Angle:");
    tft180_show_float(40, 8, attitude.yaw, 3, 2);       /* 显示偏航角 */
    tft180_show_string(0, 16, "X:");
    tft180_show_float(12, 16, Robot_Pos_X, 2, 2);       /* 显示X坐标 */
    tft180_show_string(50, 16, "Y:");
    tft180_show_float(62, 16, Robot_Pos_Y, 2, 2);       /* 显示Y坐标 */
    tft180_show_string(0, 24, "Point:");
    tft180_show_uint(40, 24, Target_Index, 2);           /* 显示目标点序号 */
    Moter_Flag = 3;   /* 设置电机模式3 */
}

/* 三级菜单页面：显示Moter_Mode模式5（含姿态、坐标、目标点信息） */
void Page_One_5_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 5");
    Moter_Flag = 4;   /* 设置电机模式4 */
    tft180_show_string(0, 8, "Angle:");
    tft180_show_float(40, 8, attitude.yaw, 3, 2);       /* 显示偏航角 */
    tft180_show_string(0, 16, "X:");
    tft180_show_float(12, 16, Robot_Pos_X, 2, 2);       /* 显示X坐标 */
    tft180_show_string(50, 16, "Y:");
    tft180_show_float(62, 16, Robot_Pos_Y, 2, 2);       /* 显示Y坐标 */
    tft180_show_string(0, 24, "Point:");
    tft180_show_uint(40, 24, Target_Index, 2);           /* 显示目标点序号 */
}

/* 三级菜单页面：显示Moter_Mode模式6（遥控模式） */
void Page_One_6_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 6");
    tft180_show_int(0, 8, YaoKong_Test_Mode, 3);        /* 显示遥控测试模式 */
    Moter_Flag = 5;   /* 设置电机模式5 */
}

/* 三级菜单页面：显示Moter_Mode模式7 */
void Page_One_7_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 7");
    Moter_Flag = 6;   /* 设置电机模式6 */
}

/* 三级菜单页面：显示Moter_Mode模式8 */
void Page_One_8_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 8");
    Moter_Flag = 7;   /* 设置电机模式7 */
}

/* 二级菜单页：显示PID子菜单第1项（Angle_V_P闪烁） */
void Page_Two_1(void)
{
    if (TimerTime % 1000 >= 500)
    {
        tft180_show_string(0, 0, "Angle_V_P:");
        tft180_show_float(80, 0, PID_Angular_V.Kp, 1, 2);  /* 显示角速度P值 */
    }
    else
        tft180_show_string(0, 0, "                    ");
    tft180_show_string(0, 20, "Angle_P:");
    tft180_show_uint(65, 20, PID_Angular.Kp, 3);
    tft180_show_float(65, 40, PID_Speed.Kp, 1, 3);
    tft180_show_float(65, 60, PID_Speed.Ki, 1, 4);
    tft180_show_float(65, 80, PID_Speed.Kd, 1, 3);
    tft180_show_string(0, 40, "Speed_P:");
    tft180_show_string(0, 60, "Speed_I:");
    tft180_show_string(0, 80, "Speed_D:");
}

/* 二级菜单页：显示PID子菜单第2项（Angle_P闪烁） */
void Page_Two_2(void)
{
    tft180_show_float(80, 0, PID_Angular_V.Kp, 1, 2);
    tft180_show_float(65, 40, PID_Speed.Kp, 1, 3);
    tft180_show_float(65, 60, PID_Speed.Ki, 1, 4);
    tft180_show_float(65, 80, PID_Speed.Kd, 1, 3);
    tft180_show_string(0, 0, "Angle_V_P:");
    if (TimerTime % 1000 >= 500)
    {
        tft180_show_string(0, 20, "Angle_P:");
        tft180_show_uint(65, 20, PID_Angular.Kp, 3);       /* 显示角度P值 */
    }
    else
        tft180_show_string(0, 20, "                    ");
    tft180_show_string(0, 40, "Speed_P:");
    tft180_show_string(0, 60, "Speed_I:");
    tft180_show_string(0, 80, "Speed_D:");
}

/* 二级菜单页：显示PID子菜单第3项（Speed_P闪烁） */
void Page_Two_3(void)
{
    tft180_show_float(80, 0, PID_Angular_V.Kp, 1, 2);
    tft180_show_uint(65, 20, PID_Angular.Kp, 3);
    tft180_show_float(65, 60, PID_Speed.Ki, 1, 4);
    tft180_show_float(65, 80, PID_Speed.Kd, 1, 3);
    tft180_show_string(0, 0, "Angle_V_P:");
    tft180_show_string(0, 20, "Angle_P:");
    if (TimerTime % 1000 >= 500)
    {
        tft180_show_float(65, 40, PID_Speed.Kp, 1, 3);     /* 显示速度P值 */
        tft180_show_string(0, 40, "Speed_P:");
    }
    else
        tft180_show_string(0, 40, "                    ");
    tft180_show_string(0, 60, "Speed_I:");
    tft180_show_string(0, 80, "Speed_D:");
}

/* 二级菜单页：显示PID子菜单第4项（Speed_I闪烁） */
void Page_Two_4(void)
{
    tft180_show_float(80, 0, PID_Angular_V.Kp, 1, 2);
    tft180_show_uint(65, 20, PID_Angular.Kp, 3);
    tft180_show_float(65, 40, PID_Speed.Kp, 1, 3);
    tft180_show_float(65, 80, PID_Speed.Kd, 1, 3);
    tft180_show_string(0, 0, "Angle_V_P:");
    tft180_show_string(0, 20, "Angle_P:");
    tft180_show_string(0, 40, "Speed_P:");
    if (TimerTime % 1000 >= 500)
    {
        tft180_show_float(65, 60, PID_Speed.Ki, 1, 4);     /* 显示速度I值 */
        tft180_show_string(0, 60, "Speed_I:");
    }
    else
        tft180_show_string(0, 60, "                    ");
    tft180_show_string(0, 80, "Speed_D:");
}

/* 二级菜单页：显示PID子菜单第5项（Speed_D闪烁） */
void Page_Two_5(void)
{
    tft180_show_float(80, 0, PID_Angular_V.Kp, 1, 2);
    tft180_show_uint(65, 20, PID_Angular.Kp, 3);
    tft180_show_float(65, 40, PID_Speed.Kp, 1, 3);
    tft180_show_float(65, 60, PID_Speed.Ki, 1, 4);
    tft180_show_string(0, 0, "Angle_V_P:");
    tft180_show_string(0, 20, "Angle_P:");
    tft180_show_string(0, 40, "Speed_P:");
    tft180_show_string(0, 60, "Speed_I:");
    if (TimerTime % 1000 >= 500)
    {
        tft180_show_float(65, 80, PID_Speed.Kd, 1, 3);     /* 显示速度D值 */
        tft180_show_string(0, 80, "Speed_D:");
    }
    else
        tft180_show_string(0, 80, "                    ");
}

/* 三级菜单页面：PID参数调节 - 角速度P（参数值闪烁） */
void Page_Two_1_1(void)
{
    if (TimerTime % 1000 >= 500)
    {
        tft180_show_string(0, 0, "Angle_V_P:");
        tft180_show_float(80, 0, PID_Angular_V.Kp, 1, 2);  /* 角速度P值闪烁 */
    }
    else
        tft180_show_string(80, 0, "         ");
    tft180_show_string(0, 20, "Angle_P:");
    tft180_show_uint(65, 20, PID_Angular.Kp, 3);
    tft180_show_float(65, 40, PID_Speed.Kp, 1, 3);
    tft180_show_float(65, 60, PID_Speed.Ki, 1, 4);
    tft180_show_float(65, 80, PID_Speed.Kd, 1, 3);
    tft180_show_string(0, 40, "Speed_P:");
    tft180_show_string(0, 60, "Speed_I:");
    tft180_show_string(0, 80, "Speed_D:");
}

/* 三级菜单页面：PID参数调节 - 角度P（参数值闪烁） */
void Page_Two_2_1(void)
{
    tft180_show_float(80, 0, PID_Angular_V.Kp, 1, 2);
    tft180_show_float(65, 40, PID_Speed.Kp, 1, 3);
    tft180_show_float(65, 60, PID_Speed.Ki, 1, 4);
    tft180_show_float(65, 80, PID_Speed.Kd, 1, 3);
    tft180_show_string(0, 0, "Angle_V_P:");
    if (TimerTime % 1000 >= 500)
    {
        tft180_show_string(0, 20, "Angle_P:");
        tft180_show_uint(65, 20, PID_Angular.Kp, 3);       /* 角度P值闪烁 */
    }
    else
        tft180_show_string(65, 20, "           ");
    tft180_show_string(0, 40, "Speed_P:");
    tft180_show_string(0, 60, "Speed_I:");
    tft180_show_string(0, 80, "Speed_D:");
}

/* 三级菜单页面：PID参数调节 - 速度P（参数值闪烁） */
void Page_Two_3_1(void)
{
    tft180_show_float(80, 0, PID_Angular_V.Kp, 1, 2);
    tft180_show_uint(65, 20, PID_Angular.Kp, 3);
    tft180_show_float(65, 60, PID_Speed.Ki, 1, 4);
    tft180_show_float(65, 80, PID_Speed.Kd, 1, 3);
    tft180_show_string(0, 0, "Angle_V_P:");
    tft180_show_string(0, 20, "Angle_P:");
    if (TimerTime % 1000 >= 500)
    {
        tft180_show_float(65, 40, PID_Speed.Kp, 1, 3);     /* 速度P值闪烁 */
        tft180_show_string(0, 40, "Speed_P:");
    }
    else
        tft180_show_string(65, 40, "           ");
    tft180_show_string(0, 60, "Speed_I:");
    tft180_show_string(0, 80, "Speed_D:");
}

/* 三级菜单页面：PID参数调节 - 速度I（参数值闪烁） */
void Page_Two_4_1(void)
{
    tft180_show_float(80, 0, PID_Angular_V.Kp, 1, 2);
    tft180_show_uint(65, 20, PID_Angular.Kp, 3);
    tft180_show_float(65, 40, PID_Speed.Kp, 1, 3);
    tft180_show_float(65, 80, PID_Speed.Kd, 1, 3);
    tft180_show_string(0, 0, "Angle_V_P:");
    tft180_show_string(0, 20, "Angle_P:");
    tft180_show_string(0, 40, "Speed_P:");
    if (TimerTime % 1000 >= 500)
    {
        tft180_show_float(65, 60, PID_Speed.Ki, 1, 4);     /* 速度I值闪烁 */
        tft180_show_string(0, 60, "Speed_I:");
    }
    else
        tft180_show_string(65, 60, "           ");
    tft180_show_string(0, 80, "Speed_D:");
}

/* 三级菜单页面：PID参数调节 - 速度D（参数值闪烁） */
void Page_Two_5_1(void)
{
    tft180_show_float(80, 0, PID_Angular_V.Kp, 1, 2);
    tft180_show_uint(65, 20, PID_Angular.Kp, 3);
    tft180_show_float(65, 40, PID_Speed.Kp, 1, 3);
    tft180_show_float(65, 60, PID_Speed.Ki, 1, 4);
    tft180_show_string(0, 0, "Angle_V_P:");
    tft180_show_string(0, 20, "Angle_P:");
    tft180_show_string(0, 40, "Speed_P:");
    tft180_show_string(0, 60, "Speed_I:");
    if (TimerTime % 1000 >= 500)
    {
        tft180_show_float(65, 80, PID_Speed.Kd, 1, 3);     /* 速度D值闪烁 */
        tft180_show_string(0, 80, "Speed_D:");
    }
    else
        tft180_show_string(65, 80, "           ");
}

/* GPS二级菜单：第1项（GPS-Show闪烁） */
void Page_Three_1(void)
{
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 0, "GPS-Show");
    else
        tft180_show_string(0, 0, "           ");
    tft180_show_string(0, 8, "GPS-Point_Get");
    tft180_show_string(0, 16, "GPS-PointNum");
    tft180_show_string(0, 24, "IMU_KM1");
    tft180_show_string(0, 32, "IMU_KM1-Show");
    tft180_show_string(0, 40, "IMU_KM2");
    tft180_show_string(0, 48, "IMU_KM2-Show");
    tft180_show_string(0, 56, "GPS-Show");
}

/* GPS二级菜单：第2项（GPS-Point_Get闪烁） */
void Page_Three_2(void)
{
    tft180_show_string(0, 0, "GPS-Show");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 8, "GPS-Point_Get");
    else
        tft180_show_string(0, 8, "           ");
    tft180_show_string(0, 16, "GPS-PointNum");
    tft180_show_string(0, 24, "IMU_KM1");
    tft180_show_string(0, 32, "IMU_KM1-Show");
    tft180_show_string(0, 40, "IMU_KM2");
    tft180_show_string(0, 48, "IMU_KM2-Show");
    tft180_show_string(0, 56, "GPS-Show");
}

/* GPS二级菜单：第3项（GPS-PointNum闪烁） */
void Page_Three_3(void)
{
    tft180_show_string(0, 0, "GPS-Show");
    tft180_show_string(0, 8, "GPS-Point_Get");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 16, "GPS-PointNum");
    else
        tft180_show_string(0, 16, "           ");
    tft180_show_string(0, 24, "IMU_KM1");
    tft180_show_string(0, 32, "IMU_KM1-Show");
    tft180_show_string(0, 40, "IMU_KM2");
    tft180_show_string(0, 48, "IMU_KM2-Show");
    tft180_show_string(0, 56, "GPS-Show");
}

/* GPS二级菜单：第4项（IMU_KM1闪烁） */
void Page_Three_4(void)
{
    tft180_show_string(0, 0, "GPS-Show");
    tft180_show_string(0, 8, "GPS-Point_Get");
    tft180_show_string(0, 16, "GPS-PointNum");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 24, "IMU_KM1");
    else
        tft180_show_string(0, 24, "           ");
    tft180_show_string(0, 32, "IMU_KM1-Show");
    tft180_show_string(0, 40, "IMU_KM2");
    tft180_show_string(0, 48, "IMU_KM2-Show");
    tft180_show_string(0, 56, "GPS-Show");
}

/* GPS二级菜单：第5项（IMU_KM1-Show闪烁） */
void Page_Three_5(void)
{
    tft180_show_string(0, 0, "GPS-Show");
    tft180_show_string(0, 8, "GPS-Point_Get");
    tft180_show_string(0, 16, "GPS-PointNum");
    tft180_show_string(0, 24, "IMU_KM1");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 32, "IMU_KM1-Show");
    else
        tft180_show_string(0, 32, "              ");
    tft180_show_string(0, 40, "IMU_KM2");
    tft180_show_string(0, 48, "IMU_KM2-Show");
    tft180_show_string(0, 56, "GPS-Show");
}

/* GPS二级菜单：第6项（IMU_KM2闪烁） */
void Page_Three_6(void)
{
    tft180_show_string(0, 0, "GPS-Show");
    tft180_show_string(0, 8, "GPS-Point_Get");
    tft180_show_string(0, 16, "GPS-PointNum");
    tft180_show_string(0, 24, "IMU_KM1");
    tft180_show_string(0, 32, "IMU_KM1-Show");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 40, "IMU_KM2");
    else
        tft180_show_string(0, 40, "              ");
    tft180_show_string(0, 48, "IMU_KM2-Show");
    tft180_show_string(0, 56, "GPS-Show");
}

/* GPS二级菜单：第7项（IMU_KM2-Show闪烁） */
void Page_Three_7(void)
{
    tft180_show_string(0, 0, "GPS-Show");
    tft180_show_string(0, 8, "GPS-Point_Get");
    tft180_show_string(0, 16, "GPS-PointNum");
    tft180_show_string(0, 24, "IMU_KM1");
    tft180_show_string(0, 32, "IMU_KM1-Show");
    tft180_show_string(0, 40, "IMU_KM2");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 48, "IMU_KM2-Show");
    else
        tft180_show_string(0, 48, "              ");
    tft180_show_string(0, 56, "GPS-Show");
}

/* GPS二级菜单：第8项（GPS-Show最后一项闪烁） */
void Page_Three_8(void)
{
    tft180_show_string(0, 0, "GPS-Show");
    tft180_show_string(0, 8, "GPS-Point_Get");
    tft180_show_string(0, 16, "GPS-PointNum");
    tft180_show_string(0, 24, "IMU_KM1");
    tft180_show_string(0, 32, "IMU_KM1-Show");
    tft180_show_string(0, 40, "IMU_KM2");
    tft180_show_string(0, 48, "IMU_KM2-Show");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 56, "GPS-Show");
    else
        tft180_show_string(0, 56, "           ");
}

/**
 * @brief  三级菜单页面：IMU使用路径点列表显示（支持滚动）
 * @note   显示当前IMU_Points_used数组中的路径点坐标，支持滚动浏览
 */
void Page_Three_5_1(void)
{
    uint8 HuaDong = 0;

    /* 1. 计算滚动偏移量：当序号超过18时启用自动滚动 */
    if (menu_serial_number_Three >= 18)
    {
        HuaDong = menu_serial_number_Three - 18;
    }

    /* 2. 固定显示当前机器人位置 (Robot_Pos_X/Y) */
    tft180_show_string(0, 0, "Now:");
    tft180_show_float(24, 0, Robot_Pos_X, 2, 2);
    tft180_show_float(6 * 11, 0, Robot_Pos_Y, 2, 2);

    /* 3. 循环显示最多18行已使用的 IMU 路径点 (IMU_Points_used) */
    for (uint8 i = 0; i < 18; i++)
    {
        uint8 y_pos = 8 * (i + 1);      /* 屏幕 Y 坐标：8, 16, 24 ... */
        uint8 index = HuaDong + i;      /* 计算当前索引 */

        /* 越界保护，防止空指针或越界读取（数组大小限制为100） */
        if (index >= 100) break;

        /* 统一格式打印序号、X、Y */
        tft180_show_uint(6, y_pos, index, 2);
        tft180_show_float(24, y_pos, IMU_Points_used[index].x, 2, 2);
        tft180_show_float(6 * 11, y_pos, IMU_Points_used[index].y, 2, 2);
    }
}

/**
 * @brief  三级菜单页面：IMU路径点列表显示（可编辑，带光标）
 * @note   显示IMU_Points数组中的路径点坐标，支持滚动和光标指示
 */
void Page_Three_4_1(void)
{
    uint8 HuaDong = 0;

    /* 1. 计算滚动偏移量 */
    if (menu_serial_number_Three >= 18)
    {
        HuaDong = menu_serial_number_Three - 18;
    }

    /* 2. 固定显示当前机器人位置 (Robot_Pos_X/Y) */
    tft180_show_string(0, 0, "Now:");
    tft180_show_float(24, 0, Robot_Pos_X, 2, 2);
    tft180_show_float(6 * 11, 0, Robot_Pos_Y, 2, 2);

    /* 3. 循环显示最多18 行 IMU 路径点 (IMU_Points) */
    for (uint8 i = 0; i < 18; i++)
    {
        uint8 y_pos = 8 * (i + 1);      /* 屏幕 Y 坐标：8, 16, 24 ... */
        uint8 index = HuaDong + i;      /* 计算当前索引 */

        /* 越界保护，防止越界读取（数组大小限制为100） */
        if (index >= 100) break;

        /* 统一格式打印序号、X、Y */
        tft180_show_uint(6, y_pos, index, 2);
        tft180_show_float(24, y_pos, IMU_Points[index].x, 2, 2);
        tft180_show_float(6 * 11, y_pos, IMU_Points[index].y, 2, 2);
    }

    /* 4. 显示光标 "*" */
    /* 小于18满屏时，或等于18时，固定显示在第18行 */
    uint8 cursor_row = (menu_serial_number_Three < 18) ? menu_serial_number_Three : 18;
    tft180_show_string(0, 8 * cursor_row, "*");
}

/**
 * @brief  三级菜单页面：GPS XY坐标点列表显示（带光标）
 * @note   显示Route_Points数组中的XY坐标，支持滚动和光标指示
 */
void Page_Three_3_1(void)
{
    uint8 HuaDong = 0;

    /* 1. 计算滚动偏移量，当序号超过18时启用自动滚动 */
    if (menu_serial_number_Three >= 18)
    {
        HuaDong = menu_serial_number_Three - 18;
    }

    /* 2. 循环显示最多18 行原始路径点 (XY_Points) */
    for (uint8 i = 0; i < 18; i++)
    {
        uint8 y_pos = 8 * (i + 1); /* 屏幕 Y 坐标：8, 16, 24 ... */
        uint8 index = HuaDong + i; /* 计算当前索引 */

        /* 越界保护，防止越界读取（数组大小限制为100） */
        if (index >= 100)
            break;

        /* 统一格式打印序号、X、Y */
        tft180_show_uint(6, y_pos, index, 2);
        tft180_show_float(24, y_pos, IMU_GPS[index].x, 2, 2);
        tft180_show_float(6 * 11, y_pos, IMU_GPS[index].y, 2, 2);
    }
}

/**
 * @brief  三级菜单页面：GPS坐标采集与编辑页面
 * @note   显示当前GPS经纬度和已采集的GPS坐标列表，支持滚动和光标指示
 */
void Page_Three_2_1(void)
{
    uint8 HuaDong = 0;

    /* 1. 计算滚动偏移量 */
    if (menu_serial_number_Three >= 18)
    {
        HuaDong = menu_serial_number_Three - 18;
    }

    /* 2. 固定显示当前 GNSS 模块的实时经纬度 */
    tft180_show_string(0, 0, "Now:");
    tft180_show_float(24, 0, gnss.longitude, 1, 6);
    tft180_show_float(6 * 12, 0, gnss.latitude, 1, 6);

    /* 3. 循环显示最多18 行循环缓存 (Route_Points) 的经纬度 */
    for (uint8 i = 0; i < 18; i++)
    {
        uint8 y_pos = 8 * (i + 1); /* 屏幕 Y 坐标：8, 16, 24 ... */
        uint8 index = HuaDong + i; /* 计算当前索引 */

        /* 越界保护，防止菜单偏移量过大导致读取越界（数组大小也是100） */
        if (index >= 100)
            break;

        /* 统一格式打印序号、经度、纬度 */
        tft180_show_uint(6, y_pos, index, 2);
        tft180_show_float(24, y_pos, Route_Points[index].longitude, 1, 6);
        tft180_show_float(6 * 12, y_pos, Route_Points[index].latitude, 1, 6);
    }

    /* 4. 显示光标 "*" */
    /* 小于18满屏时，或等于18时，固定显示在第18行 */
    uint8 cursor_row = (menu_serial_number_Three < 18) ? menu_serial_number_Three : 18;
    tft180_show_string(0, 8 * cursor_row, "*");
}

/**
 * @brief  三级菜单页面：GPS数据详情显示
 * @note   显示GNSS模块的时间、状态、经纬度、速度、方向、卫星数等信息
 */
void Page_Three_1_1(void)
{
    tft180_show_uint(0, 8 * 0, gnss.time.year, 4);       /* 显示年份 */
    tft180_show_uint(40, 8 * 0, gnss.time.month, 2);     /* 显示月份 */
    tft180_show_uint(80, 8 * 0, gnss.time.day, 2);       /* 显示日期 */
    tft180_show_string(0, 8 * 1, "sta:");
    tft180_show_uint(40, 8 * 1, gnss.state, 5);          /* 显示GNSS状态 */
    tft180_show_string(0, 8 * 2, "E:");
    tft180_show_float(40, 8 * 2, gnss.latitude, 4, 6);   /* 显示纬度 */
    tft180_show_string(0, 8 * 3, "N:");
    tft180_show_float(40, 8 * 3, gnss.longitude, 4, 6);  /* 显示经度 */
    tft180_show_string(0, 8 * 4, "V:");
    tft180_show_float(40, 8 * 4, gnss.speed, 4, 6);      /* 显示速度 */
    tft180_show_string(0, 8 * 5, "dir:");
    tft180_show_float(40, 8 * 5, gnss.direction, 4, 6);  /* 显示方向角 */
    tft180_show_string(0, 8 * 6, "num:");
    tft180_show_uint(40, 8 * 6, gnss.satellite_used, 5); /* 显示使用卫星数 */
}

/**
 * @brief  三级菜单页面：IMU KM2使用路径点列表显示
 * @note   显示IMU_Points_used_KM2数组中的路径点坐标，支持滚动浏览
 */
void Page_Three_7_1(void)
{
    uint8 HuaDong = 0;

    /* 1. 计算滚动偏移量（菜单项序号不超过18时无需滚动） */
    if (menu_serial_number_Three >= 18)
    {
        HuaDong = menu_serial_number_Three - 18;
    }

    /* 2. 固定显示当前机器人位置 (Robot_Pos_X/Y) */
    tft180_show_string(0, 0, "Now:");
    tft180_show_float(24, 0, Robot_Pos_X, 2, 2);
    tft180_show_float(6 * 11, 0, Robot_Pos_Y, 2, 2);

    /* 3. 循环显示最多18 行已搜索路径点 (IMU_Points_used_KM2) */
    for (uint8 i = 0; i < 18; i++)
    {
        uint8 y_pos = 8 * (i + 1); /* 屏幕 Y 坐标：8, 16, 24 ... */
        uint8 index = HuaDong + i; /* 计算当前索引 */

        /* 越界保护 (数组大小限制为100) */
        if (index >= 100)
            break;

        /* 统一格式打印序号、X、Y */
        tft180_show_uint(6, y_pos, index, 2);
        tft180_show_float(24, y_pos, IMU_Points_used_KM2[index].x, 2, 2);
        tft180_show_float(6 * 11, y_pos, IMU_Points_used_KM2[index].y, 2, 2);
    }
}

/**
 * @brief  三级菜单页面：IMU KM2路径点列表显示（可编辑，带光标）
 * @note   显示IMU_Points_KM2数组中的路径点坐标，支持滚动和光标指示
 */
void Page_Three_6_1(void)
{
    uint8 HuaDong = 0;

    /* 1. 计算滚动偏移量 */
    if (menu_serial_number_Three >= 18)
    {
        HuaDong = menu_serial_number_Three - 18;
    }

    /* 2. 固定显示当前机器人位置 (Robot_Pos_X/Y) */
    tft180_show_string(0, 0, "Now:");
    tft180_show_float(24, 0, Robot_Pos_X, 2, 2);
    tft180_show_float(6 * 11, 0, Robot_Pos_Y, 2, 2);

    /* 3. 循环显示最多18 行 IMU 路径点数据 */
    for (uint8 i = 0; i < 18; i++)
    {
        uint8 y_pos = 8 * (i + 1); /* 屏幕 Y 坐标：8, 16, 24 ... 144 */
        uint8 index = HuaDong + i; /* 计算当前索引 */

        /* 数组越界保护，防止错误导航导致数据混乱 (数组大小限制为100) */
        if (index >= 100)
            break;

        /* 统一格式打印序号、X、Y */
        tft180_show_uint(6, y_pos, index, 2);
        tft180_show_float(24, y_pos, IMU_Points_KM2[index].x, 2, 2);
        tft180_show_float(6 * 11, y_pos, IMU_Points_KM2[index].y, 2, 2);
    }

    /* 4. 显示光标 "*" */
    /* 如果序号小于18，光标跟随项目滚动；如果大于等于18，则固定显示在最底行(18) */
    uint8 cursor_row = (menu_serial_number_Three < 18) ? menu_serial_number_Three : 18;
    tft180_show_string(0, 8 * cursor_row, "*");
}

/**
 * @brief  三级菜单页面：GPS融合坐标点列表显示
 * @note   显示IMU_GPS_Used数组中的坐标点，支持滚动浏览
 */
void Page_Three_8_1(void)
{
    uint8 HuaDong = 0;
    if (menu_serial_number_Three >= 18)
    {
        HuaDong = menu_serial_number_Three - 18;
    }

    /* 1. 固定显示当前航迹推算位置 Fused_X / Fused_Y */
    tft180_show_string(0, 0, "Now:");
    tft180_show_float(24, 0, Fused_X, 2, 2);
    tft180_show_float(6 * 11, 0, Fused_Y, 2, 2);

    /* 2. 循环显示最多18 行已搜索路径点 */
    for (uint8 i = 0; i < 18; i++)
    {
        uint8 y_pos = 8 * (i + 1); /* 屏幕 Y 坐标：8, 16, 24 ... */
        uint8 index = HuaDong + i; /* 计算当前索引 */

        /* 数组越界保护，防止菜单偏移量过大导致读取非法内存 (数组大小限制为100) */
        if (index >= 100)
            break;

        /* 统一按照原有布局打印序号、X、Y */
        tft180_show_uint(6, y_pos, index, 2);
        tft180_show_float(24, y_pos, IMU_GPS_Used[index].x, 2, 2);
        tft180_show_float(6 * 11, y_pos, IMU_GPS_Used[index].y, 2, 2);
    }
}


// =========================================================================
// 函数功能：在 TFT180 屏幕上等比例绘制路径点轨迹
// 函数参数：points - 指向 IMU_Point_t 结构体数组的首地址 (路径点)
//          count  - 需要绘制的轨迹点数
// =========================================================================
void Draw_Trajectory_On_TFT180(IMU_Point_t *points, uint16 count)
{
    /* 如果数组为空或点数少于2个无法连线，直接退出 */
    if (points == NULL || count < 2) return;

    float x_min = points[0].x, x_max = points[0].x;
    float y_min = points[0].y, y_max = points[0].y;

    /* 1. 寻找所有点的最大最小边界 */
    for (uint16 i = 1; i < count; i++)
    {
        if (points[i].x < x_min) x_min = points[i].x;
        if (points[i].x > x_max) x_max = points[i].x;
        if (points[i].y < y_min) y_min = points[i].y;
        if (points[i].y > y_max) y_max = points[i].y;
    }

    /* 2. 计算轨迹数据的跨度 */
    float range_x = x_max - x_min;
    float range_y = y_max - y_min;
    if (range_x < 0.001f) range_x = 0.001f; /* 防止除以0导致的异常 */
    if (range_y < 0.001f) range_y = 0.001f;

    /* 3. 动态获取屏幕宽高并计算缩放比例 */
    float draw_w = tft180_width_max - 2 * MARGIN;
    float draw_h = tft180_height_max - 2 * MARGIN;

    float scale_x = draw_w / range_x;
    float scale_y = draw_h / range_y;
    float scale = (scale_x < scale_y) ? scale_x : scale_y; /* 取小值，保证不超出屏幕 */

    /* 计算居中显示的偏移量 */
    float offset_x = MARGIN + (draw_w - range_x * scale) / 2.0f;
    float offset_y = MARGIN + (draw_h - range_y * scale) / 2.0f;

    /* 4. 清屏并准备绘图 */
    tft180_clear();

    /* 5. 遍历所有点，映射坐标并绘图 */
    uint16 last_px = 0, last_py = 0;

    for (uint16 i = 0; i < count; i++)
    {
        /* 映射为屏幕像素格式 */
        uint16 pixel_x = (uint16)((points[i].x - x_min) * scale + offset_x);

        /* 映射并翻转 Y 轴（屏幕 Y 轴朝下，实际 Y 轴朝上） */
        uint16 pixel_y = (uint16)(tft180_height_max - ((points[i].y - y_min) * scale + offset_y));

        /* 从第二个点开始，与上一个点连线 */
        if (i > 0)
        {
            tft180_draw_line(last_px, last_py, pixel_x, pixel_y, RGB565_RED); /* 画红线 */
        }

        /* 起终点标识逻辑（给起点和终点画不同颜色的标记） */
        if (i == 0) {
            /* 起点画蓝色点（可考虑十字/实心方块） */
            tft180_draw_point(pixel_x, pixel_y, RGB565_BLUE);
        } else if (i == count - 1) {
            /* 终点画绿色点 */
            tft180_draw_point(pixel_x, pixel_y, RGB565_GREEN);
        }

        /* 当前被选中的点，画圆圈标识 */
        if (i == selected_index)
        {
            /* 以下为画圆圈标记代码（暂未调通，备后续使用） */
            /* tft180_draw_circle(pixel_x, pixel_y, 4, RGB565_BLUE); */

            /* 设置方向提示的画框功能（由于用户没有画圆函数，暂未实现） */
            /* tft180_draw_rectangle(pixel_x - 3, pixel_y - 3, pixel_x + 3, pixel_y + 3, RGB565_BLUE); */
        }

        /* 记录上一次坐标，供下一次循环使用 */
        last_px = pixel_x;
        last_py = pixel_y;
    }

    if(TFT_XY_Flag==0)tft180_show_string(0, 0, "show  ");
    else tft180_show_string(0, 0, "XiuGai");
    if(TFT_XY_Flag==1)
    {
        if(XiuGai_XY==0)tft180_show_string(50, 0, "XiuGai X");
        else if(XiuGai_XY==1)tft180_show_string(50, 0, "XiuGai Y");

    }
    else
    {
        tft180_show_string(50, 0, "         ");
    }
}

/**
 * @brief  在TFT180屏幕上绘制GPS轨迹
 * @param  points  GPS坐标点数组
 * @param  count   点的数量
 */
void Draw_GPS_Trajectory_On_TFT180(GPS_Point_t *points, uint16 count)
{
    /* 如果数组为空或点数少于2个无法连线，直接退出 */
    if (points == NULL || count < 2) return;

    /* 获取第一个点的纬度，用于计算经度缩放系数 (墨卡托投影的简化版) */
    double lat0_rad = points[0].latitude * M_PI / 180.0;
    double cos_lat0 = cos(lat0_rad);

    /* 记录最大的、最小的X、Y */
    float x_min = 0.0f, x_max = 0.0f;
    float y_min = 0.0f, y_max = 0.0f;

    /* 1. 寻找所有点的最大最小边界 (全部转换为相对坐标系再计算) */
    for (uint16 i = 0; i < count; i++)
    {
        /* 关键：使用 double 计算避免精度丢失（小数点后6位的微小差异） */
        double d_lon = (points[i].longitude - points[0].longitude) * M_PI / 180.0;
        double d_lat = (points[i].latitude - points[0].latitude) * M_PI / 180.0;

        /* 计算相对位置的 X 和 Y 偏移量（单位：米），转为 float 方便屏幕计算 */
        float x_m = (float)(d_lon * EARTH_RADIUS * cos_lat0);
        float y_m = (float)(d_lat * EARTH_RADIUS);

        if (i == 0) {
            x_min = x_max = x_m;
            y_min = y_max = y_m;
        } else {
            if (x_m < x_min) x_min = x_m;
            if (x_m > x_max) x_max = x_m;
            if (y_m < y_min) y_min = y_m;
            if (y_m > y_max) y_max = y_m;
        }
    }

    /* 2. 计算轨迹数据的宽度（单位：米） */
    float range_x = x_max - x_min;
    float range_y = y_max - y_min;

    /* 如果没有那么大的距离，强制设为 1 米防止除以 0 或过度放大 */
    if (range_x < 0.001f) range_x = 0.001f;
    if (range_y < 0.001f) range_y = 0.001f;

    /* 3. 动态获取屏幕宽高并计算缩放比例 */
    float draw_w = tft180_width_max - 2 * MARGIN;
    float draw_h = tft180_height_max - 2 * MARGIN;

    /* 计算 1 米在屏幕上占多少个像素 */
    float scale_x = draw_w / range_x;
    float scale_y = draw_h / range_y;
    float scale = (scale_x < scale_y) ? scale_x : scale_y; /* 取小值，保证不超出屏幕 */

    /* 计算居中显示的偏移量 */
    float offset_x = MARGIN + (draw_w - range_x * scale) / 2.0f;
    float offset_y = MARGIN + (draw_h - range_y * scale) / 2.0f;

    /* 4. 清屏并准备绘图 */
    tft180_clear();

    /* 5. 遍历所有点，映射坐标并绘图 */
    uint16 last_px = 0, last_py = 0;

    for (uint16 i = 0; i < count; i++)
    {
        /* 重新计算当前点的坐标（因为单片机 RAM 较小，不建议一开始就把全部坐标缓存起来，而是分段计算） */
        double d_lon = (points[i].longitude - points[0].longitude) * M_PI / 180.0;
        double d_lat = (points[i].latitude - points[0].latitude) * M_PI / 180.0;
        float x_m = (float)(d_lon * EARTH_RADIUS * cos_lat0);
        float y_m = (float)(d_lat * EARTH_RADIUS);

        /* 映射为屏幕像素格式 */
        uint16 pixel_x = (uint16)((x_m - x_min) * scale + offset_x);

        /* 映射并翻转 Y 轴（屏幕 Y 轴朝下，实际地理坐标朝上） */
        uint16 pixel_y = (uint16)(tft180_height_max - ((y_m - y_min) * scale + offset_y));

        /* 从第二个点开始，与上一个点连线 */
        if (i > 0)
        {
            tft180_draw_line(last_px, last_py, pixel_x, pixel_y, RGB565_RED); /* 画红线 */
        }

        /* 画起终点标识 */
        if (i == 0) {
            tft180_draw_point(pixel_x, pixel_y, RGB565_BLUE);
        } else if (i == count - 1) {
            tft180_draw_point(pixel_x, pixel_y, RGB565_GREEN);
        }

        /* 选中的点画圆圈 */
        if (i == selected_index)
        {
            /* tft180_draw_circle(pixel_x, pixel_y, 4, RGB565_BLUE); */
        }

        last_px = pixel_x;
        last_py = pixel_y;
    }

    /* --- 保留原有的 UI 状态显示逻辑 --- */
    if(TFT_XY_Flag==0) tft180_show_string(0, 0, "show  ");
    else tft180_show_string(0, 0, "XiuGai");

    if(TFT_XY_Flag==1)
    {
        if(XiuGai_XY==0) tft180_show_string(50, 0, "XiuGai X");
        else if(XiuGai_XY==1) tft180_show_string(50, 0, "XiuGai Y");
    }
    else
    {
        tft180_show_string(50, 0, "         ");
    }
}

/* Point_Line二级菜单：第1项（IMU_Points闪烁） */
void Page_Four_1(void)
{
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 0, "IMU_Points");
    else
        tft180_show_string(0, 0, "           ");
    tft180_show_string(0, 8, "IMU_Points_Used");
    tft180_show_string(0, 16, "IMU_Points_KM2");
    tft180_show_string(0, 24, "IMU_Points_KM2_used");
    tft180_show_string(0, 32, "IMU_GPS");
    tft180_show_string(0, 40, "IMU_GPS_used");
}

/* Point_Line二级菜单：第2项（IMU_Points_Used闪烁） */
void Page_Four_2(void)
{
    tft180_show_string(0, 0, "IMU_Points");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 8, "IMU_Points_Used");
    else
        tft180_show_string(0, 8, "                 ");
    tft180_show_string(0, 16, "IMU_Points_KM2");
    tft180_show_string(0, 24, "IMU_Points_KM2_used");
    tft180_show_string(0, 32, "IMU_GPS");
    tft180_show_string(0, 40, "IMU_GPS_used");
}

/* Point_Line二级菜单：第3项（IMU_Points_KM2闪烁） */
void Page_Four_3(void)
{
    tft180_show_string(0, 0, "IMU_Points");
    tft180_show_string(0, 8, "IMU_Points_Used");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 16, "IMU_Points_KM2");
    else
        tft180_show_string(0, 16, "                 ");
    tft180_show_string(0, 24, "IMU_Points_KM2_used");
    tft180_show_string(0, 32, "IMU_GPS");
    tft180_show_string(0, 40, "IMU_GPS_used");
}

/* Point_Line二级菜单：第4项（IMU_Points_KM2_used闪烁） */
void Page_Four_4(void)
{
    tft180_show_string(0, 0, "IMU_Points");
    tft180_show_string(0, 8, "IMU_Points_Used");
    tft180_show_string(0, 16, "IMU_Points_KM2");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 24, "IMU_Points_KM2_used");
    else
        tft180_show_string(0, 24, "                 ");
    tft180_show_string(0, 32, "IMU_GPS");
    tft180_show_string(0, 40, "IMU_GPS_used");
}

/* Point_Line二级菜单：第5项（IMU_GPS闪烁） */
void Page_Four_5(void)
{
    tft180_show_string(0, 0, "IMU_Points");
    tft180_show_string(0, 8, "IMU_Points_Used");
    tft180_show_string(0, 16, "IMU_Points_KM2");
    tft180_show_string(0, 24, "IMU_Points_KM2_used");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 32, "IMU_GPS");
    else
        tft180_show_string(0, 32, "                 ");
    tft180_show_string(0, 40, "IMU_GPS_used");
}

/* Point_Line二级菜单：第6项（IMU_GPS_used闪烁） */
void Page_Four_6(void)
{
    tft180_show_string(0, 0, "IMU_Points");
    tft180_show_string(0, 8, "IMU_Points_Used");
    tft180_show_string(0, 16, "IMU_Points_KM2");
    tft180_show_string(0, 24, "IMU_Points_KM2_used");
    tft180_show_string(0, 32, "IMU_GPS");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 40, "IMU_GPS_used");
    else
        tft180_show_string(0, 40, "                 ");
}

/* 三级菜单：GPS轨迹绘制页面 */
void Page_Four_1_1(void)
{
    Draw_GPS_Trajectory_On_TFT180(Start_GPS_Array, GPS_SAMPLE_TARGET);
}

/* 三级菜单：IMU使用点轨迹绘制页面 */
void Page_Four_2_1(void)
{
    Draw_Trajectory_On_TFT180(IMU_Points_used, current_IMU_point_count_used);
    tft180_show_uint(0, 8, selected_index, 2);
    tft180_show_float(20, 8, IMU_Points_used[selected_index].x, 2, 2);
    tft180_show_float(80, 8, IMU_Points_used[selected_index].y, 2, 2);
}

/* 三级菜单：IMU KM2轨迹绘制页面（待实现） */
void Page_Four_3_1(void)
{
    /* 暂未实现 */
}

/* 三级菜单：IMU KM2使用点轨迹绘制页面 */
void Page_Four_4_1(void)
{
    Draw_Trajectory_On_TFT180(IMU_Points_used_KM2, current_IMU_point_count_used_KM2);
    tft180_show_uint(0, 8, selected_index, 2);
    tft180_show_float(20, 8, IMU_Points_used_KM2[selected_index].x, 2, 2);
    tft180_show_float(80, 8, IMU_Points_used_KM2[selected_index].y, 2, 2);
}

/* 三级菜单：GPS轨迹绘制页面（待实现） */
void Page_Four_5_1(void)
{
    /* 暂未实现 */
}

/* 三级菜单：GPS融合轨迹绘制页面 */
void Page_Four_6_1(void)
{
    Draw_Trajectory_On_TFT180(IMU_GPS_Used, current_IMU_GPS_Num_Used);
    tft180_show_uint(0, 8, selected_index, 2);
    tft180_show_float(20, 8, IMU_GPS_Used[selected_index].x, 2, 2);
    tft180_show_float(80, 8, IMU_GPS_Used[selected_index].y, 2, 2);
}


/* 三级菜单：摄像头图像显示页面 */
void Page_Five_1(void)
{
    tft180_displayimage03x((const uint8 *)mt9v03x_image, 160, 128);
}

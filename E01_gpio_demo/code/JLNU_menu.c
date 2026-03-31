/*
 * JLNU_menu.c
 *
 *  Created on: 2025年10月21日
 *      Author: 凯
 */

#include "zf_common_headfile.h"

uint8 menu_serial_number_One = 1;   // 一级菜单序列号
uint8 menu_serial_number_Two = 0;   // 二级菜单序列号
uint8 menu_serial_number_Three = 0; // 三级菜单序列号
uint8 menu_level = 1;

uint8 selected_index=0; //路径展示的
uint8 TFT_XY_Flag=0;//修改模式和展示模式切换
uint8 XiuGai_XY=0;//修改X还是Y

void menu_init(void)
{
    tft180_set_dir(TFT180_PORTAIT); // 需要先横屏 不然显示不下
    tft180_init();
    system_delay_ms(1000);
    tft180_clear();
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介      屏幕菜单显示函数
// 参数说明      void
// 返回参数      void
// 使用示例      tft_show();
// 备注信息      本函数根据当前菜单层级和序列号显示对应的菜单页面，支持三级菜单嵌套结构
//-------------------------------------------------------------------------------------------------------------------
void tft_show(void)
{
    // 判断当前菜单层级：根据三级菜单序列号是否为0来确定当前所在菜单层级
    // 注意：判断顺序从三级到一级，优先判断最高层级
    if (menu_serial_number_Three != 0)
        menu_level = 3; // 三级菜单序列号非0，说明处于三级菜单
    else if (menu_serial_number_Two != 0)
        menu_level = 2; // 二级菜单序列号非0，说明处于二级菜单
    else if (menu_serial_number_One != 0)
        menu_level = 1; // 一级菜单序列号非0，说明处于一级菜单
    // 注意：当所有序列号都为0时，menu_level保持为0，不显示任何菜单页面

    // 根据计算出的菜单层级，执行对应的菜单显示逻辑
    switch (menu_level)
    {
    case 1: // 一级菜单显示逻辑
        // 根据一级菜单序列号显示对应的页面
        switch (menu_serial_number_One)
        {
        case 1:
            Page_One(); // 显示页面一
            break;
        case 2:
            Page_Two(); // 显示页面二
            break;
        case 3:
            Page_Three(); // 显示页面三
            break;
        case 4:
            Page_Four(); // 显示页面四
            break;
        case 5:
            // 扩展菜单用，暂时不用
            tft180_show_string(0, 0, "Menu 5");
            break;
        case 6:
            tft180_show_string(0, 0, "Menu 6");
            break;
        case 7:
            tft180_show_string(0, 0, "Menu 7");
            break;
        default:
            // 默认情况：处理无效的一级菜单序列号
            // 可以在此处添加错误处理代码，如显示错误信息或返回主菜单
            break;
        }
        break; // 结束case 1

    case 2: // 二级菜单显示逻辑
        // 先根据一级菜单序列号确定主分类，再根据二级序列号确定子页面
        switch (menu_serial_number_One)
        {
        case 1: // 一级菜单第1项下的二级菜单
            switch (menu_serial_number_Two)
            {
            case 1:
                Page_One_1(); // 显示页面一的第一子页面
                break;
            case 2:
                Page_One_2();
                break;
            case 3:
                Page_One_3();
                break;
            case 4:
                Page_One_4(); // 显示页面一的第二子页面
                break;
            case 5:
                Page_One_5(); // 显示页面一的第二子页面
                break;
            case 6:
                Page_One_6(); // 显示页面一的第二子页面
                break;
            case 7:
                Page_One_7(); // 显示页面一的第二子页面
                break;
            case 8:
                Page_One_8(); // 显示页面一的第二子页面
                break;
                // 注意：可以在此添加更多case处理其他二级菜单项
            }
            break; // 结束switch(menu_serial_number_Two)

        case 2: // 一级菜单第2项下的二级菜单
            switch (menu_serial_number_Two)
            {
            case 1:
                Page_Two_1();
                break;
            case 2:
                Page_Two_2();
                break;
            case 3:
                Page_Two_3();
                break;
            case 4:
                Page_Two_4();
                break;
            case 5:
                Page_Two_5();
                break;
                // 注意：可以在此添加更多case处理其他二级菜单项
            }
            break; // 结束switch(menu_serial_number_Two)
        case 3:
            switch (menu_serial_number_Two)
            {
            case 1:
                Page_Three_1(); // 显示页面三的第一子页面
                break;
            case 2:
                Page_Three_2();
                break;
            case 3:
                Page_Three_3();
                break;
            case 4:
                Page_Three_4();
                break;
            case 5:
                Page_Three_5();
                break;
            case 6:
                Page_Three_6();
                break;
            case 7:
                Page_Three_7();
                break;
            case 8:
                Page_Three_8();
                break;
            // 如果有更多子页面，继续添加 case
            default:
                break;
            }
            break;
            // 注意：可以在此添加更多case处理其他一级菜单项下的二级菜单
        case 4: // 一级菜单第4项下的二级菜单
            switch (menu_serial_number_Two)
            {
            case 1:
                Page_Four_1(); // 显示页面四的第一子页面
                break;
            case 2:
                Page_Four_2(); // 显示页面四的第二子页面
                break;
            case 3:
                Page_Four_3(); // 显示页面四的第三子页面
                break;
            case 4:
                Page_Four_4(); // 显示页面四的第三子页面
                break;
            case 5:
                Page_Four_5(); // 显示页面四的第三子页面
                break;
            case 6:
                Page_Four_6(); // 显示页面四的第三子页面
                break;
            // 如果你需要更多子页面（比如到 8），继续在这里添加 case 4, case 5...
            default:
                break;
            }
            break; // 结束 case 4
        }
        break; // 结束case 2

    case 3: // 三级菜单显示逻辑
        // 三级菜单需要三个序列号确定具体页面：一级->二级->三级
        switch (menu_serial_number_One)
        {
        case 1:                             // 一级菜单第1项
            switch (menu_serial_number_Two) // 二级菜单选择
            {
            case 1:                               // 二级菜单第1项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_One_1_1();
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-1-2");
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)

            case 2:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_One_2_1();
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-2-2");
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break;                            // 结束switch(menu_serial_number_Three)
            case 3:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_One_3_1();
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-3-2");
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break;                            // 结束switch(menu_serial_number_Three)
            case 4:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_One_4_1();
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-4-2");
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break;                            // 结束switch(menu_serial_number_Three)
            case 5:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_One_5_1();
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-5-2");
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break;                            // 结束switch(menu_serial_number_Three)
            case 6:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_One_6_1();
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-6-2");
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break;                            // 结束switch(menu_serial_number_Three)
            case 7:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_One_7_1();
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-7-2");
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)
            case 8:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_One_8_1();
                    break;
                case 2:
                    tft180_show_string(0, 0, "Menu 1-8-2");
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)
                // 注意：可以在此添加更多case处理其他二级菜单项
            }
            break; // 结束switch(menu_serial_number_Two)

        case 2:                             // 一级菜单第2项
            switch (menu_serial_number_Two) // 二级菜单选择
            {
            case 1:                               // 二级菜单第1项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Two_1_1();
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)

            case 2:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Two_2_1();
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)

            case 3:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Two_3_1();
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)

            case 4:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Two_4_1();
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)

            case 5:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Two_5_1();
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)
                // 注意：可以在此添加更多case处理其他二级菜单项
            }
            break; // 结束switch(menu_serial_number_Two)

        case 3:                             // 一级菜单第3项
            switch (menu_serial_number_Two) // 二级菜单选择
            {
            case 1:                               // 二级菜单第1项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Three_1_1();
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)

            case 2:               // 二级菜单第2项 (GPS-Point_Get)
                Page_Three_2_1(); // 无论光标跑到几，都让这个函数去处理滑动刷新
                break;

            case 3:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Three_3_1();
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)

            case 4: // 二级菜单第2项

                Page_Three_4_1();
                break;
                // 注意：可以在此添加更多case处理其他三级菜单项

            case 5:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Three_5_1();
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)
            case 6:    // 二级菜单第2项
                Page_Three_6_1();
                break;                            // 结束switch(menu_serial_number_Three)
            case 7:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Three_7_1();
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break;                            // 结束switch(menu_serial_number_Three)
            case 8:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Three_8_1();
                    break;
                    // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)
                // 注意：可以在此添加更多case处理其他二级菜单项
            }
            break; // 结束switch(menu_serial_number_Two)

        case 4:                               // 一级菜单第4项
            switch (menu_serial_number_Two)   // 二级菜单选择
            {
            case 1:                               // 二级菜单第1项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Four_1_1(); // 显示页面四的第1个子页面的第1个三级页面
                    break;
                case 2:
                    // Page_Four_1_2();
                    break;
                // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)

            case 2:                               // 二级菜单第2项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Four_2_1(); // 显示页面四的第2个子页面的第1个三级页面
                    break;
                case 2:
                    // Page_Four_2_2();
                    break;
                // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)

            case 3:                               // 二级菜单第3项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Four_3_1(); // 显示页面四的第3个子页面的第1个三级页面
                    break;
                case 2:
                    // Page_Four_3_2();
                    break;
                // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)
            case 4:                               // 二级菜单第3项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Four_4_1(); // 显示页面四的第3个子页面的第1个三级页面
                    break;
                case 2:
                    // Page_Four_3_2();
                    break;
                // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)
            case 5:                               // 二级菜单第3项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Four_5_1(); // 显示页面四的第5个子页面的第1个三级页面
                    break;
                case 2:
                    // Page_Four_3_2();
                    break;
                // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)
            case 6:                               // 二级菜单第3项
                switch (menu_serial_number_Three) // 三级菜单选择
                {
                case 1:
                    Page_Four_6_1(); // 显示页面四的第6个子页面的第1个三级页面
                    break;
                case 2:
                    // Page_Four_3_2();
                    break;
                // 注意：可以在此添加更多case处理其他三级菜单项
                }
                break; // 结束switch(menu_serial_number_Three)

            // 注意：如果你的一级菜单4下面还有更多二级菜单（比如 case 4, case 5），继续在这里往下加即可
            }
            break; // 结束 case 4 (menu_serial_number_One 判断)

        // 注意：可以在此添加更多case处理其他一级菜单项 (比如 case 5)
        }
        break; // 结束 case 3 (menu_level 判断)

    // 注意：可以在此添加case 0的处理逻辑，当所有序列号都为0时的情况
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介      页面显示函数集合
// 参数说明      void
// 返回参数      void
// 使用示例      Page_One(); Page_Two(); Page_Three(); Page_Four(); Page_One_1(); Page_One_2();
// 备注信息      这些函数是各个菜单页面的具体实现，当前为空函数，需要根据实际需求添加显示逻辑
//-------------------------------------------------------------------------------------------------------------------

// 一级菜单第一项页面显示（闪烁效果）
void Page_One(void)
{
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 0, "Moter_Mode");
    else
        tft180_show_string(0, 0, "            ");
    tft180_show_string(0, 20, "PID");
    tft180_show_string(0, 40, "GPS");
    tft180_show_string(0, 60, "Point_Line");
    tft180_show_string(0, 80, "Menu_5");
}

// 一级菜单第二项页面显示（闪烁效果）
void Page_Two(void)
{
    tft180_show_string(0, 0, "Moter_Mode");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 20, "PID");
    else
        tft180_show_string(0, 20, "            ");
    tft180_show_string(0, 40, "GPS");
    tft180_show_string(0, 60, "Point_Line");
}

// 一级菜单第三项页面显示（闪烁效果）
void Page_Three(void)
{
    tft180_show_string(0, 0, "Moter_Mode");
    tft180_show_string(0, 20, "PID");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 40, "GPS");
    else
        tft180_show_string(0, 40, "            ");
    tft180_show_string(0, 60, "Point_Line");
}

// 一级菜单第四项页面显示（闪烁效果）
void Page_Four(void)
{
    tft180_show_string(0, 0, "Moter_Mode");
    tft180_show_string(0, 20, "PID");
    tft180_show_string(0, 40, "GPS");
    if (TimerTime % 1000 >= 500)
        tft180_show_string(0, 60, "Point_Line");
    else
        tft180_show_string(0, 60, "            ");
}

// 二级菜单第一项页面显示（Moter_Mode子菜单第一项）
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
// 二级菜单第二项页面显示（Moter_Mode子菜单第二项）
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

// 二级菜单第三项页面显示（Moter_Mode子菜单第三项）
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

// 二级菜单第四项页面显示（Moter_Mode子菜单第四项）
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

// 二级菜单第五项页面显示（Moter_Mode子菜单第五项）
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

// 二级菜单第六项页面显示（Moter_Mode子菜单第六项）
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

// 三级菜单页面显示（Moter_Mode模式1）
void Page_One_1_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 1");
    Moter_Flag = 0;
}

// 三级菜单页面显示（Moter_Mode模式2）
void Page_One_2_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 2");
    Moter_Flag = 1;
}

// 三级菜单页面显示（Moter_Mode模式3）
void Page_One_3_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 3");
    Moter_Flag = 2;
}

// 三级菜单页面显示（Moter_Mode模式4）
void Page_One_4_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 4");
    tft180_show_string(0, 8, "Angle:");
    tft180_show_float(40, 8, attitude.yaw, 3, 2);
    tft180_show_string(0, 16, "X:");
    tft180_show_float(12, 16, Robot_Pos_X, 2, 2);
    tft180_show_string(50, 16, "Y:");
    tft180_show_float(62, 16, Robot_Pos_Y, 2, 2);
    tft180_show_string(0, 24, "Point:");
    tft180_show_uint(40, 24, Target_Index, 2);
    Moter_Flag = 3;
}

// 三级菜单页面显示（Moter_Mode模式5）
void Page_One_5_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 5");
    Moter_Flag = 4;
    tft180_show_string(0, 8, "Angle:");
    tft180_show_float(40, 8, attitude.yaw, 3, 2);
    tft180_show_string(0, 16, "X:");
    tft180_show_float(12, 16, Robot_Pos_X, 2, 2);
    tft180_show_string(50, 16, "Y:");
    tft180_show_float(62, 16, Robot_Pos_Y, 2, 2);
    tft180_show_string(0, 24, "Point:");
    tft180_show_uint(40, 24, Target_Index, 2);
}

// 三级菜单页面显示（Moter_Mode模式6）
void Page_One_6_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 6");
    Moter_Flag = 5;
}
void Page_One_7_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 7");
    Moter_Flag = 6;
}
void Page_One_8_1(void)
{
    tft180_show_string(0, 0, "Motor Mod 8");
    Moter_Flag = 7;
}
// 二级菜单第一项页面显示（PID子菜单第一项）
void Page_Two_1(void)
{
    if (TimerTime % 1000 >= 500)
    {
        tft180_show_string(0, 0, "Angle_V_P:");
        tft180_show_float(80, 0, PID_Angular_V.Kp, 1, 2);
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

// 二级菜单第二项页面显示（PID子菜单第二项）
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
        tft180_show_uint(65, 20, PID_Angular.Kp, 3);
    }
    else
        tft180_show_string(0, 20, "                    ");
    tft180_show_string(0, 40, "Speed_P:");
    tft180_show_string(0, 60, "Speed_I:");
    tft180_show_string(0, 80, "Speed_D:");
}

// 二级菜单第三项页面显示（PID子菜单第三项）
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
        tft180_show_float(65, 40, PID_Speed.Kp, 1, 3);
        tft180_show_string(0, 40, "Speed_P:");
    }
    else
        tft180_show_string(0, 40, "                    ");
    tft180_show_string(0, 60, "Speed_I:");
    tft180_show_string(0, 80, "Speed_D:");
}

// 二级菜单第四项页面显示（PID子菜单第四项）
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
        tft180_show_float(65, 60, PID_Speed.Ki, 1, 4);
        tft180_show_string(0, 60, "Speed_I:");
    }
    else
        tft180_show_string(0, 60, "                    ");
    tft180_show_string(0, 80, "Speed_D:");
}

// 二级菜单第五项页面显示（PID子菜单第五项）
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
        tft180_show_float(65, 80, PID_Speed.Kd, 1, 3);
        tft180_show_string(0, 80, "Speed_D:");
    }
    else
        tft180_show_string(0, 80, "                    ");
}

// 三级菜单页面显示（PID调节第一项）
void Page_Two_1_1(void)
{
    if (TimerTime % 1000 >= 500)
    {
        tft180_show_string(0, 0, "Angle_V_P:");
        tft180_show_float(80, 0, PID_Angular_V.Kp, 1, 2);
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

// 三级菜单页面显示（PID调节第二项）
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
        tft180_show_uint(65, 20, PID_Angular.Kp, 3);
    }
    else
        tft180_show_string(65, 20, "           ");
    tft180_show_string(0, 40, "Speed_P:");
    tft180_show_string(0, 60, "Speed_I:");
    tft180_show_string(0, 80, "Speed_D:");
}

// 三级菜单页面显示（PID调节第三项）
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
        tft180_show_float(65, 40, PID_Speed.Kp, 1, 3);
        tft180_show_string(0, 40, "Speed_P:");
    }
    else
        tft180_show_string(65, 40, "           ");
    tft180_show_string(0, 60, "Speed_I:");
    tft180_show_string(0, 80, "Speed_D:");
}

// 三级菜单页面显示（PID调节第四项）
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
        tft180_show_float(65, 60, PID_Speed.Ki, 1, 4);
        tft180_show_string(0, 60, "Speed_I:");
    }
    else
        tft180_show_string(65, 60, "           ");
    tft180_show_string(0, 80, "Speed_D:");
}

// 三级菜单页面显示（PID调节第五项）
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
        tft180_show_float(65, 80, PID_Speed.Kd, 1, 3);
        tft180_show_string(0, 80, "Speed_D:");
    }
    else
        tft180_show_string(65, 80, "           ");
}

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
void Page_Three_5_1(void)
{
    uint8 HuaDong = 0;

    // 1. 补上滑动逻辑（不然列表永远卡在0，没法往下滑）
    if (menu_serial_number_Three >= 18)
    {
        HuaDong = menu_serial_number_Three - 18;
    }

    // 2. 顶部固定显示：当前机器人坐标 (Robot_Pos_X/Y)
    tft180_show_string(0, 0, "Now:");
    tft180_show_float(24, 0, Robot_Pos_X, 2, 2);
    tft180_show_float(6 * 11, 0, Robot_Pos_Y, 2, 2);

    // 3. 循环显示：18 行已使用的 IMU 采点坐标 (IMU_Points_used)
    for (uint8 i = 0; i < 18; i++)
    {
        uint8 y_pos = 8 * (i + 1);      // 屏幕 Y 坐标：8, 16, 24 ...
        uint8 index = HuaDong + i;      // 数组当前索引

        // 越界保护（防止滑到底读取野指针，假设数组最大是100，如果是别的数值记得改）
        if (index >= 100) break; 

        // 统一格式打印：序号、X、Y (已顺手修复第一行 Y 坐标的 2, 6 笔误)
        tft180_show_uint(6, y_pos, index, 2);
        tft180_show_float(24, y_pos, IMU_Points_used[index].x, 2, 2);
        tft180_show_float(6 * 11, y_pos, IMU_Points_used[index].y, 2, 2);
    }
    
    // （注：这段原代码里也没有画光标 '*' 的逻辑，需要的话把前几个函数的结尾光标代码拷过来就行）
}
void Page_Three_4_1(void)
{
    uint8 HuaDong = 0;
    
    // 1. 计算滑动偏移量
    if (menu_serial_number_Three >= 18)
    {
        HuaDong = menu_serial_number_Three - 18;
    }

    // 2. 顶部固定显示：当前机器人坐标 (Robot_Pos_X/Y)
    tft180_show_string(0, 0, "Now:");
    tft180_show_float(24, 0, Robot_Pos_X, 2, 2);
    tft180_show_float(6 * 11, 0, Robot_Pos_Y, 2, 2);

    // 3. 循环显示：18 行 IMU 采点坐标 (IMU_Points)
    for (uint8 i = 0; i < 18; i++)
    {
        uint8 y_pos = 8 * (i + 1);      // 屏幕 Y 坐标：8, 16, 24 ...
        uint8 index = HuaDong + i;      // 数组当前索引

        // 越界保护（防止滑到底读取野指针，假设数组最大是100）
        if (index >= 100) break; 

        // 统一格式打印：序号、X、Y (已修复第一行的 2, 6 笔误)
        tft180_show_uint(6, y_pos, index, 2);
        tft180_show_float(24, y_pos, IMU_Points[index].x, 2, 2);
        tft180_show_float(6 * 11, y_pos, IMU_Points[index].y, 2, 2);
    }

    // 4. 显示光标 "*"
    // 小于18跟着走，大于等于18停在最底行(18)
    uint8 cursor_row = (menu_serial_number_Three < 18) ? menu_serial_number_Three : 18;
    tft180_show_string(0, 8 * cursor_row, "*");
}
void Page_Three_3_1(void)
{
    uint8 HuaDong = 0;

    // 1. 补上滑动逻辑（不然列表永远停在第0个）
    if (menu_serial_number_Three >= 18)
    {
        HuaDong = menu_serial_number_Three - 18;
    }

    // 2. 循环显示：18 行原始采点坐标 (XY_Points)
    for (uint8 i = 0; i < 18; i++)
    {
        uint8 y_pos = 8 * (i + 1); // 屏幕 Y 坐标：8, 16, 24 ...
        uint8 index = HuaDong + i; // 数组当前索引

        // 越界保护（防止滑到底读取野指针，假设数组最大是100）
        if (index >= 100)
            break;

        // 统一格式打印：序号、X、Y (已修复第一行的 2, 6 笔误)
        tft180_show_uint(6, y_pos, index, 2);
        tft180_show_float(24, y_pos, IMU_GPS[index].x, 2, 2);
        tft180_show_float(6 * 11, y_pos, IMU_GPS[index].y, 2, 2);
    }

    // （注：你这段原代码里没有光标 '*' 逻辑，需要的话自己加一下哦）
}

void Page_Three_2_1(void)
{
    uint8 HuaDong = 0;

    // 1. 计算滑动偏移量
    if (menu_serial_number_Three >= 18)
    {
        HuaDong = menu_serial_number_Three - 18;
    }

    // 2. 顶部固定显示：当前 GNSS 模块的实时经纬度
    tft180_show_string(0, 0, "Now:");
    tft180_show_float(24, 0, gnss.longitude, 1, 6);
    tft180_show_float(6 * 12, 0, gnss.latitude, 1, 6);

    // 3. 循环显示：18 行循迹点 (Route_Points) 经纬度
    for (uint8 i = 0; i < 18; i++)
    {
        uint8 y_pos = 8 * (i + 1); // 屏幕 Y 坐标：8, 16, 24 ...
        uint8 index = HuaDong + i; // 数组当前索引

        // 越界保护：防止菜单滑太深读取野指针死机 (假设数组容量也是100，如果是别的数值记得改下)
        if (index >= 100)
            break;

        // 统一格式打印：序号、经度、纬度
        tft180_show_uint(6, y_pos, index, 2);
        tft180_show_float(24, y_pos, Route_Points[index].longitude, 1, 6);
        tft180_show_float(6 * 12, y_pos, Route_Points[index].latitude, 1, 6);
    }

    // 4. 显示光标 "*"
    // 小于18跟着走，大于等于18停在最底行(18)
    uint8 cursor_row = (menu_serial_number_Three < 18) ? menu_serial_number_Three : 18;
    tft180_show_string(0, 8 * cursor_row, "*");
}
void Page_Three_1_1(void)
{
    tft180_show_uint(0, 8 * 0, gnss.time.year, 4);
    tft180_show_uint(40, 8 * 0, gnss.time.month, 2);
    tft180_show_uint(80, 8 * 0, gnss.time.day, 2);
    tft180_show_string(0, 8 * 1, "sta:");
    tft180_show_uint(40, 8 * 1, gnss.state, 5);
    tft180_show_string(0, 8 * 2, "E:");
    tft180_show_float(40, 8 * 2, gnss.latitude, 4, 6);
    tft180_show_string(0, 8 * 3, "N:");
    tft180_show_float(40, 8 * 3, gnss.longitude, 4, 6);
    tft180_show_string(0, 8 * 4, "V:");
    tft180_show_float(40, 8 * 4, gnss.speed, 4, 6);
    tft180_show_string(0, 8 * 5, "dir:");
    tft180_show_float(40, 8 * 5, gnss.direction, 4, 6);
    tft180_show_string(0, 8 * 6, "num:");
    tft180_show_uint(40, 8 * 6, gnss.satellite_used, 5);
    tft180_show_string(0, 8 * 7, "HDOP:");
    tft180_show_float(40, 8 * 7, gnss.hdop, 4, 6);
}
void Page_Three_7_1(void)
{
    uint8 HuaDong = 0;

    // 1. 补上你漏掉的滑动逻辑 (让菜单能真正滑起来)
    if (menu_serial_number_Three >= 18)
    {
        HuaDong = menu_serial_number_Three - 18;
    }

    // 2. 顶部固定显示：当前机器人坐标 (Robot_Pos_X/Y)
    tft180_show_string(0, 0, "Now:");
    tft180_show_float(24, 0, Robot_Pos_X, 2, 2);
    tft180_show_float(6 * 11, 0, Robot_Pos_Y, 2, 2);

    // 3. 循环显示：18 行寻迹点坐标 (IMU_Points_used_KM2)
    for (uint8 i = 0; i < 18; i++)
    {
        uint8 y_pos = 8 * (i + 1); // 屏幕 Y 坐标：8, 16, 24 ...
        uint8 index = HuaDong + i; // 数组当前索引

        // 越界保护 (假设数组最大是100)
        if (index >= 100)
            break;

        // 统一格式打印：序号、X、Y (已修复祖传的 2, 6 笔误)
        tft180_show_uint(6, y_pos, index, 2);
        tft180_show_float(24, y_pos, IMU_Points_used_KM2[index].x, 2, 2);
        tft180_show_float(6 * 11, y_pos, IMU_Points_used_KM2[index].y, 2, 2);
    }

}
void Page_Three_6_1(void)
{
    uint8 HuaDong = 0;

    // 1. 计算滑动偏移量
    if (menu_serial_number_Three >= 18)
    {
        HuaDong = menu_serial_number_Three - 18;
    }

    // 2. 顶部固定显示：当前机器人坐标 (Robot_Pos_X/Y)
    tft180_show_string(0, 0, "Now:");
    tft180_show_float(24, 0, Robot_Pos_X, 2, 2);
    tft180_show_float(6 * 11, 0, Robot_Pos_Y, 2, 2);

    // 3. 循环显示：18 行 IMU 坐标数据
    for (uint8 i = 0; i < 18; i++)
    {
        uint8 y_pos = 8 * (i + 1); // 屏幕 Y 坐标：8, 16, 24 ... 144
        uint8 index = HuaDong + i; // 数组当前索引

        // 增加越界保护：防止滑动过深导致死机 (假设数组容量是100)
        if (index >= 100)
            break;

        // 统一格式打印：序号、X、Y (已顺手修复原代码混入的 2, 6)
        tft180_show_uint(6, y_pos, index, 2);
        tft180_show_float(24, y_pos, IMU_Points_KM2[index].x, 2, 2);
        tft180_show_float(6 * 11, y_pos, IMU_Points_KM2[index].y, 2, 2);
    }

    // 4. 显示光标 "*"
    // 如果序号小于18，光标跟着序号走；如果大于等于18，光标固定在最底下那行(18)
    uint8 cursor_row = (menu_serial_number_Three < 18) ? menu_serial_number_Three : 18;
    tft180_show_string(0, 8 * cursor_row, "*");
}
void Page_Three_8_1(void)
{
    uint8 HuaDong = 0;
    if (menu_serial_number_Three >= 18)
    {
        HuaDong = menu_serial_number_Three - 18;
    }

    // 1. 顶部固定显示：当前融合坐标 Fused_X / Fused_Y
    tft180_show_string(0, 0, "Now:");
    tft180_show_float(24, 0, Fused_X, 2, 2);
    tft180_show_float(6 * 11, 0, Fused_Y, 2, 2);

    // 2. 循环显示：18 行寻迹点坐标
    for (uint8 i = 0; i < 18; i++)
    {
        uint8 y_pos = 8 * (i + 1); // 屏幕 Y 坐标：8, 16, 24 ...
        uint8 index = HuaDong + i; // 数组当前索引

        // 增加越界保护：防止菜单滑太深导致读取了数组外面的乱码甚至死机 (假设你的数组最大是100)
        if (index >= 100)
            break;

        // 统一按你原来的布局打印：序号、X、Y
        tft180_show_uint(6, y_pos, index, 2);
        tft180_show_float(24, y_pos, IMU_GPS_Used[index].x, 2, 2);
        tft180_show_float(6 * 11, y_pos, IMU_GPS_Used[index].y, 2, 2); 
    }
}


// =========================================================================
// 函数功能：在 TFT180 屏幕上等比例绘制任意轨迹
// 参数说明：points - 指向 IMU_Point_t 结构体数组的首地址 (数组名)
//          count  - 需要绘制的轨迹点数量
// =========================================================================
void Draw_Trajectory_On_TFT180(IMU_Point_t *points, uint16 count)
{
    // 如果传入的是空指针，或者点数少于2个连不成线，直接退出
    if (points == NULL || count < 2) return; 

    float x_min = points[0].x, x_max = points[0].x;
    float y_min = points[0].y, y_max = points[0].y;

    // 1. 寻找物理世界的坐标边界
    for (uint16 i = 1; i < count; i++)
    {
        if (points[i].x < x_min) x_min = points[i].x;
        if (points[i].x > x_max) x_max = points[i].x;
        if (points[i].y < y_min) y_min = points[i].y;
        if (points[i].y > y_max) y_max = points[i].y;
    }

    // 2. 计算物理轨迹的跨度
    float range_x = x_max - x_min;
    float range_y = y_max - y_min;
    if (range_x < 0.001f) range_x = 0.001f; // 防止除以0硬件死机
    if (range_y < 0.001f) range_y = 0.001f;

    // 3. 动态获取屏幕宽高计算缩放比例 
    float draw_w = tft180_width_max - 2 * MARGIN;
    float draw_h = tft180_height_max - 2 * MARGIN;
    
    float scale_x = draw_w / range_x;
    float scale_y = draw_h / range_y;
    float scale = (scale_x < scale_y) ? scale_x : scale_y; // 取小值，保证绝对不变形

    // 居中显示的偏移量
    float offset_x = MARGIN + (draw_w - range_x * scale) / 2.0f;
    float offset_y = MARGIN + (draw_h - range_y * scale) / 2.0f;

    // 4. 清空屏幕原有内容
    tft180_clear();

    // 5. 遍历所有点，映射坐标并画线
    uint16 last_px = 0, last_py = 0; 

    for (uint16 i = 0; i < count; i++)
    {
        // 映射为屏幕像素格式
        uint16 pixel_x = (uint16)((points[i].x - x_min) * scale + offset_x);
        
        // 映射并反转 Y 轴（屏幕 Y 轴朝下，真实 Y 轴朝上）
        uint16 pixel_y = (uint16)(tft180_height_max - ((points[i].y - y_min) * scale + offset_y));

        // 从第二个点开始，和上一个点连线
        if (i > 0)
        {
            tft180_draw_line(last_px, last_py, pixel_x, pixel_y, RGB565_RED); // 画红线
        }

        // 顺手做个高级功能：给起点和终点打上不同颜色的标记！
        if (i == 0) {
            // 起点画个蓝色点（或者十字/实心方块）
            tft180_draw_point(pixel_x, pixel_y, RGB565_BLUE); 
        } else if (i == count - 1) {
            // 终点画个绿色点
            tft180_draw_point(pixel_x, pixel_y, RGB565_GREEN);
        }

        // ==========================================
        // ? 新增逻辑：如果是被选中的点，画个蓝圈高亮
        // ==========================================
        if (i == selected_index) 
        {
            // 假设你的屏幕驱动库里有画空心圆的函数，半径给 4 个像素
            tft180_draw_circle(pixel_x, pixel_y, 4, RGB565_BLUE); 
            
            // 备用方案：如果你的库没有画圆函数，可以用画空心矩形代替：
            // tft180_draw_rectangle(pixel_x - 3, pixel_y - 3, pixel_x + 3, pixel_y + 3, RGB565_BLUE);
        }

        // 记录下来，留给下一次循环连线
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
void Page_Four_1()
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
void Page_Four_2()
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
void Page_Four_3()
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
void Page_Four_4()
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
void Page_Four_5()
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
void Page_Four_6()
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
void Page_Four_1_1()
{
    
}
void Page_Four_2_1()
{
    Draw_Trajectory_On_TFT180(IMU_Points_used,current_IMU_point_count_used);
                tft180_show_uint(0, 8, selected_index, 2);
        tft180_show_float(20, 8, IMU_Points_used[selected_index].x, 2, 2);
        tft180_show_float(80, 8, IMU_Points_used[selected_index].y, 2, 2);
}
void Page_Four_3_1()
{

}
void Page_Four_4_1()
{

}
void Page_Four_5_1()
{

}
void Page_Four_6_1()
{
    Draw_Trajectory_On_TFT180(IMU_GPS_Used,current_IMU_GPS_Num_Used);
                tft180_show_uint(0, 8, selected_index, 2);
        tft180_show_float(20, 8, IMU_GPS_Used[selected_index].x, 2, 2);
        tft180_show_float(80, 8, IMU_GPS_Used[selected_index].y, 2, 2);
}


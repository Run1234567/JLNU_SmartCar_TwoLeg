/*
 * JLNU_menu.c
 *
 *  Created on: 2025年10月21日
 *      Author: 凯
 */

#include "zf_common_headfile.h"

uint8 menu_serial_number_One=1;//一级菜单序列号
uint8 menu_serial_number_Two=0;//二级菜单序列号
uint8 menu_serial_number_Three=0;//三级菜单序列号
uint8 menu_level = 1;
void menu_init(void)
{
    tft180_set_dir(TFT180_PORTAIT);                                           // 需要先横屏 不然显示不下
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
    if(menu_serial_number_Three != 0) menu_level = 3;     // 三级菜单序列号非0，说明处于三级菜单
    else if(menu_serial_number_Two != 0) menu_level = 2;  // 二级菜单序列号非0，说明处于二级菜单
    else if(menu_serial_number_One != 0) menu_level = 1;  // 一级菜单序列号非0，说明处于一级菜单
    // 注意：当所有序列号都为0时，menu_level保持为0，不显示任何菜单页面
    
    // 根据计算出的菜单层级，执行对应的菜单显示逻辑
    switch(menu_level)
    {
        case 1: // 一级菜单显示逻辑
            // 根据一级菜单序列号显示对应的页面
            switch(menu_serial_number_One)
            {
                case 1:
                    Page_One();     // 显示页面一
                    break;
                case 2:
                    Page_Two();     // 显示页面二
                    break;
                case 3:
                    Page_Three();   // 显示页面三
                    break;
                case 4:
                    Page_Four();    // 显示页面四
                    break;
                case 5:
                    //扩展菜单用，暂时不用
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
            break;  // 结束case 1
            
        case 2: // 二级菜单显示逻辑
            // 先根据一级菜单序列号确定主分类，再根据二级序列号确定子页面
            switch(menu_serial_number_One)
            {
                case 1: // 一级菜单第1项下的二级菜单
                    switch(menu_serial_number_Two)
                    {
                        case 1:
                            Page_One_1();  // 显示页面一的第一子页面
                            break;
                        case 2:
                            Page_One_2(); 
                            break;
                        case 3:
                            Page_One_3();  
                            break;
                        case 4:
                            Page_One_4();  // 显示页面一的第二子页面
                            break;
                        case 5:
                            Page_One_5();  // 显示页面一的第二子页面
                            break;
                        case 6:
                            Page_One_6();  // 显示页面一的第二子页面
                            break;
                        case 7:
                            Page_One_7();  // 显示页面一的第二子页面
                            break;
                        // 注意：可以在此添加更多case处理其他二级菜单项
                    }
                    break;  // 结束switch(menu_serial_number_Two)
                    
                case 2: // 一级菜单第2项下的二级菜单
                    switch(menu_serial_number_Two)
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
                    break;  // 结束switch(menu_serial_number_Two)
                case 3: 
                    switch(menu_serial_number_Two)
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
            }
            break;  // 结束case 2
            
        case 3: // 三级菜单显示逻辑
            // 三级菜单需要三个序列号确定具体页面：一级->二级->三级
            switch(menu_serial_number_One)
            {
                case 1: // 一级菜单第1项
                    switch(menu_serial_number_Two)  // 二级菜单选择
                    {
                        case 1: // 二级菜单第1项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_One_1_1();
                                    break;
                                case 2:
                                    tft180_show_string(0, 0, "Menu 1-1-2");
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                            
                        case 2: // 二级菜单第2项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_One_2_1();
                                    break;
                                case 2:
                                    tft180_show_string(0, 0, "Menu 1-2-2");
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                        case 3: // 二级菜单第2项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_One_3_1();
                                    break;
                                case 2:
                                    tft180_show_string(0, 0, "Menu 1-3-2");
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                        case 4: // 二级菜单第2项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_One_4_1();
                                    break;
                                case 2:
                                    tft180_show_string(0, 0, "Menu 1-4-2");
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                        case 5: // 二级菜单第2项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_One_5_1();
                                    break;
                                case 2:
                                    tft180_show_string(0, 0, "Menu 1-5-2");
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                        case 6: // 二级菜单第2项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_One_6_1();
                                    break;
                                case 2:
                                    tft180_show_string(0, 0, "Menu 1-6-2");
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                        case 7: // 二级菜单第2项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_One_7_1();
                                    break;
                                case 2:
                                    tft180_show_string(0, 0, "Menu 1-7-2");
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                                    
                        // 注意：可以在此添加更多case处理其他二级菜单项
                    }
                    break;  // 结束switch(menu_serial_number_Two)
                    
                case 2: // 一级菜单第2项
                    switch(menu_serial_number_Two)  // 二级菜单选择
                    {
                        case 1: // 二级菜单第1项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_Two_1_1();
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                            
                        case 2: // 二级菜单第2项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_Two_2_1();
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                        
                        case 3: // 二级菜单第2项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_Two_3_1();
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                        
                        case 4: // 二级菜单第2项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_Two_4_1();
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                        
                        case 5: // 二级菜单第2项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_Two_5_1();
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                        // 注意：可以在此添加更多case处理其他二级菜单项
                    }
                    break;  // 结束switch(menu_serial_number_Two)

                case 3: // 一级菜单第3项
                    switch(menu_serial_number_Two)  // 二级菜单选择
                    {
                        case 1: // 二级菜单第1项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_Three_1_1();
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                            
                        case 2: // 二级菜单第2项 (GPS-Point_Get)
                            Page_Three_2_1(); // 无论光标跑到几，都让这个函数去处理滑动刷新
                            break;
                        
                        case 3: // 二级菜单第2项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_Three_3_1();
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                        
                        case 4: // 二级菜单第2项
                            
                            Page_Three_4_1();
                            break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                        
                        case 5: // 二级菜单第2项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_Three_5_1();
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                        case 6: // 二级菜单第2项
                                Page_Three_6_1();
                            break;  // 结束switch(menu_serial_number_Three)
                        case 7: // 二级菜单第2项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_Three_7_1();
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                        case 8: // 二级菜单第2项
                            switch(menu_serial_number_Three)  // 三级菜单选择
                            {
                                case 1:
                                    Page_Three_8_1();
                                    break;
                                // 注意：可以在此添加更多case处理其他三级菜单项
                            }
                            break;  // 结束switch(menu_serial_number_Three)
                        // 注意：可以在此添加更多case处理其他二级菜单项
                    }
                    break;  // 结束switch(menu_serial_number_Two)
                    
                // 注意：可以在此添加更多case处理其他一级菜单项
            }
            break; 
            
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
    if(TimerTime%1000>=500)
    tft180_show_string(0,0,"Moter_Mode");
    else
    tft180_show_string(0,0,"            ");
    tft180_show_string(0,20,"PID");
    tft180_show_string(0,40,"GPS");
    tft180_show_string(0,60,"Menu_4");
    tft180_show_string(0,80,"Menu_5");
}

// 一级菜单第二项页面显示（闪烁效果）
void Page_Two(void)
{
    tft180_show_string(0,0,"Moter_Mode");
    if(TimerTime%1000>=500)
    tft180_show_string(0,20,"PID");
    else
    tft180_show_string(0,20,"            ");
    tft180_show_string(0,40,"GPS");
    tft180_show_string(0,60,"Menu_4");
}

// 一级菜单第三项页面显示（闪烁效果）
void Page_Three(void)
{
    tft180_show_string(0,0,"Moter_Mode");
    tft180_show_string(0,20,"PID");
    if(TimerTime%1000>=500)
    tft180_show_string(0,40,"GPS");
    else
    tft180_show_string(0,40,"            ");
    tft180_show_string(0,60,"Menu_4");
}

// 一级菜单第四项页面显示（闪烁效果）
void Page_Four(void)
{
    tft180_show_string(0,0,"Moter_Mode");
    tft180_show_string(0,20,"PID");
    tft180_show_string(0,40,"GPS");
    if(TimerTime%1000>=500)
    tft180_show_string(0,60,"Menu_4");
    else
    tft180_show_string(0,60,"            ");
}

// 二级菜单第一项页面显示（Moter_Mode子菜单第一项）
void Page_One_1(void)
{
    if(TimerTime%1000>=500)
    tft180_show_string(0,0,"Motor_Zero");
    else
    tft180_show_string(0,0,"            ");
    tft180_show_string(0,20,"Motor_IMU_Test");
    tft180_show_string(0,40,"Motor_Speed=0");
    tft180_show_string(0,60,"Motor_KeMu_1");
    tft180_show_string(0,80,"Motor_KM_2");
    tft180_show_string(0,100,"Motor_YaoKong");
    tft180_show_string(0,110,"Motor_GPS");
}

// 二级菜单第二项页面显示（Moter_Mode子菜单第二项）
void Page_One_2(void)
{
    tft180_show_string(0,0,"Motor_Zero");
    if(TimerTime%1000>=500)
    tft180_show_string(0,20,"Motor_IMU_Test");
    else
    tft180_show_string(0,20,"            ");
    tft180_show_string(0,40,"Motor_Speed=0");
    tft180_show_string(0,60,"Motor_KeMu_1");
    tft180_show_string(0,80,"Motor_KM_2");
    tft180_show_string(0,100,"Motor_YaoKong");
    tft180_show_string(0,110,"Motor_GPS");
}

// 二级菜单第三项页面显示（Moter_Mode子菜单第三项）
void Page_One_3(void)
{
    tft180_show_string(0,0,"Motor_Zero");
    tft180_show_string(0,20,"Motor_IMU_Test");
    if(TimerTime%1000>=500)
    tft180_show_string(0,40,"Motor_Speed=0");
    else
    tft180_show_string(0,40,"            ");
    tft180_show_string(0,60,"Motor_KeMu_1");
    tft180_show_string(0,80,"Motor_KM_2");
    tft180_show_string(0,100,"Motor_YaoKong");
    tft180_show_string(0,110,"Motor_GPS");
}

// 二级菜单第四项页面显示（Moter_Mode子菜单第四项）
void Page_One_4(void)
{
    tft180_show_string(0,0,"Motor_Zero");
    tft180_show_string(0,20,"Motor_IMU_Test");
    tft180_show_string(0,40,"Motor_Speed=0");
    if(TimerTime%1000>=500)
    tft180_show_string(0,60,"Motor_KeMu_1");
    else
    tft180_show_string(0,60,"            ");
    tft180_show_string(0,80,"Motor_KM_2");
    tft180_show_string(0,100,"Motor_YaoKong");
    tft180_show_string(0,110,"Motor_GPS");
}

// 二级菜单第五项页面显示（Moter_Mode子菜单第五项）
void Page_One_5(void)
{
    tft180_show_string(0,0,"Motor_Zero");
    tft180_show_string(0,20,"Motor_IMU_Test");
    tft180_show_string(0,40,"Motor_Speed=0");
    tft180_show_string(0,60,"Motor_KeMu_1");
    if(TimerTime%1000>=500)
    tft180_show_string(0,80,"Motor_KM_2");
    else
    tft180_show_string(0,80,"            ");
    tft180_show_string(0,100,"Motor_YaoKong");
    tft180_show_string(0,110,"Motor_GPS");
}

// 二级菜单第六项页面显示（Moter_Mode子菜单第六项）
void Page_One_6(void)
{
    tft180_show_string(0,0,"Motor_Zero");
    tft180_show_string(0,20,"Motor_IMU_Test");
    tft180_show_string(0,40,"Motor_Speed=0");
    tft180_show_string(0,60,"Motor_KeMu_1");
    tft180_show_string(0,80,"Motor_KM_2");
    if(TimerTime%1000>=500)
    tft180_show_string(0,100,"Motor_YaoKong");
    else
    tft180_show_string(0,100,"            ");
    tft180_show_string(0,110,"Motor_GPS");
}
void Page_One_7(void)
{
    tft180_show_string(0,0,"Motor_Zero");
    tft180_show_string(0,20,"Motor_IMU_Test");
    tft180_show_string(0,40,"Motor_Speed=0");
    tft180_show_string(0,60,"Motor_KeMu_1");
    tft180_show_string(0,80,"Motor_KM_2");
    tft180_show_string(0,100,"Motor_YaoKong");
    if(TimerTime%1000>=500)
    tft180_show_string(0,110,"Motor_GPS");
    else
    tft180_show_string(0,110,"            ");
}

// 三级菜单页面显示（Moter_Mode模式1）
void Page_One_1_1(void)
{
    tft180_show_string(0,0,"Motor Mod 1");
    Moter_Flag=0;
}

// 三级菜单页面显示（Moter_Mode模式2）
void Page_One_2_1(void)
{
    tft180_show_string(0,0,"Motor Mod 2");
    Moter_Flag=1;
}

// 三级菜单页面显示（Moter_Mode模式3）
void Page_One_3_1(void)
{
    tft180_show_string(0,0,"Motor Mod 3");
    Moter_Flag=2;
}

// 三级菜单页面显示（Moter_Mode模式4）
void Page_One_4_1(void)
{
    tft180_show_string(0,0,"Motor Mod 4");
    tft180_show_string(0,8,"Angle:");
    tft180_show_float(40,8,attitude.yaw,3,2);
    tft180_show_string(0,16,"X:");
    tft180_show_float(12,16,Robot_Pos_X,2,2);
    tft180_show_string(50,16,"Y:");
    tft180_show_float(62,16,Robot_Pos_Y,2,2);
    tft180_show_string(0,24,"Point:");
    tft180_show_uint(40,24,Target_Index,2);
    Moter_Flag=3;
}

// 三级菜单页面显示（Moter_Mode模式5）
void Page_One_5_1(void)
{
    tft180_show_string(0,0,"Motor Mod 5");
    Moter_Flag=4;
    tft180_show_string(0,8,"Angle:");
    tft180_show_float(40,8,attitude.yaw,3,2);
    tft180_show_string(0,16,"X:");
    tft180_show_float(12,16,Robot_Pos_X,2,2);
    tft180_show_string(50,16,"Y:");
    tft180_show_float(62,16,Robot_Pos_Y,2,2);
    tft180_show_string(0,24,"Point:");
    tft180_show_uint(40,24,Target_Index,2);
}

// 三级菜单页面显示（Moter_Mode模式6）
void Page_One_6_1(void)
{
    tft180_show_string(0,0,"Motor Mod 6");
    Moter_Flag=5;
    
}
void Page_One_7_1(void)
{
    tft180_show_string(0,0,"Motor Mod 7");
    Moter_Flag=6;
}
// 二级菜单第一项页面显示（PID子菜单第一项）
void Page_Two_1(void)
{
    if(TimerTime%1000>=500)
    {
    tft180_show_string(0,0,"Angle_V_P:");
    tft180_show_float(80,0,PID_Angular_V.Kp,1,2);
    }
    else
    tft180_show_string(0,0,"                    ");
    tft180_show_string(0,20,"Angle_P:");
    tft180_show_uint(65,20,PID_Angular.Kp,3);
    tft180_show_float(65,40,PID_Speed.Kp,1,3);
    tft180_show_float(65,60,PID_Speed.Ki,1,4);
    tft180_show_float(65,80,PID_Speed.Kd,1,3);
    tft180_show_string(0,40,"Speed_P:");
    tft180_show_string(0,60,"Speed_I:");
    tft180_show_string(0,80,"Speed_D:");
}

// 二级菜单第二项页面显示（PID子菜单第二项）
void Page_Two_2(void)
{
    tft180_show_float(80,0,PID_Angular_V.Kp,1,2);
    tft180_show_float(65,40,PID_Speed.Kp,1,3);
    tft180_show_float(65,60,PID_Speed.Ki,1,4);
    tft180_show_float(65,80,PID_Speed.Kd,1,3);
    tft180_show_string(0,0,"Angle_V_P:");
    if(TimerTime%1000>=500)
    {
    tft180_show_string(0,20,"Angle_P:");
    tft180_show_uint(65,20,PID_Angular.Kp,3);
    }
    else
    tft180_show_string(0,20,"                    ");
    tft180_show_string(0,40,"Speed_P:");
    tft180_show_string(0,60,"Speed_I:");
    tft180_show_string(0,80,"Speed_D:");
}

// 二级菜单第三项页面显示（PID子菜单第三项）
void Page_Two_3(void)
{
    tft180_show_float(80,0,PID_Angular_V.Kp,1,2);
    tft180_show_uint(65,20,PID_Angular.Kp,3);
    tft180_show_float(65,60,PID_Speed.Ki,1,4);
    tft180_show_float(65,80,PID_Speed.Kd,1,3);
    tft180_show_string(0,0,"Angle_V_P:");
    tft180_show_string(0,20,"Angle_P:");
    if(TimerTime%1000>=500)
    {
    tft180_show_float(65,40,PID_Speed.Kp,1,3);
    tft180_show_string(0,40,"Speed_P:");
    }
    else
    tft180_show_string(0,40,"                    ");
    tft180_show_string(0,60,"Speed_I:");
    tft180_show_string(0,80,"Speed_D:");
}

// 二级菜单第四项页面显示（PID子菜单第四项）
void Page_Two_4(void)
{
    tft180_show_float(80,0,PID_Angular_V.Kp,1,2);
    tft180_show_uint(65,20,PID_Angular.Kp,3);
    tft180_show_float(65,40,PID_Speed.Kp,1,3);
    tft180_show_float(65,80,PID_Speed.Kd,1,3);
    tft180_show_string(0,0,"Angle_V_P:");
    tft180_show_string(0,20,"Angle_P:");
    tft180_show_string(0,40,"Speed_P:");
    if(TimerTime%1000>=500)
    {
    tft180_show_float(65,60,PID_Speed.Ki,1,4);
    tft180_show_string(0,60,"Speed_I:");
    }
    else
    tft180_show_string(0,60,"                    ");
    tft180_show_string(0,80,"Speed_D:");
}

// 二级菜单第五项页面显示（PID子菜单第五项）
void Page_Two_5(void)
{
    tft180_show_float(80,0,PID_Angular_V.Kp,1,2);
    tft180_show_uint(65,20,PID_Angular.Kp,3);
    tft180_show_float(65,40,PID_Speed.Kp,1,3);
    tft180_show_float(65,60,PID_Speed.Ki,1,4);
    tft180_show_string(0,0,"Angle_V_P:");
    tft180_show_string(0,20,"Angle_P:");
    tft180_show_string(0,40,"Speed_P:");
    tft180_show_string(0,60,"Speed_I:");
    if(TimerTime%1000>=500)
    {
    tft180_show_float(65,80,PID_Speed.Kd,1,3);
    tft180_show_string(0,80,"Speed_D:");
    }
    else
    tft180_show_string(0,80,"                    ");
}

// 三级菜单页面显示（PID调节第一项）
void Page_Two_1_1(void)
{
    if(TimerTime%1000>=500)
    {
    tft180_show_string(0,0,"Angle_V_P:");
    tft180_show_float(80,0,PID_Angular_V.Kp,1,2);
    }
    else
    tft180_show_string(80,0,"         ");
    tft180_show_string(0,20,"Angle_P:");
    tft180_show_uint(65,20,PID_Angular.Kp,3);
    tft180_show_float(65,40,PID_Speed.Kp,1,3);
    tft180_show_float(65,60,PID_Speed.Ki,1,4);
    tft180_show_float(65,80,PID_Speed.Kd,1,3);
    tft180_show_string(0,40,"Speed_P:");
    tft180_show_string(0,60,"Speed_I:");
    tft180_show_string(0,80,"Speed_D:");
}

// 三级菜单页面显示（PID调节第二项）
void Page_Two_2_1(void)
{
    tft180_show_float(80,0,PID_Angular_V.Kp,1,2);
    tft180_show_float(65,40,PID_Speed.Kp,1,3);
    tft180_show_float(65,60,PID_Speed.Ki,1,4);
    tft180_show_float(65,80,PID_Speed.Kd,1,3);
    tft180_show_string(0,0,"Angle_V_P:");
    if(TimerTime%1000>=500)
    {
    tft180_show_string(0,20,"Angle_P:");
    tft180_show_uint(65,20,PID_Angular.Kp,3);
    }
    else
    tft180_show_string(65,20,"           ");
    tft180_show_string(0,40,"Speed_P:");
    tft180_show_string(0,60,"Speed_I:");
    tft180_show_string(0,80,"Speed_D:");
}

// 三级菜单页面显示（PID调节第三项）
void Page_Two_3_1(void)
{
    tft180_show_float(80,0,PID_Angular_V.Kp,1,2);
    tft180_show_uint(65,20,PID_Angular.Kp,3);
    tft180_show_float(65,60,PID_Speed.Ki,1,4);
    tft180_show_float(65,80,PID_Speed.Kd,1,3);
    tft180_show_string(0,0,"Angle_V_P:");
    tft180_show_string(0,20,"Angle_P:");
    if(TimerTime%1000>=500)
    {
    tft180_show_float(65,40,PID_Speed.Kp,1,3);
    tft180_show_string(0,40,"Speed_P:");
    }
    else
    tft180_show_string(65,40,"           ");
    tft180_show_string(0,60,"Speed_I:");
    tft180_show_string(0,80,"Speed_D:");
}

// 三级菜单页面显示（PID调节第四项）
void Page_Two_4_1(void)
{
    tft180_show_float(80,0,PID_Angular_V.Kp,1,2);
    tft180_show_uint(65,20,PID_Angular.Kp,3);
    tft180_show_float(65,40,PID_Speed.Kp,1,3);
    tft180_show_float(65,80,PID_Speed.Kd,1,3);
    tft180_show_string(0,0,"Angle_V_P:");
    tft180_show_string(0,20,"Angle_P:");
    tft180_show_string(0,40,"Speed_P:");
    if(TimerTime%1000>=500)
    {
    tft180_show_float(65,60,PID_Speed.Ki,1,4);
    tft180_show_string(0,60,"Speed_I:");
    }
    else
    tft180_show_string(65,60,"           ");
    tft180_show_string(0,80,"Speed_D:");
}

// 三级菜单页面显示（PID调节第五项）
void Page_Two_5_1(void)
{
    tft180_show_float(80,0,PID_Angular_V.Kp,1,2);
    tft180_show_uint(65,20,PID_Angular.Kp,3);
    tft180_show_float(65,40,PID_Speed.Kp,1,3);
    tft180_show_float(65,60,PID_Speed.Ki,1,4);
    tft180_show_string(0,0,"Angle_V_P:");
    tft180_show_string(0,20,"Angle_P:");
    tft180_show_string(0,40,"Speed_P:");
    tft180_show_string(0,60,"Speed_I:");
    if(TimerTime%1000>=500)
    {
    tft180_show_float(65,80,PID_Speed.Kd,1,3);
    tft180_show_string(0,80,"Speed_D:");
    }
    else
    tft180_show_string(65,80,"           ");
}


void Page_Three_1(void)
{
    if(TimerTime%1000>=500)
    tft180_show_string(0,0,"GPS-Show");
    else
    tft180_show_string(0,0,"           ");
    tft180_show_string(0,8,"GPS-Point_Get");
    tft180_show_string(0,16,"GPS-PointNum");
    tft180_show_string(0,24,"IMU_KM1");
    tft180_show_string(0,32,"IMU_KM1-Show");
    tft180_show_string(0,40,"IMU_KM2");
    tft180_show_string(0,48,"IMU_KM2-Show");
    tft180_show_string(0,56,"GPS-Show");
}
void Page_Three_2(void)
{
    tft180_show_string(0,0,"GPS-Show");
    if(TimerTime%1000>=500)
    tft180_show_string(0,8,"GPS-Point_Get");
    else
    tft180_show_string(0,8,"           ");
    tft180_show_string(0,16,"GPS-PointNum");
    tft180_show_string(0,24,"IMU_KM1");
    tft180_show_string(0,32,"IMU_KM1-Show");
    tft180_show_string(0,40,"IMU_KM2");
    tft180_show_string(0,48,"IMU_KM2-Show");
    tft180_show_string(0,56,"GPS-Show");
}
void  Page_Three_3(void)
{
   
    tft180_show_string(0,0,"GPS-Show");
    tft180_show_string(0,8,"GPS-Point_Get");
    if(TimerTime%1000>=500)
    tft180_show_string(0,16,"GPS-PointNum");
    else
    tft180_show_string(0,16,"           ");
    tft180_show_string(0,24,"IMU_KM1");
    tft180_show_string(0,32,"IMU_KM1-Show");
    tft180_show_string(0,40,"IMU_KM2");
    tft180_show_string(0,48,"IMU_KM2-Show");
    tft180_show_string(0,56,"GPS-Show");
}
void  Page_Three_4(void)
{
    tft180_show_string(0,0,"GPS-Show");
    
    tft180_show_string(0,8,"GPS-Point_Get");
    tft180_show_string(0,16,"GPS-PointNum");
    if(TimerTime%1000>=500)
    tft180_show_string(0,24,"IMU_KM1");
    else
    tft180_show_string(0,24,"           ");
    tft180_show_string(0,32,"IMU_KM1-Show");
    tft180_show_string(0,40,"IMU_KM2");
    tft180_show_string(0,48,"IMU_KM2-Show");
    tft180_show_string(0,56,"GPS-Show");
}
void  Page_Three_5 (void)
{
    tft180_show_string(0,0,"GPS-Show");
    tft180_show_string(0,8,"GPS-Point_Get");
    tft180_show_string(0,16,"GPS-PointNum");
    tft180_show_string(0,24,"IMU_KM1");
    if(TimerTime%1000>=500)
    tft180_show_string(0,32,"IMU_KM1-Show");
    else
    tft180_show_string(0,32,"              ");
    tft180_show_string(0,40,"IMU_KM2");
    tft180_show_string(0,48,"IMU_KM2-Show");
    tft180_show_string(0,56,"GPS-Show");
}
void  Page_Three_6(void)
{
    tft180_show_string(0,0,"GPS-Show");
    
    tft180_show_string(0,8,"GPS-Point_Get");
    tft180_show_string(0,16,"GPS-PointNum");
    
    tft180_show_string(0,24,"IMU_KM1");
    
    tft180_show_string(0,32,"IMU_KM1-Show");
    if(TimerTime%1000>=500)
    tft180_show_string(0,40,"IMU_KM2");
    else
    tft180_show_string(0,40,"              ");
    tft180_show_string(0,48,"IMU_KM2-Show");
    tft180_show_string(0,56,"GPS-Show");
}
void  Page_Three_7 (void)
{
    tft180_show_string(0,0,"GPS-Show");
    tft180_show_string(0,8,"GPS-Point_Get");
    tft180_show_string(0,16,"GPS-PointNum");
    tft180_show_string(0,24,"IMU_KM1");
    
    tft180_show_string(0,32,"IMU_KM1-Show");
    
    tft180_show_string(0,40,"IMU_KM2");
    if(TimerTime%1000>=500)
    tft180_show_string(0,48,"IMU_KM2-Show");
    else
    tft180_show_string(0,48,"              ");
    tft180_show_string(0,56,"GPS-Show");
}
void  Page_Three_8 (void)
{
    tft180_show_string(0,0,"GPS-Show");
    tft180_show_string(0,8,"GPS-Point_Get");
    tft180_show_string(0,16,"GPS-PointNum");
    tft180_show_string(0,24,"IMU_KM1");
    tft180_show_string(0,32,"IMU_KM1-Show");
    tft180_show_string(0,40,"IMU_KM2");
    tft180_show_string(0,48,"IMU_KM2-Show");
    if(TimerTime%1000>=500)
    tft180_show_string(0,56,"GPS-Show");
    else
    tft180_show_string(0,56,"           ");
}
void Page_Three_5_1(void)
{
    uint8 HuaDong=0;
    tft180_show_string(0,0,"Now:");tft180_show_float(24,0,Robot_Pos_X, 2, 2);   tft180_show_float(6*11,0,Robot_Pos_Y, 2, 2);
    tft180_show_uint(6,8*1,HuaDong,2);tft180_show_float(24,8*1,IMU_Points_used[HuaDong].x, 2, 2);   tft180_show_float(6*11,8*1,IMU_Points_used[HuaDong].y, 2, 6);
    tft180_show_uint(6,8*2,HuaDong+1,2);tft180_show_float(24,8*2,IMU_Points_used[HuaDong+1].x, 2, 2);   tft180_show_float(6*11,8*2,IMU_Points_used[HuaDong+1].y, 2, 2);
    tft180_show_uint(6,8*3,HuaDong+2,2);tft180_show_float(24,8*3,IMU_Points_used[HuaDong+2].x, 2, 2);   tft180_show_float(6*11,8*3,IMU_Points_used[HuaDong+2].y, 2, 2);
    tft180_show_uint(6,8*4,HuaDong+3,2);tft180_show_float(24,8*4,IMU_Points_used[HuaDong+3].x, 2, 2);   tft180_show_float(6*11,8*4,IMU_Points_used[HuaDong+3].y, 2, 2);
    tft180_show_uint(6,8*5,HuaDong+4,2);tft180_show_float(24,8*5,IMU_Points_used[HuaDong+4].x, 2, 2);   tft180_show_float(6*11,8*5,IMU_Points_used[HuaDong+4].y, 2, 2);
    tft180_show_uint(6,8*6,HuaDong+5,2);tft180_show_float(24,8*6,IMU_Points_used[HuaDong+5].x, 2, 2);   tft180_show_float(6*11,8*6,IMU_Points_used[HuaDong+5].y, 2, 2);
    tft180_show_uint(6,8*7,HuaDong+6,2);tft180_show_float(24,8*7,IMU_Points_used[HuaDong+6].x, 2, 2);   tft180_show_float(6*11,8*7,IMU_Points_used[HuaDong+6].y, 2, 2);                                        
    tft180_show_uint(6,8*8,HuaDong+7,2);tft180_show_float(24,8*8,IMU_Points_used[HuaDong+7].x, 2, 2);   tft180_show_float(6*11,8*8,IMU_Points_used[HuaDong+7].y, 2, 2);
    tft180_show_uint(6,8*9,HuaDong+8,2);tft180_show_float(24,8*9,IMU_Points_used[HuaDong+8].x, 2, 2);   tft180_show_float(6*11,8*9,IMU_Points_used[HuaDong+8].y, 2,2);
    tft180_show_uint(6,8*10,HuaDong+9,2);tft180_show_float(24,8*10,IMU_Points_used[HuaDong+9].x, 2, 2);   tft180_show_float(6*11,8*10,IMU_Points_used[HuaDong+9].y, 2, 2);
    tft180_show_uint(6,8*11,HuaDong+10,2);tft180_show_float(24,8*11,IMU_Points_used[HuaDong+10].x, 2, 2);   tft180_show_float(6*11,8*11,IMU_Points_used[HuaDong+10].y, 2, 2);
    tft180_show_uint(6,8*12,HuaDong+11,2);tft180_show_float(24,8*12,IMU_Points_used[HuaDong+11].x, 2, 2);   tft180_show_float(6*11,8*12,IMU_Points_used[HuaDong+11].y, 2, 2);
    tft180_show_uint(6,8*13,HuaDong+12,2);tft180_show_float(24,8*13,IMU_Points_used[HuaDong+12].x, 2, 2);   tft180_show_float(6*11,8*13,IMU_Points_used[HuaDong+12].y, 2, 2);
    tft180_show_uint(6,8*14,HuaDong+13,2);tft180_show_float(24,8*14,IMU_Points_used[HuaDong+13].x, 2, 2);   tft180_show_float(6*11,8*14,IMU_Points_used[HuaDong+13].y, 2, 2);
    tft180_show_uint(6,8*15,HuaDong+14,2);tft180_show_float(24,8*15,IMU_Points_used[HuaDong+14].x, 2, 2);   tft180_show_float(6*11,8*15,IMU_Points_used[HuaDong+14].y, 2, 2);
    tft180_show_uint(6,8*16,HuaDong+15,2);tft180_show_float(24,8*16,IMU_Points_used[HuaDong+15].x, 2, 2);   tft180_show_float(6*11,8*16,IMU_Points_used[HuaDong+15].y, 2, 2);
    tft180_show_uint(6,8*17,HuaDong+16,2);tft180_show_float(24,8*17,IMU_Points_used[HuaDong+16].x, 2, 2);   tft180_show_float(6*11,8*17,IMU_Points_used[HuaDong+16].y, 2, 2);
    tft180_show_uint(6,8*18,HuaDong+17,2);tft180_show_float(24,8*18,IMU_Points_used[HuaDong+17].x, 2, 2);   tft180_show_float(6*11,8*18,IMU_Points_used[HuaDong+17].y, 2, 2);
}
void Page_Three_4_1(void)
{
    uint8 HuaDong=0;
    if(menu_serial_number_Three>=18)
    {
        HuaDong = menu_serial_number_Three - 18;    
    }
    tft180_show_string(0,0,"Now:");tft180_show_float(24,0,Robot_Pos_X, 2, 2);   tft180_show_float(6*11,0,Robot_Pos_Y, 2, 2);
    tft180_show_uint(6,8*1,HuaDong,2);tft180_show_float(24,8*1,IMU_Points[HuaDong].x, 2, 2);   tft180_show_float(6*11,8*1,IMU_Points[HuaDong].y, 2, 6);
    tft180_show_uint(6,8*2,HuaDong+1,2);tft180_show_float(24,8*2,IMU_Points[HuaDong+1].x, 2, 2);   tft180_show_float(6*11,8*2,IMU_Points[HuaDong+1].y, 2, 2);
    tft180_show_uint(6,8*3,HuaDong+2,2);tft180_show_float(24,8*3,IMU_Points[HuaDong+2].x, 2, 2);   tft180_show_float(6*11,8*3,IMU_Points[HuaDong+2].y, 2, 2);
    tft180_show_uint(6,8*4,HuaDong+3,2);tft180_show_float(24,8*4,IMU_Points[HuaDong+3].x, 2, 2);   tft180_show_float(6*11,8*4,IMU_Points[HuaDong+3].y, 2, 2);
    tft180_show_uint(6,8*5,HuaDong+4,2);tft180_show_float(24,8*5,IMU_Points[HuaDong+4].x, 2, 2);   tft180_show_float(6*11,8*5,IMU_Points[HuaDong+4].y, 2, 2);
    tft180_show_uint(6,8*6,HuaDong+5,2);tft180_show_float(24,8*6,IMU_Points[HuaDong+5].x, 2, 2);   tft180_show_float(6*11,8*6,IMU_Points[HuaDong+5].y, 2, 2);
    tft180_show_uint(6,8*7,HuaDong+6,2);tft180_show_float(24,8*7,IMU_Points[HuaDong+6].x, 2, 2);   tft180_show_float(6*11,8*7,IMU_Points[HuaDong+6].y, 2, 2);                                        
    tft180_show_uint(6,8*8,HuaDong+7,2);tft180_show_float(24,8*8,IMU_Points[HuaDong+7].x, 2, 2);   tft180_show_float(6*11,8*8,IMU_Points[HuaDong+7].y, 2, 2);
    tft180_show_uint(6,8*9,HuaDong+8,2);tft180_show_float(24,8*9,IMU_Points[HuaDong+8].x, 2, 2);   tft180_show_float(6*11,8*9,IMU_Points[HuaDong+8].y, 2,2);
    tft180_show_uint(6,8*10,HuaDong+9,2);tft180_show_float(24,8*10,IMU_Points[HuaDong+9].x, 2, 2);   tft180_show_float(6*11,8*10,IMU_Points[HuaDong+9].y, 2, 2);
    tft180_show_uint(6,8*11,HuaDong+10,2);tft180_show_float(24,8*11,IMU_Points[HuaDong+10].x, 2, 2);   tft180_show_float(6*11,8*11,IMU_Points[HuaDong+10].y, 2, 2);
    tft180_show_uint(6,8*12,HuaDong+11,2);tft180_show_float(24,8*12,IMU_Points[HuaDong+11].x, 2, 2);   tft180_show_float(6*11,8*12,IMU_Points[HuaDong+11].y, 2, 2);
    tft180_show_uint(6,8*13,HuaDong+12,2);tft180_show_float(24,8*13,IMU_Points[HuaDong+12].x, 2, 2);   tft180_show_float(6*11,8*13,IMU_Points[HuaDong+12].y, 2, 2);
    tft180_show_uint(6,8*14,HuaDong+13,2);tft180_show_float(24,8*14,IMU_Points[HuaDong+13].x, 2, 2);   tft180_show_float(6*11,8*14,IMU_Points[HuaDong+13].y, 2, 2);
    tft180_show_uint(6,8*15,HuaDong+14,2);tft180_show_float(24,8*15,IMU_Points[HuaDong+14].x, 2, 2);   tft180_show_float(6*11,8*15,IMU_Points[HuaDong+14].y, 2, 2);
    tft180_show_uint(6,8*16,HuaDong+15,2);tft180_show_float(24,8*16,IMU_Points[HuaDong+15].x, 2, 2);   tft180_show_float(6*11,8*16,IMU_Points[HuaDong+15].y, 2, 2);
    tft180_show_uint(6,8*17,HuaDong+16,2);tft180_show_float(24,8*17,IMU_Points[HuaDong+16].x, 2, 2);   tft180_show_float(6*11,8*17,IMU_Points[HuaDong+16].y, 2, 2);
    tft180_show_uint(6,8*18,HuaDong+17,2);tft180_show_float(24,8*18,IMU_Points[HuaDong+17].x, 2, 2);   tft180_show_float(6*11,8*18,IMU_Points[HuaDong+17].y, 2, 2);
    if(menu_serial_number_Three<18)
    tft180_show_string(0,8*(menu_serial_number_Three),"*");
    else
    tft180_show_string(0,8*(18),"*");
}
void Page_Three_3_1(void)
{ 
    uint8 HuaDong=0;
    tft180_show_uint(6,8*1,HuaDong,2);tft180_show_float(24,8*1,XY_Points[HuaDong].x, 2, 2);   tft180_show_float(6*11,8*1,XY_Points[HuaDong].y, 2, 6);
    tft180_show_uint(6,8*2,HuaDong+1,2);tft180_show_float(24,8*2,XY_Points[HuaDong+1].x, 2, 2);   tft180_show_float(6*11,8*2,XY_Points[HuaDong+1].y, 2, 2);
    tft180_show_uint(6,8*3,HuaDong+2,2);tft180_show_float(24,8*3,XY_Points[HuaDong+2].x, 2, 2);   tft180_show_float(6*11,8*3,XY_Points[HuaDong+2].y, 2, 2);
    tft180_show_uint(6,8*4,HuaDong+3,2);tft180_show_float(24,8*4,XY_Points[HuaDong+3].x, 2, 2);   tft180_show_float(6*11,8*4,XY_Points[HuaDong+3].y, 2, 2);
    tft180_show_uint(6,8*5,HuaDong+4,2);tft180_show_float(24,8*5,XY_Points[HuaDong+4].x, 2, 2);   tft180_show_float(6*11,8*5,XY_Points[HuaDong+4].y, 2, 2);
    tft180_show_uint(6,8*6,HuaDong+5,2);tft180_show_float(24,8*6,XY_Points[HuaDong+5].x, 2, 2);   tft180_show_float(6*11,8*6,XY_Points[HuaDong+5].y, 2, 2);
    tft180_show_uint(6,8*7,HuaDong+6,2);tft180_show_float(24,8*7,XY_Points[HuaDong+6].x, 2, 2);   tft180_show_float(6*11,8*7,XY_Points[HuaDong+6].y, 2, 2);                                        
    tft180_show_uint(6,8*8,HuaDong+7,2);tft180_show_float(24,8*8,XY_Points[HuaDong+7].x, 2, 2);   tft180_show_float(6*11,8*8,XY_Points[HuaDong+7].y, 2, 2);
    tft180_show_uint(6,8*9,HuaDong+8,2);tft180_show_float(24,8*9,XY_Points[HuaDong+8].x, 2, 2);   tft180_show_float(6*11,8*9,XY_Points[HuaDong+8].y, 2,2);
    tft180_show_uint(6,8*10,HuaDong+9,2);tft180_show_float(24,8*10,XY_Points[HuaDong+9].x, 2, 2);   tft180_show_float(6*11,8*10,XY_Points[HuaDong+9].y, 2, 2);
    tft180_show_uint(6,8*11,HuaDong+10,2);tft180_show_float(24,8*11,XY_Points[HuaDong+10].x, 2, 2);   tft180_show_float(6*11,8*11,XY_Points[HuaDong+10].y, 2, 2);
    tft180_show_uint(6,8*12,HuaDong+11,2);tft180_show_float(24,8*12,XY_Points[HuaDong+11].x, 2, 2);   tft180_show_float(6*11,8*12,XY_Points[HuaDong+11].y, 2, 2);
    tft180_show_uint(6,8*13,HuaDong+12,2);tft180_show_float(24,8*13,XY_Points[HuaDong+12].x, 2, 2);   tft180_show_float(6*11,8*13,XY_Points[HuaDong+12].y, 2, 2);
    tft180_show_uint(6,8*14,HuaDong+13,2);tft180_show_float(24,8*14,XY_Points[HuaDong+13].x, 2, 2);   tft180_show_float(6*11,8*14,XY_Points[HuaDong+13].y, 2, 2);
    tft180_show_uint(6,8*15,HuaDong+14,2);tft180_show_float(24,8*15,XY_Points[HuaDong+14].x, 2, 2);   tft180_show_float(6*11,8*15,XY_Points[HuaDong+14].y, 2, 2);
    tft180_show_uint(6,8*16,HuaDong+15,2);tft180_show_float(24,8*16,XY_Points[HuaDong+15].x, 2, 2);   tft180_show_float(6*11,8*16,XY_Points[HuaDong+15].y, 2, 2);
    tft180_show_uint(6,8*17,HuaDong+16,2);tft180_show_float(24,8*17,XY_Points[HuaDong+16].x, 2, 2);   tft180_show_float(6*11,8*17,XY_Points[HuaDong+16].y, 2, 2);
    tft180_show_uint(6,8*18,HuaDong+17,2);tft180_show_float(24,8*18,XY_Points[HuaDong+17].x, 2, 2);   tft180_show_float(6*11,8*18,XY_Points[HuaDong+17].y, 2, 2);
}

void Page_Three_2_1(void)
{
    uint8 HuaDong=0;
    if(menu_serial_number_Three>=18)
    {
        HuaDong = menu_serial_number_Three - 18;    
    }
    tft180_show_string(0,0,"Now:");tft180_show_float(24,0,gnss.longitude, 1, 6);   tft180_show_float(6*12,0,gnss.latitude, 1, 6);
    tft180_show_uint(6,8*1,HuaDong,2);tft180_show_float(24,8*1,Route_Points[HuaDong].longitude, 1, 6);   tft180_show_float(6*12,8*1,Route_Points[HuaDong].latitude, 1, 6);
    tft180_show_uint(6,8*2,HuaDong+1,2);tft180_show_float(24,8*2,Route_Points[HuaDong+1].longitude, 1, 6);   tft180_show_float(6*12,8*2,Route_Points[HuaDong+1].latitude, 1, 6);
    tft180_show_uint(6,8*3,HuaDong+2,2);tft180_show_float(24,8*3,Route_Points[HuaDong+2].longitude, 1, 6);   tft180_show_float(6*12,8*3,Route_Points[HuaDong+2].latitude, 1, 6);
    tft180_show_uint(6,8*4,HuaDong+3,2);tft180_show_float(24,8*4,Route_Points[HuaDong+3].longitude, 1, 6);   tft180_show_float(6*12,8*4,Route_Points[HuaDong+3].latitude, 1, 6);
    tft180_show_uint(6,8*5,HuaDong+4,2);tft180_show_float(24,8*5,Route_Points[HuaDong+4].longitude, 1, 6);   tft180_show_float(6*12,8*5,Route_Points[HuaDong+4].latitude, 1, 6);
    tft180_show_uint(6,8*6,HuaDong+5,2);tft180_show_float(24,8*6,Route_Points[HuaDong+5].longitude, 1, 6);   tft180_show_float(6*12,8*6,Route_Points[HuaDong+5].latitude, 1, 6);
    tft180_show_uint(6,8*7,HuaDong+6,2);tft180_show_float(24,8*7,Route_Points[HuaDong+6].longitude, 1, 6);   tft180_show_float(6*12,8*7,Route_Points[HuaDong+6].latitude, 1, 6);                                        
    tft180_show_uint(6,8*8,HuaDong+7,2);tft180_show_float(24,8*8,Route_Points[HuaDong+7].longitude, 1, 6);   tft180_show_float(6*12,8*8,Route_Points[HuaDong+7].latitude, 1, 6);
    tft180_show_uint(6,8*9,HuaDong+8,2);tft180_show_float(24,8*9,Route_Points[HuaDong+8].longitude, 1, 6);   tft180_show_float(6*12,8*9,Route_Points[HuaDong+8].latitude, 1, 6);
    tft180_show_uint(6,8*10,HuaDong+9,2);tft180_show_float(24,8*10,Route_Points[HuaDong+9].longitude, 1, 6);   tft180_show_float(6*12,8*10,Route_Points[HuaDong+9].latitude, 1, 6);
    tft180_show_uint(6,8*11,HuaDong+10,2);tft180_show_float(24,8*11,Route_Points[HuaDong+10].longitude, 1, 6);   tft180_show_float(6*12,8*11,Route_Points[HuaDong+10].latitude, 1, 6);
    tft180_show_uint(6,8*12,HuaDong+11,2);tft180_show_float(24,8*12,Route_Points[HuaDong+11].longitude, 1, 6);   tft180_show_float(6*12,8*12,Route_Points[HuaDong+11].latitude, 1, 6);
    tft180_show_uint(6,8*13,HuaDong+12,2);tft180_show_float(24,8*13,Route_Points[HuaDong+12].longitude, 1, 6);   tft180_show_float(6*12,8*13,Route_Points[HuaDong+12].latitude, 1, 6);
    tft180_show_uint(6,8*14,HuaDong+13,2);tft180_show_float(24,8*14,Route_Points[HuaDong+13].longitude, 1, 6);   tft180_show_float(6*12,8*14,Route_Points[HuaDong+13].latitude, 1, 6);
    tft180_show_uint(6,8*15,HuaDong+14,2);tft180_show_float(24,8*15,Route_Points[HuaDong+14].longitude, 1, 6);   tft180_show_float(6*12,8*15,Route_Points[HuaDong+14].latitude, 1, 6);
    tft180_show_uint(6,8*16,HuaDong+15,2);tft180_show_float(24,8*16,Route_Points[HuaDong+15].longitude, 1, 6);   tft180_show_float(6*12,8*16,Route_Points[HuaDong+15].latitude,1, 6);
    tft180_show_uint(6,8*17,HuaDong+16,2);tft180_show_float(24,8*17,Route_Points[HuaDong+16].longitude, 1, 6);   tft180_show_float(6*12,8*17,Route_Points[HuaDong+16].latitude, 1, 6);
    tft180_show_uint(6,8*18,HuaDong+17,2);tft180_show_float(24,8*18,Route_Points[HuaDong+17].longitude, 1, 6);   tft180_show_float(6*12,8*18,Route_Points[HuaDong+17].latitude, 1, 6);
    if(menu_serial_number_Three<18)
    tft180_show_string(0,8*(menu_serial_number_Three),"*");
    else
    tft180_show_string(0,8*(18),"*");
}
void Page_Three_1_1(void)
{
   tft180_show_uint(  0, 8*0, gnss.time.year, 4);
    tft180_show_uint( 40, 8*0, gnss.time.month, 2);
    tft180_show_uint( 80, 8*0, gnss.time.day, 2);
    tft180_show_string(0,8*1,"sta:");
    tft180_show_uint(  40, 8*1, gnss.state, 5);
    tft180_show_string(0,8*2,"E:");
    tft180_show_float( 40, 8*2, gnss.latitude, 4, 6);
    tft180_show_string(0,8*3,"N:");
    tft180_show_float( 40, 8*3, gnss.longitude, 4, 6);
    tft180_show_string(0,8*4,"V:");
    tft180_show_float( 40, 8*4, gnss.speed, 4, 6);
    tft180_show_string(0,8*5,"dir:");
    tft180_show_float( 40, 8*5, gnss.direction, 4, 6);
    tft180_show_string(0,8*6,"num:");
    tft180_show_uint(40, 8*6, gnss.satellite_used, 5);
}

void Page_Three_7_1(void)
{
    uint8 HuaDong=0;
    tft180_show_string(0,0,"Now:");tft180_show_float(24,0,Robot_Pos_X, 2, 2);   tft180_show_float(6*11,0,Robot_Pos_Y, 2, 2);
    tft180_show_uint(6,8*1,HuaDong,2);tft180_show_float(24,8*1,IMU_Points_used_KM2[HuaDong].x, 2, 2);   tft180_show_float(6*11,8*1,IMU_Points_used_KM2[HuaDong].y, 2, 6);
    tft180_show_uint(6,8*2,HuaDong+1,2);tft180_show_float(24,8*2,IMU_Points_used_KM2[HuaDong+1].x, 2, 2);   tft180_show_float(6*11,8*2,IMU_Points_used_KM2[HuaDong+1].y, 2, 2);
    tft180_show_uint(6,8*3,HuaDong+2,2);tft180_show_float(24,8*3,IMU_Points_used_KM2[HuaDong+2].x, 2, 2);   tft180_show_float(6*11,8*3,IMU_Points_used_KM2[HuaDong+2].y, 2, 2);
    tft180_show_uint(6,8*4,HuaDong+3,2);tft180_show_float(24,8*4,IMU_Points_used_KM2[HuaDong+3].x, 2, 2);   tft180_show_float(6*11,8*4,IMU_Points_used_KM2[HuaDong+3].y, 2, 2);
    tft180_show_uint(6,8*5,HuaDong+4,2);tft180_show_float(24,8*5,IMU_Points_used_KM2[HuaDong+4].x, 2, 2);   tft180_show_float(6*11,8*5,IMU_Points_used_KM2[HuaDong+4].y, 2, 2);
    tft180_show_uint(6,8*6,HuaDong+5,2);tft180_show_float(24,8*6,IMU_Points_used_KM2[HuaDong+5].x, 2, 2);   tft180_show_float(6*11,8*6,IMU_Points_used_KM2[HuaDong+5].y, 2, 2);
    tft180_show_uint(6,8*7,HuaDong+6,2);tft180_show_float(24,8*7,IMU_Points_used_KM2[HuaDong+6].x, 2, 2);   tft180_show_float(6*11,8*7,IMU_Points_used_KM2[HuaDong+6].y, 2, 2);                                        
    tft180_show_uint(6,8*8,HuaDong+7,2);tft180_show_float(24,8*8,IMU_Points_used_KM2[HuaDong+7].x, 2, 2);   tft180_show_float(6*11,8*8,IMU_Points_used_KM2[HuaDong+7].y, 2, 2);
    tft180_show_uint(6,8*9,HuaDong+8,2);tft180_show_float(24,8*9,IMU_Points_used_KM2[HuaDong+8].x, 2, 2);   tft180_show_float(6*11,8*9,IMU_Points_used_KM2[HuaDong+8].y, 2,2);
    tft180_show_uint(6,8*10,HuaDong+9,2);tft180_show_float(24,8*10,IMU_Points_used_KM2[HuaDong+9].x, 2, 2);   tft180_show_float(6*11,8*10,IMU_Points_used_KM2[HuaDong+9].y, 2, 2);
    tft180_show_uint(6,8*11,HuaDong+10,2);tft180_show_float(24,8*11,IMU_Points_used_KM2[HuaDong+10].x, 2, 2);   tft180_show_float(6*11,8*11,IMU_Points_used_KM2[HuaDong+10].y, 2, 2);
    tft180_show_uint(6,8*12,HuaDong+11,2);tft180_show_float(24,8*12,IMU_Points_used_KM2[HuaDong+11].x, 2, 2);   tft180_show_float(6*11,8*12,IMU_Points_used_KM2[HuaDong+11].y, 2, 2);
    tft180_show_uint(6,8*13,HuaDong+12,2);tft180_show_float(24,8*13,IMU_Points_used_KM2[HuaDong+12].x, 2, 2);   tft180_show_float(6*11,8*13,IMU_Points_used_KM2[HuaDong+12].y, 2, 2);
    tft180_show_uint(6,8*14,HuaDong+13,2);tft180_show_float(24,8*14,IMU_Points_used_KM2[HuaDong+13].x, 2, 2);   tft180_show_float(6*11,8*14,IMU_Points_used_KM2[HuaDong+13].y, 2, 2);
    tft180_show_uint(6,8*15,HuaDong+14,2);tft180_show_float(24,8*15,IMU_Points_used_KM2[HuaDong+14].x, 2, 2);   tft180_show_float(6*11,8*15,IMU_Points_used_KM2[HuaDong+14].y, 2, 2);
    tft180_show_uint(6,8*16,HuaDong+15,2);tft180_show_float(24,8*16,IMU_Points_used_KM2[HuaDong+15].x, 2, 2);   tft180_show_float(6*11,8*16,IMU_Points_used_KM2[HuaDong+15].y, 2, 2);
    tft180_show_uint(6,8*17,HuaDong+16,2);tft180_show_float(24,8*17,IMU_Points_used_KM2[HuaDong+16].x, 2, 2);   tft180_show_float(6*11,8*17,IMU_Points_used_KM2[HuaDong+16].y, 2, 2);
    tft180_show_uint(6,8*18,HuaDong+17,2);tft180_show_float(24,8*18,IMU_Points_used_KM2[HuaDong+17].x, 2, 2);   tft180_show_float(6*11,8*18,IMU_Points_used_KM2[HuaDong+17].y, 2, 2);
}
void Page_Three_6_1(void)
{
    uint8 HuaDong=0;
    if(menu_serial_number_Three>=18)
    {
        HuaDong = menu_serial_number_Three - 18;    
    }
    tft180_show_string(0,0,"Now:");tft180_show_float(24,0,Robot_Pos_X, 2, 2);   tft180_show_float(6*11,0,Robot_Pos_Y, 2, 2);
    tft180_show_uint(6,8*1,HuaDong,2);tft180_show_float(24,8*1,IMU_Points_KM2[HuaDong].x, 2, 2);   tft180_show_float(6*11,8*1,IMU_Points_KM2[HuaDong].y, 2, 6);
    tft180_show_uint(6,8*2,HuaDong+1,2);tft180_show_float(24,8*2,IMU_Points_KM2[HuaDong+1].x, 2, 2);   tft180_show_float(6*11,8*2,IMU_Points_KM2[HuaDong+1].y, 2, 2);
    tft180_show_uint(6,8*3,HuaDong+2,2);tft180_show_float(24,8*3,IMU_Points_KM2[HuaDong+2].x, 2, 2);   tft180_show_float(6*11,8*3,IMU_Points_KM2[HuaDong+2].y, 2, 2);
    tft180_show_uint(6,8*4,HuaDong+3,2);tft180_show_float(24,8*4,IMU_Points_KM2[HuaDong+3].x, 2, 2);   tft180_show_float(6*11,8*4,IMU_Points_KM2[HuaDong+3].y, 2, 2);
    tft180_show_uint(6,8*5,HuaDong+4,2);tft180_show_float(24,8*5,IMU_Points_KM2[HuaDong+4].x, 2, 2);   tft180_show_float(6*11,8*5,IMU_Points_KM2[HuaDong+4].y, 2, 2);
    tft180_show_uint(6,8*6,HuaDong+5,2);tft180_show_float(24,8*6,IMU_Points_KM2[HuaDong+5].x, 2, 2);   tft180_show_float(6*11,8*6,IMU_Points_KM2[HuaDong+5].y, 2, 2);
    tft180_show_uint(6,8*7,HuaDong+6,2);tft180_show_float(24,8*7,IMU_Points_KM2[HuaDong+6].x, 2, 2);   tft180_show_float(6*11,8*7,IMU_Points_KM2[HuaDong+6].y, 2, 2);                                        
    tft180_show_uint(6,8*8,HuaDong+7,2);tft180_show_float(24,8*8,IMU_Points_KM2[HuaDong+7].x, 2, 2);   tft180_show_float(6*11,8*8,IMU_Points_KM2[HuaDong+7].y, 2, 2);
    tft180_show_uint(6,8*9,HuaDong+8,2);tft180_show_float(24,8*9,IMU_Points_KM2[HuaDong+8].x, 2, 2);   tft180_show_float(6*11,8*9,IMU_Points_KM2[HuaDong+8].y, 2,2);
    tft180_show_uint(6,8*10,HuaDong+9,2);tft180_show_float(24,8*10,IMU_Points_KM2[HuaDong+9].x, 2, 2);   tft180_show_float(6*11,8*10,IMU_Points_KM2[HuaDong+9].y, 2, 2);
    tft180_show_uint(6,8*11,HuaDong+10,2);tft180_show_float(24,8*11,IMU_Points_KM2[HuaDong+10].x, 2, 2);   tft180_show_float(6*11,8*11,IMU_Points_KM2[HuaDong+10].y, 2, 2);
    tft180_show_uint(6,8*12,HuaDong+11,2);tft180_show_float(24,8*12,IMU_Points_KM2[HuaDong+11].x, 2, 2);   tft180_show_float(6*11,8*12,IMU_Points_KM2[HuaDong+11].y, 2, 2);
    tft180_show_uint(6,8*13,HuaDong+12,2);tft180_show_float(24,8*13,IMU_Points_KM2[HuaDong+12].x, 2, 2);   tft180_show_float(6*11,8*13,IMU_Points_KM2[HuaDong+12].y, 2, 2);
    tft180_show_uint(6,8*14,HuaDong+13,2);tft180_show_float(24,8*14,IMU_Points_KM2[HuaDong+13].x, 2, 2);   tft180_show_float(6*11,8*14,IMU_Points_KM2[HuaDong+13].y, 2, 2);
    tft180_show_uint(6,8*15,HuaDong+14,2);tft180_show_float(24,8*15,IMU_Points_KM2[HuaDong+14].x, 2, 2);   tft180_show_float(6*11,8*15,IMU_Points_KM2[HuaDong+14].y, 2, 2);
    tft180_show_uint(6,8*16,HuaDong+15,2);tft180_show_float(24,8*16,IMU_Points_KM2[HuaDong+15].x, 2, 2);   tft180_show_float(6*11,8*16,IMU_Points_KM2[HuaDong+15].y, 2, 2);
    tft180_show_uint(6,8*17,HuaDong+16,2);tft180_show_float(24,8*17,IMU_Points_KM2[HuaDong+16].x, 2, 2);   tft180_show_float(6*11,8*17,IMU_Points_KM2[HuaDong+16].y, 2, 2);
    tft180_show_uint(6,8*18,HuaDong+17,2);tft180_show_float(24,8*18,IMU_Points_KM2[HuaDong+17].x, 2, 2);   tft180_show_float(6*11,8*18,IMU_Points_KM2[HuaDong+17].y, 2, 2);
    if(menu_serial_number_Three<18)
    tft180_show_string(0,8*(menu_serial_number_Three),"*");
    else
    tft180_show_string(0,8*(18),"*");
}
void Page_Three_8_1(void)
{
    uint8 HuaDong=0;
    if(menu_serial_number_Three>=18)
    {
        HuaDong = menu_serial_number_Three - 18;    
    }
    tft180_show_string(0,0,"Now:");tft180_show_float(24,0,Robot_Pos_X, 2, 2);   tft180_show_float(6*11,0,Robot_Pos_Y, 2, 2);
    tft180_show_uint(6,8*1,HuaDong,2);tft180_show_float(24,8*1,XY_Points_used[HuaDong].x, 2, 2);   tft180_show_float(6*11,8*1,XY_Points_used[HuaDong].y, 2, 6);
    tft180_show_uint(6,8*2,HuaDong+1,2);tft180_show_float(24,8*2,XY_Points_used[HuaDong+1].x, 2, 2);   tft180_show_float(6*11,8*2,XY_Points_used[HuaDong+1].y, 2, 2);
    tft180_show_uint(6,8*3,HuaDong+2,2);tft180_show_float(24,8*3,XY_Points_used[HuaDong+2].x, 2, 2);   tft180_show_float(6*11,8*3,XY_Points_used[HuaDong+2].y, 2, 2);
    tft180_show_uint(6,8*4,HuaDong+3,2);tft180_show_float(24,8*4,XY_Points_used[HuaDong+3].x, 2, 2);   tft180_show_float(6*11,8*4,XY_Points_used[HuaDong+3].y, 2, 2);
    tft180_show_uint(6,8*5,HuaDong+4,2);tft180_show_float(24,8*5,XY_Points_used[HuaDong+4].x, 2, 2);   tft180_show_float(6*11,8*5,XY_Points_used[HuaDong+4].y, 2, 2);
    tft180_show_uint(6,8*6,HuaDong+5,2);tft180_show_float(24,8*6,XY_Points_used[HuaDong+5].x, 2, 2);   tft180_show_float(6*11,8*6,XY_Points_used[HuaDong+5].y, 2, 2);
    tft180_show_uint(6,8*7,HuaDong+6,2);tft180_show_float(24,8*7,XY_Points_used[HuaDong+6].x, 2, 2);   tft180_show_float(6*11,8*7,XY_Points_used[HuaDong+6].y, 2, 2);                                        
    tft180_show_uint(6,8*8,HuaDong+7,2);tft180_show_float(24,8*8,XY_Points_used[HuaDong+7].x, 2, 2);   tft180_show_float(6*11,8*8,XY_Points_used[HuaDong+7].y, 2, 2);
    tft180_show_uint(6,8*9,HuaDong+8,2);tft180_show_float(24,8*9,XY_Points_used[HuaDong+8].x, 2, 2);   tft180_show_float(6*11,8*9,XY_Points_used[HuaDong+8].y, 2,2);
    tft180_show_uint(6,8*10,HuaDong+9,2);tft180_show_float(24,8*10,XY_Points_used[HuaDong+9].x, 2, 2);   tft180_show_float(6*11,8*10,XY_Points_used[HuaDong+9].y, 2, 2);
    tft180_show_uint(6,8*11,HuaDong+10,2);tft180_show_float(24,8*11,XY_Points_used[HuaDong+10].x, 2, 2);   tft180_show_float(6*11,8*11,XY_Points_used[HuaDong+10].y, 2, 2);
    tft180_show_uint(6,8*12,HuaDong+11,2);tft180_show_float(24,8*12,XY_Points_used[HuaDong+11].x, 2, 2);   tft180_show_float(6*11,8*12,XY_Points_used[HuaDong+11].y, 2, 2);
    tft180_show_uint(6,8*13,HuaDong+12,2);tft180_show_float(24,8*13,XY_Points_used[HuaDong+12].x, 2, 2);   tft180_show_float(6*11,8*13,XY_Points_used[HuaDong+12].y, 2, 2);
    tft180_show_uint(6,8*14,HuaDong+13,2);tft180_show_float(24,8*14,XY_Points_used[HuaDong+13].x, 2, 2);   tft180_show_float(6*11,8*14,XY_Points_used[HuaDong+13].y, 2, 2);
    tft180_show_uint(6,8*15,HuaDong+14,2);tft180_show_float(24,8*15,XY_Points_used[HuaDong+14].x, 2, 2);   tft180_show_float(6*11,8*15,XY_Points_used[HuaDong+14].y, 2, 2);
    tft180_show_uint(6,8*16,HuaDong+15,2);tft180_show_float(24,8*16,XY_Points_used[HuaDong+15].x, 2, 2);   tft180_show_float(6*11,8*16,XY_Points_used[HuaDong+15].y, 2, 2);
    tft180_show_uint(6,8*17,HuaDong+16,2);tft180_show_float(24,8*17,XY_Points_used[HuaDong+16].x, 2, 2);   tft180_show_float(6*11,8*17,XY_Points_used[HuaDong+16].y, 2, 2);
    tft180_show_uint(6,8*18,HuaDong+17,2);tft180_show_float(24,8*18,XY_Points_used[HuaDong+17].x, 2, 2);   tft180_show_float(6*11,8*18,XY_Points_used[HuaDong+17].y, 2, 2);
}

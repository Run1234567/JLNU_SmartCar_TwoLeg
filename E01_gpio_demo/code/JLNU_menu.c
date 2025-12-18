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
    tft180_set_dir(TFT180_CROSSWISE);                                           // 需要先横屏 不然显示不下
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
                        case 6:
                            Page_One_6();  // 显示页面一的第二子页面
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
    tft180_show_string(0,40,"Menu_3");
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
    tft180_show_string(0,40,"Menu_3");
    tft180_show_string(0,60,"Menu_4");
}

// 一级菜单第三项页面显示（闪烁效果）
void Page_Three(void)
{
    tft180_show_string(0,0,"Moter_Mode");
    tft180_show_string(0,20,"PID");
    if(TimerTime%1000>=500)
    tft180_show_string(0,40,"Menu_3");
    else
    tft180_show_string(0,40,"            ");
    tft180_show_string(0,60,"Menu_4");
}

// 一级菜单第四项页面显示（闪烁效果）
void Page_Four(void)
{
    tft180_show_string(0,0,"Moter_Mode");
    tft180_show_string(0,20,"PID");
    tft180_show_string(0,40,"Menu_3");
    if(TimerTime%1000>=500)
    tft180_show_string(0,60,"Menu_4");
    else
    tft180_show_string(0,60,"            ");
}

// 二级菜单第一项页面显示（Moter_Mode子菜单第一项）
void Page_One_1(void)
{
    if(TimerTime%1000>=500)
    tft180_show_string(0,0,"Motor Mod 1");
    else
    tft180_show_string(0,0,"            ");
    tft180_show_string(0,20,"Motor Mod 2");
    tft180_show_string(0,40,"Motor Mod 3");
    tft180_show_string(0,60,"Motor Mod 4");
    tft180_show_string(0,80,"Motor Mod 5");
    tft180_show_string(0,100,"Motor Mod 6");
}

// 二级菜单第二项页面显示（Moter_Mode子菜单第二项）
void Page_One_2(void)
{
    tft180_show_string(0,0,"Motor Mod 1");
    if(TimerTime%1000>=500)
    tft180_show_string(0,20,"Motor Mod 2");
    else
    tft180_show_string(0,20,"            ");
    tft180_show_string(0,40,"Motor Mod 3");
    tft180_show_string(0,60,"Motor Mod 4");
    tft180_show_string(0,80,"Motor Mod 5");
    tft180_show_string(0,100,"Motor Mod 6");
}

// 二级菜单第三项页面显示（Moter_Mode子菜单第三项）
void Page_One_3(void)
{
    tft180_show_string(0,0,"Motor Mod 1");
    tft180_show_string(0,20,"Motor Mod 2");
    if(TimerTime%1000>=500)
    tft180_show_string(0,40,"Motor Mod 3");
    else
    tft180_show_string(0,40,"            ");
    tft180_show_string(0,60,"Motor Mod 4");
    tft180_show_string(0,80,"Motor Mod 5");
    tft180_show_string(0,100,"Motor Mod 6");
}

// 二级菜单第四项页面显示（Moter_Mode子菜单第四项）
void Page_One_4(void)
{
    tft180_show_string(0,0,"Motor Mod 1");
    tft180_show_string(0,20,"Motor Mod 2");
    tft180_show_string(0,40,"Motor Mod 3");
    if(TimerTime%1000>=500)
    tft180_show_string(0,60,"Motor Mod 4");
    else
    tft180_show_string(0,60,"            ");
    tft180_show_string(0,80,"Motor Mod 5");
    tft180_show_string(0,100,"Motor Mod 6");
}

// 二级菜单第五项页面显示（Moter_Mode子菜单第五项）
void Page_One_5(void)
{
    tft180_show_string(0,0,"Motor Mod 1");
    tft180_show_string(0,20,"Motor Mod 2");
    tft180_show_string(0,40,"Motor Mod 3");
    tft180_show_string(0,60,"Motor Mod 4");
    if(TimerTime%1000>=500)
    tft180_show_string(0,80,"Motor Mod 5");
    else
    tft180_show_string(0,80,"            ");
    tft180_show_string(0,100,"Motor Mod 6");
}

// 二级菜单第六项页面显示（Moter_Mode子菜单第六项）
void Page_One_6(void)
{
    tft180_show_string(0,0,"Motor Mod 1");
    tft180_show_string(0,20,"Motor Mod 2");
    tft180_show_string(0,40,"Motor Mod 3");
    tft180_show_string(0,60,"Motor Mod 4");
    tft180_show_string(0,80,"Motor Mod 5");
    if(TimerTime%1000>=500)
    tft180_show_string(0,100,"Motor Mod 6");
    else
    tft180_show_string(0,100,"            ");
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
    Moter_Flag=3;
}

// 三级菜单页面显示（Moter_Mode模式5）
void Page_One_5_1(void)
{
    tft180_show_string(0,0,"Motor Mod 5");
    Moter_Flag=4;
}

// 三级菜单页面显示（Moter_Mode模式6）
void Page_One_6_1(void)
{
    tft180_show_string(0,0,"Motor Mod 6");
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
/*
 * JLNU_menu.c
 *
 *  Created on: 2025年10月21日
 *      Author: 凯
 */

#include "zf_common_headfile.h"

uint8_t lcd_flag = 0;//页面切换标志位
char text0[30];

void menu_init(void)
{
    tft180_set_dir(TFT180_CROSSWISE);                                           // 需要先横屏 不然显示不下
    tft180_init();
    system_delay_ms(1000);
    tft180_clear();
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     屏幕菜单函数
// 参数说明     void
// 返回参数     void
// 使用示例     tft_show();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void tft_show(void)
{
    switch(lcd_flag)
    {
        case 0:
            one_page();
            break;
        case 1:
            two_page();
            break;
        case 2:
            three_page();
            break;

    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     页面显示函数
// 参数说明     void
// 返回参数     void
// 使用示例     one_page();two_page();three_page();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void one_page(void)
{
//角度参数


  //  sprintf(text0,"  %.1f  ",Angle_Forward.filtering_angle);
    tft180_show_string(32, 0, text0);


//速度参数

    // sprintf(text0,"L:%d   ",Speed_Left);
    // tft180_show_string(0, 64, text0);
    // sprintf(text0,"R:%d   ",Speed_Right);
    // tft180_show_string(0, 48, text0);
//舵机pwm
    tft180_show_string(0, 32, "pwm :");
    sprintf(text0,"R:%d  ",pwm_r);
    tft180_show_string(48, 32, text0);
    sprintf(text0,"L:%d  ",pwm_l);
    tft180_show_string(96, 32, text0);


}
void two_page(void)
{
    tft180_show_string(0, 0, "mode :");

}
void three_page(void)
{
    tft180_show_string(0, 0, "points :");
}





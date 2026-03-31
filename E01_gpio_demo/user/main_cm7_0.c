/*********************************************************************************************************************
 * CYT4BB Opensourec Library 即（ CYT4BB 开源库）是一个基于官方 SDK 接口的第三方开源库
 * Copyright (c) 2022 SEEKFREE 逐飞科技
 *
 * 本文件是 CYT4BB 开源库的一部分
 *
 * CYT4BB 开源库 是免费软件
 * 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
 * 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
 *
 * 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
 * 甚至没有隐含的适销性或适合特定用途的保证
 * 更多细节请参见 GPL
 *
 * 您应该在收到本开源库的同时收到一份 GPL 的副本
 * 如果没有，请参阅<https://www.gnu.org/licenses/>
 *
 * 额外注明：
 * 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
 * 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
 * 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
 * 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
 *
 * 文件名称          main_cm7_0
 * 公司名称          成都逐飞科技有限公司
 * 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
 * 开发环境          IAR 9.40.1
 * 适用平台          CYT4BB
 * 店铺链接          https://seekfree.taobao.com/
 *
 * 修改记录
 * 日期              作者                备注
 * 2024-1-4       pudding            first version
 ********************************************************************************************************************/

#include "zf_common_headfile.h"

// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完

// *************************** 例程硬件连接说明 ***************************
// 核心板正常供电即可 无需额外连接
// 如果使用主板测试 主板必须要用电池供电

// *************************** 例程测试说明 ***************************
// 1.核心板烧录完成本例程，完成上电
// 2.可以看到核心板上四个 LED 呈流水灯状闪烁
// 3.将 SWITCH1 / SWITCH2 两个宏定义对应的引脚分别按照 00 01 10 11 的组合接到 1-VCC 0-GND 或者波动对应主板的拨码开关
// 3.不同的组合下，四个 LED 流水灯状闪烁的频率会发生变化
// 4.将 KEY1 / KEY2 / KEY3 / KEY4 两个宏定义对应的引脚接到 1-VCC 0-GND 或者 按对应按键
// 5.任意引脚接 GND 或者 按键按下会使得所有LED一起闪烁 松开后恢复流水灯
// 如果发现现象与说明严重不符 请参照本文件最下方 例程常见问题说明 进行排查

// **************************** 代码区域 ****************************
uint8 GPS_XY_Flag = 0; // 0表示还没记录过，1表示已经记录过了
int main(void)
{
    clock_init(SYSTEM_CLOCK_250M); // 时钟配置及系统初始化<务必保留>
    debug_init();
    DWT_Init();
    KEY_INIT();
    menu_init();


    Buzzer_Init();
    Buzzer_Time=500;
    gnss_init(TAU1201);
    imu660rc_init(IMU660RC_QUARTERNION_DISABLE);
    Servo_Four_Init();
    small_driver_uart_init();
    small_driver_get_speed();
    PID_Init_All();

    UART_Wireless_Init();
    Load_IMU_From_Flash();
    Load_IMU_KM2_From_Flash();
    Load_XY_From_Flash();
    Load_IMU_GPS_From_Flash();
    uart_receiver_init();

    pit_ms_init(PIT_CH10, 1000); // 接收控制中断
    pit_ms_init(PIT_CH2, 1);     // 开始小车中断
    static bool was_high = false;
    bool is_high = (High_Right_Point + High_Left_Point > 600);
    while (true)
    {
        Key_ISR();
        pwm_r = pwm_r + 0.1 * (High_Right_Point - pwm_r);
        pwm_l = pwm_l + 0.1 * (High_Left_Point - pwm_l);
        Servo_Leg_Control(pwm_l, pwm_r, PID_Speed.Output * 5);
        tft_show();
        if (High_Right_Point + High_Left_Point > 600)
        {
            Mechanical_Zero_Point = -1.5f;
        }
        else
        {
            Mechanical_Zero_Point = -5.1f;
        }
        if (is_high == true && was_high == false)
        {
            // 逻辑 A：刚才低于600，现在高于600 (跨越上边界)
            PID_Init(&PID_Angular_V, Angular_V_P_High, Angular_V_I_High, Angular_V_D_High, 0);
            PID_Init(&PID_Angular, Angular_P_High, Angular_I_High, Angular_D_High, 0);
            was_high = true; // 事情做完后，把“刚才”的状态更新为高
        }
        else if (is_high == false && was_high == true)
        {
            // 逻辑 B：刚才高于600，现在低于600 (跨越下边界)
            PID_Init(&PID_Angular_V, Angular_V_P, Angular_V_I, Angular_V_D, 0);
            PID_Init(&PID_Angular, Angular_P, Angular_I, Angular_D, 0);
            was_high = false; // 事情做完后，把“刚才”的状态更新为低
        }
        if (gnss_flag)
        {
            gnss_flag = 0;
            gnss_data_parse(); // 开始解析数据
            GPS_XY_Flag=1;
        }
        // if (uart_receiver.finsh_flag == 1)
        // {
        //     if (1 == uart_receiver.state) // 遥控器失控状态判断
        //     {
        //     }
        //     else
        //     {
        //     }
        //     uart_receiver.finsh_flag = 0;
        // }
        system_delay_ms(20);
        // 此处编写需要循环执行的代码
    }
}

// *************************** 例程常见问题说明 ***************************
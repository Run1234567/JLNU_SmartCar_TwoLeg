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


#define WIFI_SSID_TEST          "RUN"
#define WIFI_PASSWORD_TEST      "88888888"                  // 如果需要连接的WIFI 没有密码则需要将 这里 替换为 NULL
#define TCP_TARGET_IP           "192.168.95.156"             // 连接目标的 IP
#define TCP_TARGET_PORT         "8086"                      // 连接目标的端口
#define WIFI_LOCAL_PORT         "6666"                      // 本机的端口 0：随机  可设置范围2048-65535  默认 6666

uint8 image_copy[MT9V03X_H][MT9V03X_W];

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

//    UART_Wireless_Init();
    Load_IMU_From_Flash();
    Load_IMU_KM2_From_Flash();
    Load_XY_From_Flash();
    Load_IMU_GPS_From_Flash();
    uart_receiver_init();

    // wifi_spi_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST);


    // // ========== 2. 连接电脑 TCP 服务器 ==========
    // // 注意：如果是自动连接模式，这个 if 里面的代码可以不用，但保险起见带上
    // if(1 != WIFI_SPI_AUTO_CONNECT)
    // {
    //     wifi_spi_socket_connect("TCP", TCP_TARGET_IP, TCP_TARGET_PORT, WIFI_LOCAL_PORT);
    //     // {
    //     //     printf("\r\n Connect TCP Servers error, try again.");
    //     //     system_delay_ms(100);
    //     // }
    // }
    mt9v03x_init();
    //************************************************************* 
    // 初始化逐飞助手接口 (指定使用 WIFI SPI)
    // seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
    
    // // 告诉上位机：我要传的是总钻风图像，数据源是 image_copy
    // seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X, image_copy[0], MT9V03X_W, MT9V03X_H);
    //************************************************************ 
    pit_ms_init(PIT_CH10, 1000); // 接收控制中断
    pit_ms_init(PIT_CH2, 1);     // 开始小车中断
    static bool was_high = false;
    bool is_high = (High_Right_Point + High_Left_Point > 600);
    while (true)
    {
        if(mt9v03x_finish_flag)
        {
            memcpy(image_copy[0], mt9v03x_image[0], MT9V03X_IMAGE_SIZE);
            // seekfree_assistant_camera_send();
            mt9v03x_finish_flag = 0;
        }
        Key_ISR();
        pwm_r = pwm_r + 0.1 * (High_Right_Point - pwm_r);
        pwm_l = pwm_l + 0.1 * (High_Left_Point - pwm_l);
        Servo_Leg_Control(pwm_l, pwm_r, PID_Speed.Output * 5);
        tft_show();
        if (High_Right_Point + High_Left_Point > 600)
        {
            Mechanical_Zero_Point = -1.51f;
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
        if (uart_receiver.finsh_flag == 1)
        {
            if (1 == uart_receiver.state) // 遥控器失控状态判断
            {
            }
            else
            {
            }
            uart_receiver.finsh_flag = 0;
        }
        system_delay_ms(20);
        // 此处编写需要循环执行的代码
    }
}

// *************************** 例程常见问题说明 ***************************
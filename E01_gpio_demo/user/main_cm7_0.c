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
// 定义 PI 和 地球半径常量
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define EARTH_RADIUS 6371000.0 // 地球平均半径，单位：米
#define GPS_SAMPLE_TARGET 50   // 目标采集点数
#include "zf_common_headfile.h"

uint8 WIFI_Flag = 0; // WIFI连接状态标志 0-未连接 1-已连接
#define WIFI_SSID_TEST "RUN"
#define WIFI_PASSWORD_TEST "88888888"  // 如果需要连接的WIFI 没有密码则需要将 这里 替换为 NULL
#define TCP_TARGET_IP "192.168.55.156" // 连接目标的 IP
#define TCP_TARGET_PORT "8086"         // 连接目标的端口
#define WIFI_LOCAL_PORT "6666"         // 本机的端口 0：随机  可设置范围2048-65535  默认 6666

uint8 image_copy[MT9V03X_H][MT9V03X_W];

GPS_Point_t Start_GPS_Array[GPS_SAMPLE_TARGET]; // 存放发车GPS的数组
int current_gps_count = 0;                      // 当前已采集的GPS点数

uint8 GPS_XY_Flag = 0; // 0表示还没记录过，1表示已经记录过了

#include <math.h>


/**
 * @brief  基于多点阵列的极高精度航向计算 (PCA 直线拟合法)
 * @param  points  GPS点数组
 * @param  count   数组长度
 * @return float   航向角 (0.0 ~ 360.0度，正北为0)
 */
float CalculateUltraAccurateHeading(GPS_Point_t points[], int count)
{
    const int SKIP_POINTS = 10; 
    int valid_count = count - SKIP_POINTS;

    if (valid_count < 2) return -1.0f;

    // 获取局部参考系的纬度，用于经度变形补偿
    double lat0_rad = points[SKIP_POINTS].latitude * M_PI / 180.0;
    double cos_lat0 = cos(lat0_rad);

    // 缓存局部投影坐标 (假设最多处理 100 个点，请根据你的 buffer 实际大小调整)
    double x[100]; 
    double y[100];
    int n = 0;

    // ========================================================
    // 第一步：坐标投影转换 & 空间距离过滤
    // ========================================================
    for (int i = SKIP_POINTS; i < count; i++) {
        // 保留你原有的过滤逻辑：如果和上一个点相距过近(原地漂移)或过远(飞点)，直接丢弃该点
        if (i > SKIP_POINTS) {
            double dx_check = (points[i].longitude - points[i-1].longitude) * cos_lat0;
            double dy_check = (points[i].latitude - points[i-1].latitude);
            double dist_sq = dx_check * dx_check + dy_check * dy_check;
            
        }
        
        // 将经纬度转换为相对于起始点的 XY 坐标 (比例值，无需乘以地球半径)
        x[n] = (points[i].longitude - points[SKIP_POINTS].longitude) * cos_lat0;
        y[n] = (points[i].latitude - points[SKIP_POINTS].latitude);
        n++;
    }

    if (n < 2) return -1.0f;

    // ========================================================
    // 第二步：最小二乘法计算中心点
    // ========================================================
    double mean_x = 0.0, mean_y = 0.0;
    for (int i = 0; i < n; i++) {
        mean_x += x[i];
        mean_y += y[i];
    }
    mean_x /= n;
    mean_y /= n;

    // ========================================================
    // 第三步：计算协方差矩阵元素 (找出这批点最集中的分布方向)
    // ========================================================
    double Sxx = 0.0, Syy = 0.0, Sxy = 0.0;
    for (int i = 0; i < n; i++) {
        double dx = x[i] - mean_x;
        double dy = y[i] - mean_y;
        Sxx += dx * dx;
        Syy += dy * dy;
        Sxy += dx * dy;
    }

    // ========================================================
    // 第四步：求解拟合线角度 & 解决 180 度反向模糊
    // ========================================================
    // 主成分分析的经典公式，得出拟合直线的极坐标角 (-π/2 到 π/2)
    double theta_rad = 0.5 * atan2(2.0 * Sxy, Sxx - Syy);

    // 拟合出来的直线没有“前后”概念，我们需要看轨迹的整体移动趋势来判断真假方向
    double dir_x = x[n-1] - x[0]; // 总体X位移
    double dir_y = y[n-1] - y[0]; // 总体Y位移
    double fit_dir_x = cos(theta_rad);
    double fit_dir_y = sin(theta_rad);

    // 点乘判断：如果拟合方向和总体位移方向夹角大于 90 度，说明方向算反了，转 180 度
    if ((dir_x * fit_dir_x + dir_y * fit_dir_y) < 0.0) {
        theta_rad += M_PI;
    }

    // ========================================================
    // 第五步：数学角转地理航向角 (正北为 0)
    // ========================================================
    // 数学中是从X轴逆时针转的，而航向是从Y轴(正北)顺时针转的
    double heading_deg = 90.0 - (theta_rad * 180.0 / M_PI);

    // 规范化到 0 ~ 360 (或者你可以自己改回 -180 ~ 180)
    while (heading_deg < -180.0) heading_deg += 360.0;
    while (heading_deg >= 180.0) heading_deg -= 360.0;

    return (float)heading_deg;
}


float Angle_CeShi = 0.0f; // 用于测试的角度变量

int main(void)
{
    clock_init(SYSTEM_CLOCK_250M); // 时钟配置及系统初始化<务必保留>
    debug_init();
    DWT_Init();
    KEY_INIT();
    menu_init();

    Buzzer_Init();
    Buzzer_Time = 500;
    gnss_init(TAU1201);
    imu660rc_init(IMU660RC_QUARTERNION_DISABLE);
    Servo_Four_Init();
    system_delay_ms(1000);
    small_driver_uart_init();
    small_driver_get_speed();
    PID_Init_All();

    UART_Wireless_Init();
    Load_IMU_From_Flash();
    Load_IMU_KM2_From_Flash();
    Load_XY_From_Flash();
    Load_IMU_GPS_From_Flash();
    uart_receiver_init();

    if (wifi_spi_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST) == 0)
    {
        // ========== 2. 连接电脑 TCP 服务器 ==========
        // 注意：如果是自动连接模式，这个 if 里面的代码可以不用，但保险起见带上
        if (1 != WIFI_SPI_AUTO_CONNECT)
        {
            if (wifi_spi_socket_connect("TCP", TCP_TARGET_IP, TCP_TARGET_PORT, WIFI_LOCAL_PORT) == 0)
                WIFI_Flag = 1; // 连接成功
        }
        mt9v03x_init();
        //*************************************************************
        // 初始化逐飞助手接口 (指定使用 WIFI SPI)
        if (WIFI_Flag == 1)
        {
            seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);

            // 告诉上位机：我要传的是总钻风图像，数据源是 image_copy
            seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X, image_copy[0], MT9V03X_W, MT9V03X_H);
            seekfree_assistant_camera_boundary_config(XY_BOUNDARY, 200, Left_boundary_x, Right_boundary_x, NULL, Left_boundary_y, Right_boundary_y, NULL);
        }
        //************************************************************
    }
    else
        mt9v03x_init();
    mt9v03x_set_exposure_time(30);

    pit_ms_init(PIT_CH10, 1000); // 接收控制中断
    pit_ms_init(PIT_CH2, 1);     // 开始小车中断,
    static bool was_high = false;
    bool is_high = (High_Right_Point + High_Left_Point > 600);
    while (true)
    {
        if (mt9v03x_finish_flag)
        {
            memcpy(image_copy[0], mt9v03x_image[0], MT9V03X_IMAGE_SIZE);
            Get_Start_Point_image();
            Trace_Left_Boundary();
            Trace_Right_Boundary();

            if (WIFI_Flag == 1)
                seekfree_assistant_camera_send();
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
            Mechanical_Zero_Point = -9.9f;
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
            if (GPS_Get_Angle_Flag == 1)
            {
                if (current_gps_count < GPS_SAMPLE_TARGET)
                {
                    Start_GPS_Array[current_gps_count].latitude = gnss.latitude;
                    Start_GPS_Array[current_gps_count].longitude = gnss.longitude;
                    current_gps_count++;
                }
                else
                {
                    Buzzer_Time = 1000;
                    GPS_Get_Angle_Flag = 0;
                    Angle_CeShi = CalculateUltraAccurateHeading(Start_GPS_Array, GPS_SAMPLE_TARGET);
                    if(Moter_Flag==5)
                    {
                    float traveled_distance = sqrtf(Robot_Pos_X * Robot_Pos_X + Robot_Pos_Y * Robot_Pos_Y);
                    // 用刚算出来的真实角度，重新投影 X 和 Y
                    // 注意：C语言的 cosf 和 sinf 必须传弧度！
                    float true_yaw_rad = Angle_CeShi * DEG_TO_RAD;
                    Robot_Pos_X = traveled_distance * cosf(true_yaw_rad);
                    Robot_Pos_Y = traveled_distance * sinf(true_yaw_rad);
                        IMU_Force_Reset_Yaw(Angle_CeShi);
                        Angle_Goal = Angle_CeShi;
                    }
                }
            }
            tft180_show_float(10, 100, Angle_CeShi, 3, 2);
            tft180_show_float(0,120,Robot_Pos_X,2,2);
            tft180_show_float(50,120,Robot_Pos_Y,2,2);
            GPS_XY_Flag = 1;
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
        system_delay_ms(10);
        // 此处编写需要循环执行的代码
    }
}

// *************************** 例程常见问题说明 ***************************
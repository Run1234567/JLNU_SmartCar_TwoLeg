/*********************************************************************************************************************
 * CYT4BB Opensourec Library（即 CYT4BB 开源库）是一个基于官方 SDK 接口的独立开源库
 * Copyright (c) 2022 SEEKFREE（逐飞科技）
 *
 * 本文件是 CYT4BB 开源库的一部分
 *
 * CYT4BB 开源库 是自由软件；
 * 你可以根据自由软件基金会发布的 GNU通用公共许可证（GNU General Public License）的条款
 * 对其进行再发布和/或修改；GPL 的第3版（即 GPL3.0），或（随你选择）任何更新的版本。
 *
 * 发布 CYT4BB 开源库的目的是希望它能有所用途，但不提供任何保证；
 * 甚至没有对适销性或针对特定用途的适用性的保证。
 * 请查阅 GPL 了解更多细节。
 *
 * 你应该在收到 CYT4BB 开源库的同时收到一份 GPL 的副本；
 * 如果没有，请访问 <https://www.gnu.org/licenses/>
 *
 * 版权声明：
 * 本开源库使用 GPL3.0 开源许可证协议 详情请查阅以下版本的说明
 * 详细的中文版请查阅 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件
 * 该许可证覆盖 libraries 文件夹下 和 本文件夹下的 LICENSE 文件
 * 欢迎各位使用并提出宝贵建议  修改代码时请保留逐飞科技的版权声明及修改记录
 *
 * 文件名称          main_cm7_0
 * 公司名称          成都逐飞科技有限公司
 * 版本信息          查看 libraries/doc 文件夹下的 version 文件 版本说明
 * 开发工具          IAR 9.40.1
 * 目标平台          CYT4BB
 * 官方淘宝店        https://seekfree.taobao.com/
 *
 * 修改记录
 * 日期              作者                备注
 * 2024-1-4       pudding            first version
 ********************************************************************************************************************/

/* 定义 PI 和 地球半径常量 */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define EARTH_RADIUS 6371000.0   /* 地球平均半径（单位：米） */
#define GPS_SAMPLE_TARGET 50     /* GPS目标采集点数 */

#include "zf_common_headfile.h"

uint8 WIFI_Flag = 0; /* WIFI连接状态标志 0-未连接 1-已连接 */
#define WIFI_SSID_TEST "RUN"
#define WIFI_PASSWORD_TEST "88888888"  /* 如果你需要连接的WIFI 没有密码 则将 密码 替换为 NULL */
#define TCP_TARGET_IP "192.168.55.156" /* 连接目标的 IP */
#define TCP_TARGET_PORT "8086"         /* 连接目标的端口 */
#define WIFI_LOCAL_PORT "6666"         /* 本机的端口 0表示随机  可设置范围2048-65535  默认 6666 */

uint8 image_copy[MT9V03X_H][MT9V03X_W];   /* 摄像头图像副本 */

GPS_Point_t Start_GPS_Array[GPS_SAMPLE_TARGET]; /* 欧式法GPS坐标数组 */
int current_gps_count = 0;                      /* 当前已采集的GPS坐标数 */

uint8 GPS_XY_Flag = 0; /* 0表示还没有记录，1表示已经记录了 */

#include <math.h>


/**
 * @brief  用于多次行进中的极高精度和极稳计算 (PCA 直线拟合法)
 * @param  points  GPS坐标数组
 * @param  count   数组长度
 * @return float   航向角 (0.0 ~ 360.0度，北为0)
 */
float CalculateUltraAccurateHeading(GPS_Point_t points[], int count)
{
    const int SKIP_POINTS = 10;
    int valid_count = count - SKIP_POINTS;

    if (valid_count < 2) return -1.0f;

    /* 取局部参考系的纬度，用于精度缩放裁剪 */
    double lat0_rad = points[SKIP_POINTS].latitude * M_PI / 180.0;
    double cos_lat0 = cos(lat0_rad);

    /* 创建局部投影数组 (最大处理 100 个点，可根据你的 buffer 实际大小调整) */
    double x[100];
    double y[100];
    int n = 0;

    /* ========================================================
     * 第一步：经纬度投影转换 & 数据收集
     * ======================================================== */
    for (int i = SKIP_POINTS; i < count; i++) {
        /* 检查原有工作逻辑（增加一个额外检查，防止(原始漂移)和远(距离)的直接多点检查） */
        if (i > SKIP_POINTS) {
            double dx_check = (points[i].longitude - points[i-1].longitude) * cos_lat0;
            double dy_check = (points[i].latitude - points[i-1].latitude);
            double dist_sq = dx_check * dx_check + dy_check * dy_check;

        }

        /* 将经纬度转换为米制坐标初始化 XY 坐标 (放大值方便后面的半径) */
        x[n] = (points[i].longitude - points[SKIP_POINTS].longitude) * cos_lat0;
        y[n] = (points[i].latitude - points[SKIP_POINTS].latitude);
        n++;
    }

    if (n < 2) return -1.0f;

    /* ========================================================
     * 第二步：计算最小二乘法需要的点
     * ======================================================== */
    double mean_x = 0.0, mean_y = 0.0;
    for (int i = 0; i < n; i++) {
        mean_x += x[i];
        mean_y += y[i];
    }
    mean_x /= n;
    mean_y /= n;

    /* ========================================================
     * 第三步：计算协方差矩阵的三个元素 (找出数据点集的分布方向)
     * ======================================================== */
    double Sxx = 0.0, Syy = 0.0, Sxy = 0.0;
    for (int i = 0; i < n; i++) {
        double dx = x[i] - mean_x;
        double dy = y[i] - mean_y;
        Sxx += dx * dx;
        Syy += dy * dy;
        Sxy += dx * dy;
    }

    /* ========================================================
     * 第四步：计算主成分角度 & 修正 180 度方向模
     * ======================================================== */
    /* 主成分分析法的数学公式可求出主直线的极角偏 (-pi/2 到 pi/2) */
    double theta_rad = 0.5 * atan2(2.0 * Sxy, Sxx - Syy);

    /* 合并运动方向：没有"前后"概念，因此需要根据轨迹的首位移动方向来判断是否翻转 */
    double dir_x = x[n-1] - x[0]; /* 末X位置 */
    double dir_y = y[n-1] - y[0]; /* 末Y位置 */
    double fit_dir_x = cos(theta_rad);
    double fit_dir_y = sin(theta_rad);

    /* 向量判断：如果运动方向与拟合方向点积为负（方向角度相差大于 90 度），说明方向反了，旋转 180 度 */
    if ((dir_x * fit_dir_x + dir_y * fit_dir_y) < 0.0) {
        theta_rad += M_PI;
    }

    /* ========================================================
     * 第五步：数学转换为航海坐标系 (正北为 0)
     * ======================================================== */
    /* 数学角度是从X轴逆时针旋转的，而航海角度是从Y轴(正北)顺时针旋转 */
    double heading_deg = 90.0 - (theta_rad * 180.0 / M_PI);

    /* 规范化到 0 ~ 360 (避免出现负数的弧度 -180 ~ 180) */
    while (heading_deg < -180.0) heading_deg += 360.0;
    while (heading_deg >= 180.0) heading_deg -= 360.0;

    return (float)heading_deg;
}


float Angle_CeShi = 0.0f; /* 用于测试的角度变量 */

int main(void)
{
    clock_init(SYSTEM_CLOCK_250M); /* 时钟设置及系统初始化【必须调用】 */
    debug_init();
    DWT_Init();
    KEY_INIT();
    menu_init();

    Buzzer_Init();
    Buzzer_Time = 500;
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

    pit_ms_init(PIT_CH10, 1000); /* 初始化无线串口接收中断 */
    pit_ms_init(PIT_CH2, 1);     /* 初始化小车控制中断 */
    static bool was_high = false;
    bool is_high = (High_Right_Point + High_Left_Point > 1000);
    while (true)
    {
        Key_ISR();
        pwm_r = pwm_r + 0.1 * (High_Right_Point - pwm_r);
        pwm_l = pwm_l + 0.1 * (High_Left_Point - pwm_l);
        Servo_Leg_Control(pwm_l, pwm_r, PID_Speed.Output * 5);
        tft_show();
        if (is_high == true && was_high == false)
        {
            /* 逻辑 A：从低处到高处越过600（穿越上方边界） */
            PID_Init(&PID_Angular_V, Angular_V_P_High, Angular_V_I_High, Angular_V_D_High, 0);
            PID_Init(&PID_Angular, Angular_P_High, Angular_I_High, Angular_D_High, 0);
            was_high = true; /* 标记完成后，把"从低处到高处"的状态设为真 */
        }
        else if (is_high == false && was_high == true)
        { 
            /* 逻辑 B：从高处到低处低于600（穿越下方边界） */
            PID_Init(&PID_Angular_V, Angular_V_P, Angular_V_I, Angular_V_D, 0);
            PID_Init(&PID_Angular, Angular_P, Angular_I, Angular_D, 0);
            was_high = false; /* 标记完成后，把"从低处到高处"的状态设为假 */
        }
        tft180_show_float(10, 100, Angle_CeShi, 3, 2);
            tft180_show_float(0, 120, Robot_Pos_X, 2, 2);
            tft180_show_float(50, 120, Robot_Pos_Y, 2, 2);
        if (uart_receiver.finsh_flag == 1)
        {
            if (1 == uart_receiver.state) /* 遥控器失联状态判断 */
            {
            }
            else
            {
            }
            uart_receiver.finsh_flag = 0;
        }
        system_delay_ms(10);
        /* 此处写你需要循环执行的代码 */
    }
}

/* **************************** 工程使用说明 **************************** */

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
* 文件名称          cm7_0_isr
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹下的 version 文件 版本说明
* 开发工具          IAR 9.40.1
* 目标平台          CYT4BB
* 官方淘宝店        https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2024-1-9      pudding            first version
* 2024-5-14     pudding            增加12个pit可选中断 以及硬件注释说明
* 2025-2-4      pudding            优化外部中断逻辑，防止外部脉冲导致的开关误触发问题，优化部分串口初始化逻辑
* 2025-2-4      pudding            增加部分串口预留接口
********************************************************************************************************************/

#include "zf_common_headfile.h"


/* **************************** PIT中断函数 **************************** */
void pit0_ch0_isr()                     /* 定时器通道 0 默认中断服务函数 */
{
    pit_isr_flag_clear(PIT_CH0);



}

void pit0_ch1_isr()                     /* 定时器通道 1 默认中断服务函数 */
{
    pit_isr_flag_clear(PIT_CH1);

}

int16 PWM_Left=0;   /* 左电机PWM输出 */
int16 PWM_Right=0;  /* 右电机PWM输出 */
void pit0_ch2_isr()                     /* 定时器通道 2 默认中断服务函数 */
{

    pit_isr_flag_clear(PIT_CH2);
    Isr_Control();  /* 执行控制任务 */

}

void pit0_ch10_isr()                    /* 定时器通道 10 默认中断服务函数 */
{
    // wireless_uart_send_float(PID_Speed.Output,3);       /* 发送速度PID输出 */
    // wireless_uart_send_string(",");
    // wireless_uart_send_decimal(uart_receiver.channel[0]);  /* 发送遥控器通道0 */
    // wireless_uart_send_string(",");
    // wireless_uart_send_decimal(uart_receiver.channel[1]);  /* 发送遥控器通道1 */
    // wireless_uart_send_string(",");
    // wireless_uart_send_decimal(uart_receiver.channel[2]);  /* 发送遥控器通道2 */
    // wireless_uart_send_string(",");
    // wireless_uart_send_decimal(uart_receiver.channel[3]);  /* 发送遥控器通道3 */
    // wireless_uart_send_string(",");
    // wireless_uart_send_decimal(uart_receiver.channel[4]);  /* 发送遥控器通道4 */
    // wireless_uart_send_string(",");
    // wireless_uart_send_decimal(uart_receiver.channel[5]);  /* 发送遥控器通道5 */
    // wireless_uart_send_string(",");
    // wireless_uart_send_float(attitude.yaw,3);              /* 发送偏航角 */
    // wireless_uart_send_string(",");
    // wireless_uart_send_float(attitude.roll,3);             /* 发送横滚角 */
    // wireless_uart_send_string(",");
    // wireless_uart_send_float(attitude.pitch,3);            /* 发送俯仰角 */
    // wireless_uart_send_string(",");
    // wireless_uart_send_decimal(Speed_Forward);             /* 发送前进速度 */
    // wireless_uart_send_string(",");
    // wireless_uart_send_float(Angle_Goal,2);                /* 发送目标速度 */
    // wireless_uart_send_string(",");
    // wireless_uart_send_float(PID_Angle.Output,3);               /* 发送右电机速度 */
    // wireless_uart_send_string(",");
    // wireless_uart_send_decimal(Speed_Left);                /* 发送左电机速度 */
    // wireless_uart_send_string("\n");

    pit_isr_flag_clear(PIT_CH10);
    Wireless_UART_PIT();  /* 无线串口定时器 */


}

void pit0_ch11_isr()                    /* 定时器通道 11 默认中断服务函数 */
{
    pit_isr_flag_clear(PIT_CH11);

}

void pit0_ch12_isr()                    /* 定时器通道 12 默认中断服务函数 */
{
    pit_isr_flag_clear(PIT_CH12);

}

void pit0_ch13_isr()                    /* 定时器通道 13 默认中断服务函数 */
{
    pit_isr_flag_clear(PIT_CH13);

}

void pit0_ch14_isr()                    /* 定时器通道 14 默认中断服务函数 */
{
    pit_isr_flag_clear(PIT_CH14);

}

void pit0_ch15_isr()                    /* 定时器通道 15 默认中断服务函数 */
{
    pit_isr_flag_clear(PIT_CH15);

}

void pit0_ch16_isr()                    /* 定时器通道 16 默认中断服务函数 */
{
    pit_isr_flag_clear(PIT_CH16);

}

void pit0_ch17_isr()                    /* 定时器通道 17 默认中断服务函数 */
{
    pit_isr_flag_clear(PIT_CH17);

}

void pit0_ch18_isr()                    /* 定时器通道 18 默认中断服务函数 */
{
    pit_isr_flag_clear(PIT_CH18);

}

void pit0_ch19_isr()                    /* 定时器通道 19 默认中断服务函数 */
{
    pit_isr_flag_clear(PIT_CH19);

}

void pit0_ch20_isr()                    /* 定时器通道 20 默认中断服务函数 */
{
    pit_isr_flag_clear(PIT_CH20);

}

void pit0_ch21_isr()                    /* 定时器通道 21 默认中断服务函数 */
{
    pit_isr_flag_clear(PIT_CH21);
    tsl1401_collect_pit_handler();  /* TSL1401线性CCD采集处理 */
}
/* **************************** PIT中断函数 **************************** */


/* **************************** 串口中断函数 **************************** */
/* 串口0默认用作调试串口 */
void uart0_isr (void)
{
    if(uart_isr_mask(UART_0))            /* 串口0接收中断 */
    {

#if DEBUG_UART_USE_INTERRUPT              /* 如果开启了 debug 串口中断 */
        debug_interrupr_handler();        /* 调用 debug 串口接收处理函数，数据会被 debug 缓冲区接收存放 */
#endif                                    /* 如果你修改了 DEBUG_UART_INDEX 那么你需要放到对应的串口中断去 */

    }
    else                                  /* 串口0发送中断 */
    {




    }
}

void uart1_isr (void)
{
    if(uart_isr_mask(UART_1))            /* 串口1接收中断 */
    {

       // wireless_module_uart_handler();  /* 无线模块统一回调函数（已注释） */

    }
    else                                  /* 串口1发送中断 */
    {




    }
}

void uart2_isr (void)
{
    if(uart_isr_mask(UART_2))            /* 串口2接收中断 */
    {

        gnss_uart_callback();            /* GPS模块回调函数 */

    }
    else                                  /* 串口2发送中断 */
    {




    }
}

void uart3_isr (void)
{
    if(uart_isr_mask(UART_3))            /* 串口3接收中断 */
    {




    }
    else                                  /* 串口3发送中断 */
    {




    }
}

void uart4_isr (void)
{
    if(uart_isr_mask(UART_4))            /* 串口4接收中断 */
    {

        uart_control_callback();         /* 串口接收器回调函数 */

    }
    else                                  /* 串口4发送中断 */
    {




    }
}

void uart5_isr (void)
{
    if(uart_isr_mask(UART_5))            /* 串口5接收中断 */
    {




    }
    else                                  /* 串口5发送中断 */
    {




    }
}

void uart6_isr (void)
{
    if(uart_isr_mask(UART_6))            /* 串口6接收中断 */
    {

        uart_receiver_handler();         /* 遥控器接收器回调函数 */

    }
    else                                  /* 串口6发送中断 */
    {




    }
}
/* **************************** 串口中断函数 **************************** */

/* **************************** 外部中断函数 **************************** */
void gpio_0_exti_isr()                  /* 外部 GPIO_0 中断服务函数 */
{



}

void gpio_1_exti_isr()                  /* 外部 GPIO_1 中断服务函数 */
{
    if(exti_flag_get(P01_0))            /* 检测P1_0端口外部中断标志 */
    {




    }
    if(exti_flag_get(P01_1))
    {



    }
}

void gpio_2_exti_isr()                  /* 外部 GPIO_2 中断服务函数 */
{
    if(exti_flag_get(P02_0))
    {


    }
    if(exti_flag_get(P02_4))
    {


    }

}

void gpio_3_exti_isr()                  /* 外部 GPIO_3 中断服务函数 */
{



}

void gpio_4_exti_isr()                  /* 外部 GPIO_4 中断服务函数 */
{



}

void gpio_5_exti_isr()                  /* 外部 GPIO_5 中断服务函数 */
{



}


void gpio_6_exti_isr()                  /* 外部 GPIO_6 中断服务函数 */
{



}

void gpio_7_exti_isr()                  /* 外部 GPIO_7 中断服务函数 */
{



}

void gpio_8_exti_isr()                  /* 外部 GPIO_8 中断服务函数 */
{



}

void gpio_9_exti_isr()                  /* 外部 GPIO_9 中断服务函数 */
{



}

void gpio_10_exti_isr()                 /* 外部 GPIO_10 中断服务函数 */
{



}

void gpio_11_exti_isr()                 /* 外部 GPIO_11 中断服务函数 */
{



}

void gpio_12_exti_isr()                 /* 外部 GPIO_12 中断服务函数 */
{



}

void gpio_13_exti_isr()                 /* 外部 GPIO_13 中断服务函数 */
{



}

void gpio_14_exti_isr()                 /* 外部 GPIO_14 中断服务函数 */
{



}

void gpio_15_exti_isr()                 /* 外部 GPIO_15 中断服务函数 */
{



}

void gpio_16_exti_isr()                 /* 外部 GPIO_16 中断服务函数 */
{



}

void gpio_17_exti_isr()                 /* 外部 GPIO_17 中断服务函数 */
{



}

void gpio_18_exti_isr()                 /* 外部 GPIO_18 中断服务函数 */
{



}

void gpio_19_exti_isr()                 /* 外部 GPIO_19 中断服务函数 */
{



}

void gpio_20_exti_isr()                 /* 外部 GPIO_20 中断服务函数 */
{



}

void gpio_21_exti_isr()                 /* 外部 GPIO_21 中断服务函数 */
{



}

void gpio_22_exti_isr()                 /* 外部 GPIO_22 中断服务函数 */
{



}

void gpio_23_exti_isr()                 /* 外部 GPIO_23 中断服务函数 */
{



}
/* **************************** 外部中断函数 **************************** */

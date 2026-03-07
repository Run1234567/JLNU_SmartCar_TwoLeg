/******************************************************************************
 * 文件名: control.c (示例)
 * 功能描述: 机器人运动控制系统，包含角度、角速度、速度的三环PID控制
 * 硬件依赖: 正点原子开发板、IMU660RA传感器、电机驱动器
 * 控制结构: 速度环 -> 角度环 -> 角速度环（串级PID控制）
 * 作者: 根据zf_common_headfile.h推断
 * 版本: 1.0
 * 创建时间: 未知
 ******************************************************************************/

#include "zf_common_headfile.h"  // 正点原子通用头文件

/* 全局变量定义 */
PIDController PID_Angular_V;      // 角速度环PID控制器（内环）
PIDController PID_Angular;        // 角度环PID控制器（中环）
PIDController PID_Speed;          // 速度环PID控制器（外环）
PIDController PID_Angle;
PIDController PID_High;

int TimerTime = 0;                // 定时器计时变量，用于任务调度
int16 Speed_Left = 0;             // 左轮实际速度（编码器读取值）
int16 Speed_Right = 0;            // 右轮实际速度（编码器读取值）
int16 Speed_Forward = 0;          // 整车平均速度（左右轮平均值）
int16 Speed_Goal = 0;             // 速度环目标值（0）
float Angle_Goal=0.00f;

int Speed_Sum=0;

float Robot_Pos_X = 0.0f;   // 世界坐标系 X (米)
float Robot_Pos_Y = 0.0f;   // 世界坐标系 Y (米)

int8 Moter_Flag=0;

/******************************************************************************
 * 函数名: PWM_SET
 * 功能描述: 设置左右电机的PWM输出值，并进行限幅保护
 * 参数说明:
 *   - PWM_L: 左电机PWM值，范围[-10000, 10000]，负值表示反转
 *   - PWM_R: 右电机PWM值，范围[-10000, 10000]，负值表示反转
 * 返回值: 无
 * 备注: 调用电机驱动函数前进行限幅，防止过大的PWM损坏硬件
 ******************************************************************************/
void PWM_SET(int16 PWM_L, int16 PWM_R)
{
    /* 左电机PWM限幅 */
    if(PWM_L > 9999) PWM_L = 10000;    // 上限限制
    if(PWM_L < -9999) PWM_L = -10000;  // 下限限制
    
    /* 右电机PWM限幅 */
    if(PWM_R > 9999) PWM_R = 10000;    // 上限限制
    if(PWM_R < -9999) PWM_R = -10000;  // 下限限制
    
    /* 设置电机驱动器的PWM占空比 */
    small_driver_set_duty(PWM_L, PWM_R);
}

/******************************************************************************
 * 函数名: PID_Init_Angular_V
 * 功能描述: 初始化角速度环PID控制器参数
 * 参数说明: 无
 * 返回值: 无
 * 备注: 使用外部定义的Angular_V_P, Angular_V_I, Angular_V_D参数
 ******************************************************************************/
void PID_Init_Angular_V()
{
    PID_Init(&PID_Angular_V, Angular_V_P, Angular_V_I, Angular_V_D, 0);
}

/******************************************************************************
 * 函数名: PID_Init_Angular
 * 功能描述: 初始化角度环PID控制器参数
 * 参数说明: 无
 * 返回值: 无
 * 备注: 使用外部定义的Angular_P, Angular_I, Angular_D参数
 ******************************************************************************/
void PID_Init_Angular()
{
    PID_Init(&PID_Angular, Angular_P, Angular_I, Angular_D, 0);
}

/******************************************************************************
 * 函数名: PID_Init_Speed
 * 功能描述: 初始化速度环PID控制器参数
 * 参数说明: 无
 * 返回值: 无
 * 备注: 使用外部定义的Speed_P, Speed_I, Speed_D参数
 ******************************************************************************/
void PID_Init_Speed()
{
    PID_Init(&PID_Speed, Speed_P, Speed_I, Speed_D, 0);
}
void PID_Angle_Init()
{
    PID_Init(&PID_Angle,Angle_PID_P,Angle_PID_I,Angle_PID_D,0);
}

void PID_High_Init()
{
    PID_Init(&PID_High,High_P,High_I,High_D,0);
}
/******************************************************************************
 * 函数名: Angular_V_Calculate
 * 功能描述: 执行角速度环PID计算，控制机器人角速度稳定
 * 控制逻辑:
 *   - 反馈值: IMU测量的当前角速度（imu660ra_gyro_x）
 *   - 设定值: 角度环PID的输出（PID_Angular.Output）
 *   - 输出: 用于控制电机差速的PWM值
 * 参数说明: 无
 * 返回值: 无
 ******************************************************************************/
void Angular_V_Calculate()
{
    PID_Calculate(&PID_Angular_V, imu660ra_gyro_x, PID_Angular.Output);
}

/******************************************************************************
 * 函数名: Angular_Calculate
 * 功能描述: 执行角度环PID计算，控制机器人倾斜角度
 * 控制逻辑:
 *   - 反馈值: 当前姿态角的横滚角（attitude.roll）
 *   - 设定值: 固定平衡点(-5.1度) + 速度环的输出（PID_Speed.Output）
 *   - 输出: 作为角速度环的设定
 * 参数说明: 无
 * 返回值: 无
 * 备注: -5.1度可能是机器人的静态平衡点偏移
 ******************************************************************************/
void Angular_Calculate()
{
    PID_Calculate(&PID_Angular, attitude.roll, -6.4);
}

/******************************************************************************
 * 函数名: Speed_Calculate
 * 功能描述: 执行速度环PID计算，控制机器人整体运动速度
 * 控制逻辑:
 *   - 反馈值: 左右轮编码器速度的平均值
 *   - 设定值: 0（速度环目标为0，维持静态平衡）
 *   - 输出: 作为角度环设定值的调整量，用于补偿速度偏差
 * 参数说明: 无
 * 返回值: 无
 * 备注: 右轮速度取负是因为电机安装方向或编码器方向定义不同
 ******************************************************************************/
void Speed_Calculate()
{
    /* 读取左右轮编码器速度 */
    Speed_Left = motor_value.receive_left_speed_data;
    Speed_Right = -motor_value.receive_right_speed_data;  // 右轮速度取反
    Speed_Sum+=Speed_Left;
    /* 计算整车平均速度 */
    Speed_Forward = (Speed_Left + Speed_Right) / 2;
    float distance_step = Speed_Forward * PULSE_TO_METER;
    // 4. 获取当前航向角并转为弧度 
    float yaw_rad = attitude.yaw * DEG_TO_RAD;
    
    // 5. 分解到位移坐标系并累加 (Dead Reckoning)
    Robot_Pos_X += distance_step * cosf(yaw_rad);
    Robot_Pos_Y += distance_step * sinf(yaw_rad);
    /* 执行PID计算，速度环目标为0（保持静止） */
    PID_Calculate(&PID_Speed, Speed_Forward, Speed_Goal);
        if(PID_Speed.Output>=10)PID_Speed.Output=10;
    if(PID_Speed.Output<=-10)PID_Speed.Output=-10;
}

void Angle_Differential_Control()
{
    PID_Calculate_Angle(&PID_Angle,attitude.yaw,Angle_Goal);
}
void PID_Init_All()
{
    PID_Init_Angular_V();  
    PID_Init_Angular();
    PID_Init_Speed();
    PID_Angle_Init();
    PID_High_Init();
}
void High_Calculate()
{
    PID_Calculate(&PID_High,attitude.pitch,-5.8);
}

/**
 * @brief  计算从当前位置到目标位置的绝对目标航向角
 * * @param  current_x  当前位置的 X 坐标 (米)
 * @param  current_y  当前位置的 Y 坐标 (米)
 * @param  target_x   目标位置的 X 坐标 (米)
 * @param  target_y   目标位置的 Y 坐标 (米)
 * * @return float      目标航向角 (单位: 度, 范围: -180.0 到 +180.0)
 */
float Calculate_Target_Angle(float current_x, float current_y, float target_x, float target_y)
{
    // 1. 计算目标点相对于当前点的坐标差值
    float dx = target_x - current_x;
    float dy = target_y - current_y;
    
    // 2. 使用 atan2f 计算绝对弧度角
    // 注意：不要用 atanf，因为 atan2f 会自动帮你判断处于哪一个象限
    float target_angle_rad = atan2f(dy, dx);
    
    // 3. 将弧度转换为度数并返回
    return target_angle_rad * RAD_TO_DEG;
}

/******************************************************************************
 * 函数名: Isr_Control
 * 功能描述: 控制中断服务函数，按不同周期执行控制任务
 * 任务调度:
 *   - 每10ms执行的任务（当前未实现，可扩展）
 *   - 每5ms执行的任务: 读取IMU数据、更新姿态、计算角度环
 *   - 每次中断执行的任务: 读取角速度、计算角速度环、输出PWM
 * 参数说明: 无
 * 返回值: 无
 * 中断频率: 1kHz（每1ms执行一次）
 ******************************************************************************/
float Target_Points[6][2] = {
    {0.0f, 0.0f},
    {1.0f, 1.0f},
    {2.0f, 0.0f},
    {3.0f, 1.0f},
    {4.0f, 0.0f},
    {0.0f, 0.0f}
};
uint8_t Target_Index = 0;   // 当前追踪的目标点序号 (0~3)
void Isr_Control()
{
    TimerTime++;  // 中断次数计数
    if(Moter_Flag==1)
    {
    // 假设你要去 (1.0, 1.0) 这个点
    /* 10ms周期任务（预留扩展） */
    if(TimerTime % 10 == 0)
    {if(Target_Index>=6)
        {
        Speed_Goal=0; 
        // Angle_Goal=0;
         Speed_Calculate();  // 每10ms执行一次速度环PID计算
        }
        else
        {
            Speed_Goal=150;
        // 可添加10ms周期的任务，如速度环计算
    Speed_Calculate();  // 每10ms执行一次速度环PID计算
    float target_x = Target_Points[Target_Index][0];
    float target_y = Target_Points[Target_Index][1];
    Angle_Goal = Calculate_Target_Angle(Robot_Pos_X, Robot_Pos_Y, target_x, target_y);
    // 2. 计算当前位置到目标点的 X、Y 偏差
                float dx = target_x - Robot_Pos_X;
                float dy = target_y - Robot_Pos_Y;
        float distance = sqrtf(dx * dx + dy * dy);
        if(distance < 0.05f)
        {
            Target_Index++;
        }
    }
    }
    
    /* 5ms周期任务（200Hz控制频率）*/
    if(TimerTime % 5 == 0)
    {
        IMU660_GetData();      // 读取IMU660RA传感器原始数据
        updateAttitude();      // 更新姿态角（四元数解算或互补滤波）
        
        Angle_Differential_Control();
        Angular_Calculate();   // 计算角度环PID
    }
    
    /* 每次中断都执行的任务（1kHz控制频率）*/
    imu660ra_get_gyro();       // 快速读取角速度数据
    Angular_V_Calculate();     // 计算角速度环PID（最高频率环）
    
    /* PWM输出控制（差速转向控制）*/
    // PWM_SET(0,0);  // 调试时可屏蔽电机输出
    PWM_SET(-(int16)(PID_Angular_V.Output*(1-PID_Angle.Output)), -(int16)(PID_Angular_V.Output*(1+PID_Angle.Output)));  // 差速转向：左右轮反向
    }
//************************************************************************************************************************************************* */
    else if(Moter_Flag==0)
    {
    PWM_SET(0,0);
    if(TimerTime % 10 == 0)
    Speed_Calculate();
    if(TimerTime % 5 == 0)
    {
        IMU660_GetData();      // 读取IMU660RA传感器原始数据
        updateAttitude();      // 更新姿态角（四元数解算或互补滤波）
    }
    }

//************************************************************************************************************************************************** */
    else if(Moter_Flag==2)
    {
    Speed_Goal=60;
    /* 10ms周期任务（预留扩展） */
    if(TimerTime % 10 == 0)
    {
        // 可添加10ms周期的任务，如速度环计算
        Speed_Calculate();  // 每10ms执行一次速度环PID计算
    }
    
    /* 5ms周期任务（200Hz控制频率）*/
    if(TimerTime % 5 == 0)
    {
        IMU660_GetData();      // 读取IMU660RA传感器原始数据
        updateAttitude();      // 更新姿态角（四元数解算或互补滤波）
        
        Angle_Differential_Control();
        Angular_Calculate();   // 计算角度环PID
        High_Calculate();
    }
    /* 每次中断都执行的任务（1kHz控制频率）*/
    imu660ra_get_gyro();       // 快速读取角速度数据
    Angular_V_Calculate();     // 计算角速度环PID（最高频率环）
    if(PID_Angular_V.Output>=4000)PID_Angular_V.Output=0;
    PWM_SET(-(int)PID_Angular_V.Output,-(int)PID_Angular_V.Output);  
    }


//************************************************************************************************************************************************************** */

    else if(Moter_Flag==3)
    {
    if(TimerTime>= 5000)
    {
        Speed_Goal=100;
        if(TimerTime%10000>=5000)
        {
        Angle_Goal=0;
        }
        else
        {
        Angle_Goal=180;
        }
        if(TimerTime%5000==0)
        {
            PID_Speed.ErrorSum=0;
        }
    }
    else
    {
        Speed_Goal=0;
    }
    
    /* 10ms周期任务（预留扩展） */
    if(TimerTime % 10 == 0)
    {
        // 可添加10ms周期的任务，如速度环计算
        Speed_Calculate();  // 每10ms执行一次速度环PID计算
    }
    
    /* 5ms周期任务（200Hz控制频率）*/
    if(TimerTime % 5 == 0)
    {
        IMU660_GetData();      // 读取IMU660RA传感器原始数据
        updateAttitude();      // 更新姿态角（四元数解算或互补滤波）
        
        Angle_Differential_Control();
        Angular_Calculate();   // 计算角度环PID
    }
    
    /* 每次中断都执行的任务（1kHz控制频率）*/
    imu660ra_get_gyro();       // 快速读取角速度数据
    Angular_V_Calculate();     // 计算角速度环PID（最高频率环）
    
    /* PWM输出控制（差速转向控制）*/
    // PWM_SET(0,0);  // 调试时可屏蔽电机输出
    PWM_SET(-(int16)(PID_Angular_V.Output*(1-PID_Angle.Output)), (int16)(PID_Angular_V.Output*(1+PID_Angle.Output)));  // 差速转向：左右轮反向
    }


//****************************************************************************************************************************************************** */
    else if(Moter_Flag==4)
    {
    int8 dir=1;
    if(TimerTime>= 5000)
    {
        Angle_Goal=0;
        if(TimerTime%10000>=5000)
        {
        dir=1;
        Speed_Goal=100;
        }
        else
        {
        dir=-1;
        Speed_Goal=-100;
        }
    }
    else
    {
        Speed_Goal=0;
    }
    
    /* 10ms周期任务（预留扩展） */
    if(TimerTime % 10 == 0)
    {
        // 可添加10ms周期的任务，如速度环计算
        Speed_Calculate();  // 每10ms执行一次速度环PID计算
    }
    
    /* 5ms周期任务（200Hz控制频率）*/
    if(TimerTime % 5 == 0)
    {
        IMU660_GetData();      // 读取IMU660RA传感器原始数据
        updateAttitude();      // 更新姿态角（四元数解算或互补滤波）
        
        Angle_Differential_Control();
        Angular_Calculate();   // 计算角度环PID
    }
    
    /* 每次中断都执行的任务（1kHz控制频率）*/
    imu660ra_get_gyro();       // 快速读取角速度数据
    Angular_V_Calculate();     // 计算角速度环PID（最高频率环）
    
    /* PWM输出控制（差速转向控制）*/
    // PWM_SET(0,0);  // 调试时可屏蔽电机输出
    PWM_SET(-(int16)(PID_Angular_V.Output*(1-PID_Angle.Output*dir)), -(int16)(PID_Angular_V.Output*(1+PID_Angle.Output*dir)));  // 差速转向：左右轮反向
    }
//******************************************************************************************************************************* */
    else if(Moter_Flag==5)
    {
    if(TimerTime>= 10000)
    {
        if(Speed_Sum<=50000)
        Speed_Goal=100;
        else
        Speed_Goal=0;
    }
    else
    {
        Speed_Goal=0;
    }
    /* 10ms周期任务（预留扩展） */
    if(TimerTime % 10 == 0)
    {
        // 可添加10ms周期的任务，如速度环计算
        Speed_Calculate();  // 每10ms执行一次速度环PID计算
        Speed_Sum+=Speed_Forward;
    }
    
    /* 5ms周期任务（200Hz控制频率）*/
    if(TimerTime % 5 == 0)
    {
        IMU660_GetData();      // 读取IMU660RA传感器原始数据
        updateAttitude();      // 更新姿态角（四元数解算或互补滤波）
        
        Angle_Differential_Control();
        Angular_Calculate();   // 计算角度环PID
        High_Calculate();
        High_Right_Point=400+(int)PID_High.Output;
        High_Left_Point=400-(int)PID_High.Output;
    }
    
    /* 每次中断都执行的任务（1kHz控制频率）*/
    imu660ra_get_gyro();       // 快速读取角速度数据
    Angular_V_Calculate();     // 计算角速度环PID（最高频率环）
    
    /* PWM输出控制（差速转向控制）*/
    // PWM_SET(0,0);  // 调试时可屏蔽电机输出
    PWM_SET(-(int16)(PID_Angular_V.Output*(1-PID_Angle.Output)), -(int16)(PID_Angular_V.Output*(1+PID_Angle.Output)));  // 差速转向：左右轮反向
    }

}

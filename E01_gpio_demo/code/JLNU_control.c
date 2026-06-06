/******************************************************************************
 * 文件名: control.c
 * 功能描述: 双足自平衡小车运动控制系统,实现角度、角速度、速度的三环PID控制
 * 硬件平台: 逐飞创意控制板、IMU660RA、无刷电机驱动
 * 控制结构: 速度环 -> 角度环 -> 角速度环(三环PID控制)
 * 依赖库: 逐飞zf_common_headfile.h驱动
 * 版本: 1.0
 * 创建时间: 未知
 ******************************************************************************/

#include "zf_common_headfile.h" // 逐飞通用头文件

/* 全局变量定义 */
PIDController PID_Angular_V; // 角速度环PID控制器(最内环)
PIDController PID_Angular;   // 角度环PID控制器(中间环)
PIDController PID_Speed;     // 速度环PID控制器(最外环)
PIDController PID_Angle;     // 转向角度PID控制器
PIDController PID_High;      // 高度PID控制器

int TimerTime = 0;       // 中断计时器,每次中断自增1
int16 Speed_Left = 0;    // 左轮实际速度(编码器读取)
int16 Speed_Right = 0;   // 右轮实际速度(编码器读取)
int16 Speed_Forward = 0; // 左右轮平均速度(取平均值)
int16 Speed_Goal = 0;    // 速度环目标值(默认0)
float Angle_Goal = 0.00f;
float Mechanical_Zero_Point = 0.00f; // 机械零点(静态平衡角度)
int Speed_Sum = 0;       // 速度累加值

float Robot_Pos_X = 0.0f; // 机器人坐标系 X (米)
float Robot_Pos_Y = 0.0f; // 机器人坐标系 Y (米)

/* ==================== GPS 坐标数据存储区 ==================== */
/* 注意:经纬度因小数点后有很多位,故用 double 而非 float 避免丢失精度 */

GPS_Point_t Route_Points[100];       // GPS路线点数组(最多100个点)
Local_Point_t XY_Points[100];        // 转换后的平面坐标点数组
uint8 current_point_count = 0;       // 当前已经采集了多少个点
float Yaw_Offset = 0;                // 航向角偏移量

Local_Point_t XY_Points_used[100];   // 转换后的平面坐标点数组(实际使用)
uint8 current_point_count_used = 0;  // 当前已经采集了多少个点

IMU_Point_t IMU_Points[100];         // IMU坐标点采集数组
IMU_Point_t IMU_Points_used[100];    // IMU坐标点使用数组

uint8 current_IMU_point_count = 0;      // 当前已经采集的IMU点数
uint8 current_IMU_point_count_used = 0; // 当前已经使用的IMU点数

uint8 current_IMU_point_count_KM2 = 0;      // KM2模式已采集IMU点数
uint8 current_IMU_point_count_used_KM2 = 0; // KM2模式已使用IMU点数
IMU_Point_t IMU_Points_used_KM2[100];       // KM2模式IMU坐标使用数组
IMU_Point_t IMU_Points_KM2[100];            // KM2模式IMU坐标采集数组

uint8 current_IMU_GPS_Num = 0;      // 已采集的GPS+IMU融合点数
uint8 current_IMU_GPS_Num_Used = 0; // 已使用的GPS+IMU融合点数
IMU_Point_t IMU_GPS[100];           // GPS+IMU融合坐标采集数组
IMU_Point_t IMU_GPS_Used[100];      // GPS+IMU融合坐标使用数组

float GPS_X_Now = 0.0f;             // 当前GPS X坐标(米)
float GPS_Y_Now = 0.0f;             // 当前GPS Y坐标(米)

GPS_Point_t Reference_GPS;        // 记录第一次进入模式时的GPS参考点
uint8 Mode7_First_Enter_Flag = 1; // 1:表示还没记录过, 0:表示已经记录过了
uint8 Mode6_First_Enter_Flag = 1; // 1:表示还没记录过, 0:表示已经记录过了
uint8 Mode5_First_Enter_Flag = 1; // 1:表示还没记录过, 0:表示已经记录过了
uint8 Mode4_First_Enter_Flag = 1; // 1:表示还没记录过, 0:表示已经记录过了
uint8 Mode3_First_Enter_Flag = 1; // 1:表示还没记录过, 0:表示已经记录过了
uint8 Mode2_First_Enter_Flag = 1; // 1:表示还没记录过, 0:表示已经记录过了
uint8 Mode1_First_Enter_Flag = 1; // 1:表示还没记录过, 0:表示已经记录过了
uint8 Mode0_First_Enter_Flag = 1; // 1:表示还没记录过, 0:表示已经记录过了
uint8 GPS_Get_Angle_Flag = 0;     // 为1时开始采集GPS坐标并计算航向角


int8_t KM2_Turn_Flag = 0; // KM2原地转向标志 (0:不转向, 1:转向中)
int32_t KM2_Turn_Out = 0; // KM2转向累计陀螺仪积分值
/* ============================================================== */

/* 遥控器通道状态变量 */
int8_t SWA_state = 0;    // 遥控器SWA开关状态
int8_t SWB_state = 0;    // 遥控器SWB开关状态
int8_t CH5_state = 0;    // 遥控器CH5通道状态
int8_t CH6_state = 0;    // 遥控器CH6通道状态

int8 Moter_Flag = 0;     // 电机使能标志位

/* GPS+IMU融合坐标 */
float Fused_X = 0.0f;    // 融合后X坐标
float Fused_Y = 0.0f;    // 融合后Y坐标
float GPS_Weight = 0.02f; // GPS融合权重(范围 0.01 ~ 0.2),可调节

int16 Buzzer_Time = 0;   // 蜂鸣器持续时间

int16_t YaoKong_Test_Mode = 0; // 遥控测试模式标志

/******************************************************************************
 * 函数名: PWM_SET
 * 功能描述: 设置左右电机的PWM值,带限幅保护
 * 参数说明:
 *   - PWM_L: 左电机PWM值,范围[-10000, 10000],正值表示正转
 *   - PWM_R: 右电机PWM值,范围[-10000, 10000],正值表示正转
 * 返回值: 无
 * 备注: 设置到电机驱动前进行限幅,防止过大的PWM损坏硬件
 ******************************************************************************/
void PWM_SET(int16 PWM_L, int16 PWM_R)
{
    /* 左电机PWM限幅 */
    if (PWM_L > 9999)
        PWM_L = 10000; // 正向最大值
    if (PWM_L < -9999)
        PWM_L = -10000; // 反向最大值

    /* 右电机PWM限幅 */
    if (PWM_R > 9999)
        PWM_R = 10000; // 正向最大值
    if (PWM_R < -9999)
        PWM_R = -10000; // 反向最大值

    /* 调用电机驱动设置PWM占空比 */
    small_driver_set_duty(PWM_L, PWM_R);
}

/******************************************************************************
 * 函数名: PID_Init_Angular_V
 * 功能描述: 初始化角速度环PID控制器参数
 * 参数说明: 无
 * 返回值: 无
 * 备注: 使用外部宏定义 Angular_V_P, Angular_V_I, Angular_V_D 初始化
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
 * 备注: 使用外部宏定义 Angular_P, Angular_I, Angular_D 初始化
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
 * 备注: 使用外部宏定义 Speed_P, Speed_I, Speed_D 初始化
 ******************************************************************************/
void PID_Init_Speed()
{
    PID_Init(&PID_Speed, Speed_P, Speed_I, Speed_D, 0);
}

/* 初始化转向角度PID控制器 */
void PID_Angle_Init()
{
    PID_Init(&PID_Angle, Angle_PID_P, Angle_PID_I, Angle_PID_D, 0);
}

/* 初始化高度PID控制器 */
void PID_High_Init()
{
    PID_Init(&PID_High, High_P, High_I, High_D, 0);
}

/******************************************************************************
 * 函数名: Angular_V_Calculate
 * 功能描述: 执行角速度环PID计算,控制机体角速度稳定
 * 计算逻辑:
 *   - 输入值: IMU测得的当前角速度(imu660rc_gyro_y)
 *   - 设定值: 角度环PID的输出(PID_Angular.Output)
 *   - 输出: 用于控制电机的PWM值
 * 参数说明: 无
 * 返回值: 无
 ******************************************************************************/
void Angular_V_Calculate()
{
    PID_Calculate(&PID_Angular_V, -imu660rc_gyro_y, PID_Angular.Output);
}

/******************************************************************************
 * 函数名: Angular_Calculate
 * 功能描述: 执行角度环PID计算,控制机体俯仰倾斜角度
 * 计算逻辑:
 *   - 输入值: 当前姿态角的俯仰角(attitude.pitch)
 *   - 设定值: 固定平衡角度(-5.1度) + 速度环输出(动态补偿)
 *   - 输出: 作为角速度环的设定输入
 * 参数说明: 无
 * 返回值: 无
 * 备注: -5.1度可能是机身静态平衡的偏移量
 ******************************************************************************/
void Angular_Calculate()
{
    PID_Calculate(&PID_Angular, attitude.pitch, Mechanical_Zero_Point);
}

/******************************************************************************
 * 函数名: Speed_Calculate
 * 功能描述: 执行速度环PID计算,控制机体前进运动速度
 * 计算逻辑:
 *   - 输入值: 左右两轮编码器速度的平均值
 *   - 设定值: 0(速度环目标为0,维持静态平衡)
 *   - 输出: 作为角度环设定值的叠加量,用于补偿速度偏差
 * 参数说明: 无
 * 返回值: 无
 * 备注: 左右速度取反,因为电机安装方向不同导致数值正负相反
 ******************************************************************************/
void Speed_Calculate()
{
    /* 获取左右轮编码器速度 */
    Speed_Left = motor_value.receive_left_speed_data;
    Speed_Right = -motor_value.receive_right_speed_data; // 右侧速度取反
    Speed_Sum += Speed_Left;
    /* 计算左右轮平均速度 */
    Speed_Forward = (Speed_Left + Speed_Right) / 2;
    float distance_step = Speed_Forward * PULSE_TO_METER;
    // 获取当前航向角并转换为弧度
    float yaw_rad = (attitude.yaw) * DEG_TO_RAD;

    float delta_x = distance_step * cosf(yaw_rad);
    float delta_y = distance_step * sinf(yaw_rad);

    // 原始的纯里程计累计(存在漂移)
    Robot_Pos_X += delta_x;
    Robot_Pos_Y += delta_y;

    // 融合坐标也同步累计(后续由GPS融合修正)
    Fused_X += delta_x;
    Fused_Y += delta_y;

    /* 执行PID计算,速度环目标为0(保持静止) */
    PID_Calculate(&PID_Speed, Speed_Forward, Speed_Goal);
    if (PID_Speed.Output >= 20)
        PID_Speed.Output = 20;
    if (PID_Speed.Output <= -20)
        PID_Speed.Output = -20;
}

/* 转向角度差速控制,根据航向角误差控制左右轮差速 */
void Angle_Differential_Control()
{
    PID_Calculate_Angle(&PID_Angle, attitude.yaw, Angle_Goal);
}

/* 初始化所有PID控制器 */
void PID_Init_All()
{
    PID_Init_Angular_V();
    PID_Init_Angular();
    PID_Init_Speed();
    PID_Angle_Init();
    PID_High_Init();
}

/* 高度PID计算(横滚角控制) */
void High_Calculate()
{
    PID_Calculate_Angle(&PID_High, attitude.roll, -176.5f);
}

/**
 * @brief  计算从当前位置到目标位置的目标航向角
 * @param  current_x  当前位置的 X 坐标 (米)
 * @param  current_y  当前位置的 Y 坐标 (米)
 * @param  target_x   目标位置的 X 坐标 (米)
 * @param  target_y   目标位置的 Y 坐标 (米)
 * @return float      目标航向角 (单位: 度, 范围: -180.0 到 +180.0)
 */
float Calculate_Target_Angle(float current_x, float current_y, float target_x, float target_y)
{
    // 1. 计算目标点相对于当前位置的坐标差值
    float dx = target_x - current_x;
    float dy = target_y - current_y;

    // 2. 使用 atan2f 计算出弧度制角度
    // 注意:不需要用 atanf,因为 atan2f 会自动处理象限判断,更加强大
    float target_angle_rad = atan2f(dy, dx);

    // 3. 将弧度转换为角度并返回
    return target_angle_rad * RAD_TO_DEG;
}

/******************************************************************************
 * 函数名: Isr_Control
 * 功能描述: 主中断服务函数,根据运行模式执行不同的控制策略
 * 运行模式:
 *   - 模式0: 电机关闭,仅读取传感器数据
 *   - 模式1: 固定点巡逻模式(基于编码器里程计)
 *   - 模式2: 原地平衡模式(速度目标为0)
 *   - 模式3: IMU坐标点巡逻模式
 *   - 模式4: KM2巡逻模式(含原地转向功能)
 *   - 模式5: 遥控采集模式(通过遥控器采集坐标点)
 *   - 模式6: GPS+IMU融合导航模式
 *   - 模式7: GPS导航模式
 * 参数说明: 无
 * 返回值: 无
 * 中断频率: 1kHz(每1ms执行一次)
 ******************************************************************************/
float Target_Points[6][2] = {
    {0.0f, 0.0f},
    {12.0f, 0.0f},
    {12.0f, 1.0f},
    {6.0f, 1.0f},
    {1.0f, 1.0f},
    {0.0f, 0.0f}};
uint8_t Target_Index = 0; // 当前跟踪的目标点索引

void Isr_Control()
{
    TimerTime++; // 中断计数自增

    /* 蜂鸣器控制 */
    if (Buzzer_Time != 0)
    {
        Buzzer_On();
        Buzzer_Time--;
    }
    if (Buzzer_Time == 0)
    {
        Buzzer_Off();
    }

    /* ==================== 模式1: 固定点巡逻模式 ==================== */
    if (Moter_Flag == 1)
    {
        /* 首次进入模式1时初始化航向偏移 */
        if (Mode1_First_Enter_Flag == 1)
        {
            Yaw_Offset = -attitude.yaw;
            Mode1_First_Enter_Flag = 0;
        }

        /* 10ms周期任务(速度环控制) */
        if (TimerTime % 10 == 0)
        {
            if (Target_Index >= 6)
            {
                Speed_Goal = 0;
                Speed_Calculate(); // 每10ms执行一次速度环PID计算
            }
            else
            {
                Speed_Goal = 150;
                // 计算10ms周期内的速度环控制量
                Speed_Calculate(); // 每10ms执行一次速度环PID计算
                float target_x = Target_Points[Target_Index][0];
                float target_y = Target_Points[Target_Index][1];
                Angle_Goal = Calculate_Target_Angle(Robot_Pos_X, Robot_Pos_Y, target_x, target_y);
                // 计算当前位置到目标点的 X、Y 偏差
                float dx = target_x - Robot_Pos_X;
                float dy = target_y - Robot_Pos_Y;
                float distance = sqrtf(dx * dx + dy * dy);
                // 如果距离目标小于 5 厘米,认为到达,切换到下一个点
                if (distance < 0.05f)
                {
                    Target_Index++;
                }
            }
        }

        /* 5ms周期任务(200Hz控制频率) */
        if (TimerTime % 5 == 0)
        {
            updateAttitude_rc(); // 更新姿态角(四元数互补滤波融合)

            Angle_Differential_Control();
            Angular_Calculate(); // 执行角度环PID
        }

        /* 每次中断都执行的任务(1kHz控制频率) */
        IMU660RC_GetData();    // 角速度、加速度采集
        Angular_V_Calculate(); // 执行角速度环PID(最高控制频率)

        /* PWM输出控制(转向差速分配) */
        if (Speed_Forward < 0)
        {
            PWM_SET((int16)(PID_Angular_V.Output * (1 - PID_Angle.Output)), (int16)(PID_Angular_V.Output * (1 + PID_Angle.Output))); // 倒车转向左右分配
        }
        else
        {
            PWM_SET((int16)(PID_Angular_V.Output * (1 + PID_Angle.Output)), (int16)(PID_Angular_V.Output * (1 - PID_Angle.Output))); // 前进转向左右分配
        }
    }

    /* ==================== 模式0: 电机关闭模式 ==================== */
    else if (Moter_Flag == 0)
    {
        PWM_SET(0, 0);       // 电机停止
        if (TimerTime % 10 == 0)
            Speed_Calculate(); // 仅更新里程计,不驱动电机
        if (TimerTime % 5 == 0)
        {
            IMU660RC_GetData();  // 获取IMU660RA传感器原始数据
            updateAttitude_rc(); // 更新姿态角(四元数互补滤波融合)
        }
    }

    /* ==================== 模式2: 原地平衡模式 ==================== */
    else if (Moter_Flag == 2)
    {
        Speed_Goal = 0; // 速度目标为0,保持原地不动

        /* 10ms周期任务 */
        if (TimerTime % 10 == 0)
        {
            Speed_Calculate(); // 每10ms执行一次速度环PID计算
        }
        /* 5ms周期任务(200Hz控制频率) */
        if (TimerTime % 5 == 0)
        {
            updateAttitude_rc(); // 更新姿态角(四元数互补滤波融合)
            Angle_Differential_Control();
            Angular_Calculate(); // 执行角度环PID
        }
        IMU660RC_GetData(); // 角速度、加速度采集
        /* 每次中断都执行的任务(1kHz控制频率) */
        Angular_V_Calculate(); // 执行角速度环PID(最高控制频率)
        /* 限制角速度环输出,防止过冲 */
        if (PID_Angular_V.Output >= 4000)
            PID_Angular_V.Output = 0;
        /* PWM输出控制(转向差速分配) */
        if (Speed_Forward < 0)
        {
            PWM_SET((int16)(PID_Angular_V.Output * (1 - PID_Angle.Output)), (int16)(PID_Angular_V.Output * (1 + PID_Angle.Output))); // 倒车转向左右分配
        }
        else
        {
            PWM_SET((int16)(PID_Angular_V.Output * (1 + PID_Angle.Output)), (int16)(PID_Angular_V.Output * (1 - PID_Angle.Output))); // 前进转向左右分配
        }
    }

    /* ==================== 模式3: IMU坐标点巡逻模式 ==================== */
    else if (Moter_Flag == 3)
    {
        /* 首次进入模式3时初始化 */
        if (Mode3_First_Enter_Flag == 1)
        {
            IMU_Force_Reset_Yaw(0); // 强制重置航向角为0
            Robot_Pos_X = 0.0f;
            Robot_Pos_Y = 0.0f;
            Mode3_First_Enter_Flag = 0;
        }
        if (TimerTime % 10 == 0)
        {
            // 判断条件:如果当前位置索引已达到或超过采集总数,或Flash中无存储数据(count==0),则停车
            if (Target_Index >= current_IMU_point_count_used || current_IMU_point_count_used == 0)
            {
                Speed_Goal = 0;
                Speed_Calculate(); // 每10ms执行一次速度环PID计算
            }
            else
            {
                if(GPS_Get_Angle_Flag==1)
                {
                    Speed_Goal = 300;
                    Angle_Goal=0;
                }
                else
                {
                Speed_Goal = 200;
                // 计算10ms周期内的速度环控制量
                Speed_Calculate(); // 每10ms执行一次速度环PID计算

                // 从 IMU 结构体中获取目标点的 X 和 Y
                float target_x = IMU_Points_used[Target_Index].x;
                float target_y = IMU_Points_used[Target_Index].y;

                Angle_Goal = Calculate_Target_Angle(Robot_Pos_X, Robot_Pos_Y, target_x, target_y);

                // 计算当前位置到目标点的 X、Y 偏差
                float dx = target_x - Robot_Pos_X;
                float dy = target_y - Robot_Pos_Y;
                float distance = sqrtf(dx * dx + dy * dy);

                // 如果距离目标小于 5 厘米,认为到达,切换到下一个点
                if (distance < 0.05f)
                {
                    Target_Index++;
                }
                }
            }
        }

        /* 5ms周期任务(200Hz控制频率) */
        if (TimerTime % 5 == 0)
        {
            updateAttitude_rc(); // 更新姿态角(四元数互补滤波融合)
            Angle_Differential_Control();
            Angular_Calculate(); // 执行角度环PID
        }

        /* 每次中断都执行的任务(1kHz控制频率) */
        IMU660RC_GetData();    // 角速度、加速度采集
        Angular_V_Calculate(); // 执行角速度环PID(最高控制频率)

        /* PWM输出控制(转向差速分配) */
        if (Speed_Forward < 0)
        {
            PWM_SET((int16)(PID_Angular_V.Output * (1 - PID_Angle.Output)), (int16)(PID_Angular_V.Output * (1 + PID_Angle.Output))); // 倒车转向左右分配
        }
        else
        {
            PWM_SET((int16)(PID_Angular_V.Output * (1 + PID_Angle.Output)), (int16)(PID_Angular_V.Output * (1 - PID_Angle.Output))); // 前进转向左右分配
        }
    }

    /* ==================== 模式4: KM2巡逻模式(含原地转向) ==================== */

    else if (Moter_Flag == 4)
    {
        /* 首次进入模式4 */
        if (Mode4_First_Enter_Flag == 1)
        {
            Mode4_First_Enter_Flag = 0;
        }
        /* KM2原地转向模式 */
        if (KM2_Turn_Flag == 1)
        {
            Speed_Goal = 0; // 转向时速度为0(原地转)
            /* 10ms周期任务 */
            if (TimerTime % 10 == 0)
            {
                Speed_Calculate(); // 每10ms执行一次速度环PID计算
            }
            /* 5ms周期任务(200Hz控制频率) */
            if (TimerTime % 5 == 0)
            {
                KM2_Turn_Out += imu660rc_gyro_z; // 累计Z轴陀螺仪积分(用于判断转过角度)

                updateAttitude_rc(); // 更新姿态角(四元数互补滤波融合)
                Angular_Calculate(); // 执行角度环PID
            }
            IMU660RC_GetData(); // 获取IMU660RA传感器原始数据
            /* 每次中断都执行的任务(1kHz控制频率) */
            Angular_V_Calculate(); // 执行角速度环PID(最高控制频率)
            if (PID_Angular_V.Output >= 4000)
                PID_Angular_V.Output = 0;
            /* 差速转向:到达目标点时原地旋转,否则前进中转向 */
            if (Target_Index >= current_IMU_point_count_used_KM2 || current_IMU_point_count_used_KM2 == 0)
                PWM_SET(-(int16)(PID_Angular_V.Output), -(int16)(PID_Angular_V.Output));
            else
                PWM_SET(-(int16)(PID_Angular_V.Output + 500), -(int16)(PID_Angular_V.Output - 500));
            /* 判断是否已转过一圈(约360度,对应陀螺仪积分值1807200) */
            if (KM2_Turn_Out >= 1807200 || KM2_Turn_Out <= -1807200)
            {
                KM2_Turn_Out = 0;   // 清零积分值
                KM2_Turn_Flag = 0;  // 退出转向模式
            }
        }
        /* KM2正常巡逻模式(非转向) */
        else
        {
            if (TimerTime % 10 == 0)
            {
                // 判断条件:如果当前位置索引已达到或超过采集总数,或Flash中无存储数据(count==0),则停车
                if (Target_Index >= current_IMU_point_count_used_KM2 || current_IMU_point_count_used_KM2 == 0)
                {
                    Speed_Goal = 0;
                    Speed_Calculate(); // 每10ms执行一次速度环PID计算
                }
                else
                {
                    Speed_Goal = 180;
                    // 计算10ms周期内的速度环控制量
                    Speed_Calculate(); // 每10ms执行一次速度环PID计算
                    // 从 IMU 结构体中获取目标点的 X 和 Y
                    float target_x = IMU_Points_used_KM2[Target_Index].x;
                    float target_y = IMU_Points_used_KM2[Target_Index].y;

                    Angle_Goal = Calculate_Target_Angle(Robot_Pos_X, Robot_Pos_Y, target_x, target_y);

                    // 计算当前位置到目标点的 X、Y 偏差
                    float dx = target_x - Robot_Pos_X;
                    float dy = target_y - Robot_Pos_Y;
                    float distance = sqrtf(dx * dx + dy * dy);

                    // 如果距离目标小于 5 厘米,认为到达,切换到下一个点并触发原地转向
                    if (distance < 0.05f)
                    {
                        Target_Index++;
                        KM2_Turn_Flag = 1; // 到达目标点,进入转向模式
                    }
                }
            }
            /* 5ms周期任务(200Hz控制频率) */
            if (TimerTime % 5 == 0)
            {
                updateAttitude_rc(); // 更新姿态角(四元数互补滤波融合)
                Angle_Differential_Control();
                Angular_Calculate(); // 执行角度环PID
            }
            /* 每次中断都执行的任务(1kHz控制频率) */
            IMU660RC_GetData();    // 角速度、加速度采集
            Angular_V_Calculate(); // 执行角速度环PID(最高控制频率)

            /* PWM输出控制(转向差速分配) */
            if (Speed_Forward < 0)
        {
            PWM_SET((int16)(PID_Angular_V.Output * (1 - PID_Angle.Output)), (int16)(PID_Angular_V.Output * (1 + PID_Angle.Output))); // 倒车转向左右分配
        }
        else
        {
            PWM_SET((int16)(PID_Angular_V.Output * (1 + PID_Angle.Output)), (int16)(PID_Angular_V.Output * (1 - PID_Angle.Output))); // 前进转向左右分配
        }
        }
    }

    /* ==================== 模式5: 遥控采集模式 ==================== */
    else if (Moter_Flag == 5)
    {
        /* 首次进入模式5时初始化GPS参考点和坐标原点 */
        if (Mode5_First_Enter_Flag == 1)
        {
            Reference_GPS.latitude = gnss.latitude;
            Reference_GPS.longitude = gnss.longitude;
            Robot_Pos_X = 0.0f;
            Robot_Pos_Y = 0.0f;
            Fused_X = 0.0f;
            Fused_Y = 0.0f;
            Mode5_First_Enter_Flag = 0;
        }
        /* 10ms周期任务 */
        if (TimerTime % 10 == 0)
        {
            /* 读取遥控器各通道状态 */
            SWA_state = SWA_Down();
            SWB_state = get_SWB_state();
            CH5_state = CH5_Down();
            CH6_state = CH6_Down();
            Speed_Goal = -speed_convert_clamped(uart_receiver.channel[1]); // 通道2控制速度
            Angle_Goal -= angle_convert_clamped(uart_receiver.channel[0]); // 通道1控制转向角度

            /* SWB=0: IMU坐标点采集模式 */
            if (SWB_state == 0)
            {
                High_Right_Point = 400;
                High_Left_Point = 400;
                /* CH5按下:记录当前坐标点 */
                if (CH5_state == 1)
                {
                    Buzzer_Time=500;
                    if(current_IMU_point_count==0)
                    {
                        Robot_Pos_X = 0.0f;
                        Robot_Pos_Y = 0.0f;
                        IMU_Force_Reset_Yaw(0); // 首次采集时重置航向角
                    }

                    IMU_Points[current_IMU_point_count].x = Robot_Pos_X;
                    IMU_Points[current_IMU_point_count].y = Robot_Pos_Y;
                    current_IMU_point_count++;
                }
                /* CH6按下:保存采集的坐标到Flash */
                if (CH6_state == 1)
                {
                    Buzzer_Time=1000;
                    Save_IMU_To_Flash();
                }
            }
            /* SWB=1: KM2坐标点采集模式 */
            else if (SWB_state == 1)
            {
                /* CH5按下:记录当前坐标点到KM2数组 */
                if (CH5_state == 1)
                {
                    IMU_Points_KM2[current_IMU_point_count_KM2].x = Robot_Pos_X;
                    IMU_Points_KM2[current_IMU_point_count_KM2].y = Robot_Pos_Y;
                    current_IMU_point_count_KM2++;
                }
                /* CH6按下:保存KM2坐标到Flash */
                if (CH6_state == 1)
                {
                    Save_IMU_KM2_To_Flash();
                }
            }
            /* SWB=2: 遥控测试模式 */
            else if (SWB_state == 2)
            {
                /* SWA切换测试子模式 */
                if(SWA_state==1)
                {
                    YaoKong_Test_Mode++;
                    if(YaoKong_Test_Mode>=3)
                    {
                        YaoKong_Test_Mode=0;
                    }
                }
                if(YaoKong_Test_Mode==0)
                {
                    if (CH5_state == 1)
                    {
                    }
                    if (CH6_state == 1)
                    {
                    }
                }
            }
            // 计算10ms周期内的速度环控制量
            Speed_Calculate(); // 每10ms执行一次速度环PID计算
        }

        /* 5ms周期任务(200Hz控制频率) */
        if (TimerTime % 5 == 0)
        {
            updateAttitude_rc(); // 更新姿态角(四元数互补滤波融合)
            /* 安全保护:横滚角过大则停车 */
            if (attitude.roll > -150.0f && attitude.roll < 150.0f)
            {
                menu_serial_number_Two = 1;
                Moter_Flag = 0;
            }
            /* 安全保护:俯仰角过大则停车 */
            if (attitude.pitch < -35.0f || attitude.pitch > 35.0f)
            {
                menu_serial_number_Two = 1;
                Moter_Flag = 0;
            }
            Angle_Differential_Control();
            Angular_Calculate(); // 执行角度环PID
            High_Calculate();    // 执行高度PID
        }

        /* 每次中断都执行的任务(1kHz控制频率) */
        IMU660RC_GetData();    // 角速度、加速度采集
        Angular_V_Calculate(); // 执行角速度环PID(最高控制频率)
        /* PWM输出控制(转向差速分配) */
        if (Speed_Forward < 0)
        {
            PWM_SET((int16)(PID_Angular_V.Output * (1 - PID_Angle.Output)), (int16)(PID_Angular_V.Output * (1 + PID_Angle.Output))); // 倒车转向左右分配
        }
        else
        {
            PWM_SET((int16)(PID_Angular_V.Output * (1 + PID_Angle.Output)), (int16)(PID_Angular_V.Output * (1 - PID_Angle.Output))); // 前进转向左右分配
        }
    }

    /* ==================== 模式6: GPS+IMU融合导航模式 ==================== */
    else if (Moter_Flag == 6)
    {
        /* 首次进入模式6时初始化GPS参考点和计算初始航向偏移 */
        if (Mode6_First_Enter_Flag == 1)
        {
            Mode6_First_Enter_Flag = 0;
            Reference_GPS.latitude = gnss.latitude;
            Reference_GPS.longitude = gnss.longitude;
            float target_x = IMU_GPS_Used[1].x;
            float target_y = IMU_GPS_Used[1].y;
            float start_angle = Calculate_Target_Angle(0.0f, 0.0f, target_x, target_y);
            // 计算航向偏移量 = 坐标系期望角度 - 机体当前实际角度
            Yaw_Offset = start_angle - attitude.yaw;
        }

        if (TimerTime % 10 == 0)
        {
            /* GPS坐标融合更新 */
            if (GPS_XY_Flag == 1)
            {
                GPS_XY_Flag = 0;
                Update_GPS_Now_XY();
                float err_x = GPS_X_Now - Fused_X;
                float err_y = GPS_Y_Now - Fused_Y;

                // 设定融合阈值(米),防止GPS突然大幅漂移(如5米)时误修正
                if (sqrtf(err_x * err_x + err_y * err_y) < 5.0f)
                {
                    // 低通滤波融合:逐步修正融合坐标以趋近GPS位置
                    Fused_X += GPS_Weight * err_x;
                    Fused_Y += GPS_Weight * err_y;
                }
            }
            // 判断条件:如果当前位置索引已达到或超过采集总数,或Flash中无存储数据(count==0),则停车
            if (Target_Index >= current_IMU_GPS_Num_Used || current_IMU_GPS_Num_Used == 0)
            {
                Speed_Goal = 0;
                Speed_Calculate(); // 每10ms执行一次速度环PID计算
            }
            else
            {
                Speed_Goal = 300;
                // 计算10ms周期内的速度环控制量
                Speed_Calculate(); // 每10ms执行一次速度环PID计算

                // 从 IMU 结构体中获取目标点的 X 和 Y
                float target_x = IMU_GPS_Used[Target_Index].x;
                float target_y = IMU_GPS_Used[Target_Index].y;
                Angle_Goal = Calculate_Target_Angle(Fused_X, Fused_Y, target_x, target_y);

                // 计算当前位置到目标点的 X、Y 偏差
                float dx = target_x - Fused_X;
                float dy = target_y - Fused_Y;
                float distance = sqrtf(dx * dx + dy * dy);

                // 如果距离目标小于 10 厘米,认为到达,切换到下一个点
                if (distance < 0.10)
                {
                    Buzzer_Time = 500;
                    Target_Index++;
                }
            }
        }
        /* 5ms周期任务(200Hz控制频率) */
        if (TimerTime % 5 == 0)
        {
            updateAttitude_rc(); // 更新姿态角(四元数互补滤波融合)
            Angle_Differential_Control();
            Angular_Calculate(); // 执行角度环PID
        }
        /* 每次中断都执行的任务(1kHz控制频率) */
        IMU660RC_GetData();    // 角速度、加速度采集
        Angular_V_Calculate(); // 执行角速度环PID(最高控制频率)

        /* PWM输出控制(转向差速分配) */
        if (Speed_Forward < 0)
        {
            PWM_SET((int16)(PID_Angular_V.Output * (1 - PID_Angle.Output)), (int16)(PID_Angular_V.Output * (1 + PID_Angle.Output))); // 倒车转向左右分配
        }
        else
        {
            PWM_SET((int16)(PID_Angular_V.Output * (1 + PID_Angle.Output)), (int16)(PID_Angular_V.Output * (1 - PID_Angle.Output))); // 前进转向左右分配
        }
    }

    /* ==================== 模式7: GPS导航模式 ==================== */
    else if (Moter_Flag == 7)
    {
        /* 首次进入模式7时初始化GPS参考点和计算初始航向偏移 */
        if (Mode7_First_Enter_Flag == 1)
        {
            Mode7_First_Enter_Flag = 0;
            Reference_GPS.latitude = gnss.latitude;
            Reference_GPS.longitude = gnss.longitude;
            float target_x = IMU_Points_used[1].x;
            float target_y = IMU_Points_used[1].y;
            float start_angle = Calculate_Target_Angle(0.0f, 0.0f, target_x, target_y);
            // 计算航向偏移量 = 坐标系期望角度 - 机体当前实际角度
            Yaw_Offset = start_angle - attitude.yaw;
        }
        if (TimerTime % 10 == 0)
        {
            if (Target_Index >= current_IMU_GPS_Num_Used || current_IMU_GPS_Num_Used == 0)
            {
                Speed_Goal = 0;
                Speed_Calculate(); // 每10ms执行一次速度环PID计算
            }
            else
            {
                Update_GPS_Now_XY(); // 更新当前GPS坐标
                Speed_Goal = 300;
                // 计算10ms周期内的速度环控制量
                Speed_Calculate(); // 每10ms执行一次速度环PID计算

                // 从 IMU 结构体中获取目标点的 X 和 Y
                float target_x = IMU_Points_used[Target_Index].x;
                float target_y = IMU_Points_used[Target_Index].y;
                Angle_Goal = Calculate_Target_Angle(GPS_X_Now, GPS_Y_Now, target_x, target_y);

                // 计算当前位置到目标点的 X、Y 偏差
                float dx = target_x - GPS_X_Now;
                float dy = target_y - GPS_Y_Now;
                float distance = sqrtf(dx * dx + dy * dy);

                // 如果距离目标小于 10 厘米,认为到达,切换到下一个点
                if (distance < 0.10)
                {
                    Buzzer_Time = 500;
                    Target_Index++;
                }
            }
        }
        /* 5ms周期任务(200Hz控制频率) */
        if (TimerTime % 5 == 0)
        {
            updateAttitude_rc(); // 更新姿态角(四元数互补滤波融合)
            Angle_Differential_Control();
            Angular_Calculate(); // 执行角度环PID
        }
        /* 每次中断都执行的任务(1kHz控制频率) */
        IMU660RC_GetData();    // 角速度、加速度采集
        Angular_V_Calculate(); // 执行角速度环PID(最高控制频率)

        /* PWM输出控制(转向差速分配) */
        if (Speed_Forward < 0)
        {
            PWM_SET((int16)(PID_Angular_V.Output * (1 - PID_Angle.Output)), (int16)(PID_Angular_V.Output * (1 + PID_Angle.Output))); // 倒车转向左右分配
        }
        else
        {
            PWM_SET((int16)(PID_Angular_V.Output * (1 + PID_Angle.Output)), (int16)(PID_Angular_V.Output * (1 - PID_Angle.Output))); // 前进转向左右分配
        }
    }
}

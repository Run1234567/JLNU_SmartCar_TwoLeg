#ifndef CODE_JLNU_IMU963AR_H_
#define CODE_JLNU_IMU963AR_H_

// 欧拉角数据结构
typedef struct
{
    float roll;     // 横滚角
    float pitch;    // 俯仰角
    float yaw;      // 偏航角
    float gyro_roll;   // 陀螺仪计算的横滚角
    float gyro_pitch;  // 陀螺仪计算的俯仰角
    float acc_roll;    // 加速度计计算的横滚角
    float acc_pitch;   // 加速度计计算的俯仰角
} euler_angle_struct;


void print_euler_angle(void);
void euler_angle_init(void);
void calculate_yaw_with_mag(void);
void calculate_euler_angle(void);

extern euler_angle_struct euler_angle;
extern uint32_t TIMER_FLAG;

#endif

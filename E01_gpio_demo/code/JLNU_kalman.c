#include "JLNU_kalman.h"
#include "math.h"

/* 限幅宏定义 */
#define LIMIT( x, min, max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )

/**
 * @name   KalmanCreate
 * @brief  创建一个卡尔曼滤波器
 * @param  p:    滤波器结构体指针
 *         T_Q:  系统过程噪声协方差
 *         T_R:  观测噪声协方差
 * @retval 无
 * @attention R固定时，Q越大越能容忍测量值的波动，Q越小越信任模型预测值。
 *            Q为0时只信任模型预测。
 *            反之，Q越小越能滤除测量值的波动，只保留模型预测值。Q为无穷时只信任测量值。
 */
void KalmanCreate(extKalman_t *p, float T_Q, float T_R)
{
    p->X_last = (float)0;
    p->P_last = 0;
    p->Q = T_Q;        /* 过程噪声协方差 */
    p->R = T_R;        /* 观测噪声协方差 */
    p->A = 1;          /* 状态转移参数 */
    p->B = 0;          /* 控制输入参数 */
    p->H = 1;          /* 观测参数 */
    p->X_mid = p->X_last;
}

/**
 * @name   KalmanFilter
 * @brief  执行一次卡尔曼滤波计算
 * @param  p:    滤波器结构体指针
 *         dat:  待滤波的测量数据
 * @retval 滤波后的估计值
 * @attention Z(k)是系统输入，即测量值。X(k|k)是卡尔曼滤波输出值，即最优估计。
 *            A=1 B=0 H=1 I=1。W(K)、V(k)是高斯噪声，相互独立且不考虑过程，因此不用管。
 *            下面是卡尔曼滤波的5个核心公式。
 *            说明：H'因为是标量，所以等于转置矩阵。
 */
float KalmanFilter(extKalman_t *p, float dat)
{
    p->X_mid = p->A * p->X_last;                               /* 预测公式(1): x(k|k-1) = A*X(k-1|k-1)+B*U(k)+W(K) */
    p->P_mid = p->A * p->P_last + p->Q;                        /* 预测公式(2): p(k|k-1) = A*p(k-1|k-1)*A'+Q */
    p->kg = p->P_mid / (p->P_mid + p->R);                      /* 更新公式(4): kg(k) = p(k|k-1)*H'/(H*p(k|k-1)*H'+R) */
    p->X_now = p->X_mid + p->kg * (dat - p->X_mid);           /* 更新公式(3): x(k|k) = X(k|k-1)+kg(k)*(Z(k)-H*X(k|k-1)) */
    p->P_now = (1 - p->kg) * p->P_mid;                         /* 更新公式(5): p(k|k) = (I-kg(k)*H)*P(k|k-1) */
    p->P_last = p->P_now;                                      /* 状态更新 */
    p->X_last = p->X_now;
    return p->X_now;                                            /* 输出最优估计值 x(k|k) */
}


/**
 * @brief 一阶低通滤波器更新函数
 *
 * 使用一阶RC低通滤波器的离散化公式进行滤波计算。
 * 适用于对噪声信号进行平滑处理，滤除高频噪声。
 *
 * @param hz   截止频率(Hz)，滤波器-3dB衰减点对应的频率
 * @param time 采样时间间隔(s)，即两次调用本函数之间的时间
 * @param in   当前时刻的输入值
 * @param out  指向输出值的指针，即为状态(上次输出值)也是输出(当前滤波值)
 *
 * @note 公式推导：
 *       模拟传递函数：H(s) = 1 / (tao*s + 1)，其中 tao = 1/(2*pi*f_c)
 *       离散化(前向差分)：s 变为 (1 - z^{-1}) / T
 *       频域变换：y[n] = y[n-1] + T/(tao+T) * (x[n] - y[n-1])
 *       其中系数 alpha = T/(tao+T) = 1 / (1 + tao/T) = 1 / (1 + 1/(2*pi*f_c*T))
 *       注：6.28f近似为2*pi的近似值
 */
void LPF_1(float hz, float time, float in, float *out)
{
    /* 计算滤波系数：alpha = 1 / (1 + 1/(2*pi*f_c*T)) */
    /* 其中：hz为截止频率，time为采样周期，6.28f近似为2*pi */
    float alpha = 1.0f / (1.0f + 1.0f / (hz * 6.28f * time));

    /* 一阶低通滤波递推公式：
     * y[n] = y[n-1] + alpha * (x[n] - y[n-1])
     * 等价于：y[n] = alpha*x[n] + (1-alpha)*y[n-1] */
    *out += alpha * (in - *out);
}


/**
 * @brief 限幅滤波器
 * @param T    采样时间间隔
 * @param hz   截止频率
 * @param data 待滤波的原始数据
 * @retval 限幅滤波后的输出值
 */
float limit_filter(float T, float hz, float data)
{
    float abs_t;
    float Lim = 0;
    LPF_1(hz, T, data, &Lim);
    abs_t = fabs(Lim);
    return LIMIT(data, -abs_t, abs_t);
}

/**
 * @author  Liu heng
 * 卡尔曼滤波器头文件，参考RoboMaster论坛
 */

#ifndef _JLNU_KALMAN_H
#define _JLNU_KALMAN_H

/* 卡尔曼滤波器状态结构体 */
typedef struct {
    float X_last;   /* 上一时刻的最优估计值 X(k-1|k-1) */
    float X_mid;    /* 当前时刻的预测估计值 X(k|k-1) */
    float X_now;    /* 当前时刻的最优估计值 X(k|k) */
    float P_mid;    /* 当前时刻预测误差协方差 P(k|k-1) */
    float P_now;    /* 当前时刻最优估计误差协方差 P(k|k) */
    float P_last;   /* 上一时刻最优估计误差协方差 P(k-1|k-1) */
    float kg;       /* 卡尔曼增益 */
    float A;        /* 系统参数 */
    float B;
    float Q;        /* 过程噪声协方差 */
    float R;        /* 观测噪声协方差 */
    float H;        /* 观测矩阵参数 */
} extKalman_t;

/* 卡尔曼滤波器函数声明 */
void KalmanCreate(extKalman_t *p, float T_Q, float T_R);    /* 创建并初始化卡尔曼滤波器 */
float KalmanFilter(extKalman_t *p, float dat);                /* 执行卡尔曼滤波计算 */

/* 一阶低通滤波器函数 */
void LPF_1(float hz, float time, float in, float *out);      /* 一阶低通滤波器更新函数 */

/* 限幅滤波器函数 */
float limit_filter(float T, float hz, float data);            /* 限幅滤波器 */

#endif

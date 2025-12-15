#include "JLNU_kalman.h"
#include "math.h"
#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )
/**
  * @name   kalmanCreate
  * @brief  创建一个卡尔曼滤波器
  * @param  p:  滤波器
  *         T_Q:系统噪声协方差
  *         T_R:测量噪声协方差
  *         
  * @retval none
  * @attention R固定，Q越大，代表越信任侧量值，Q无穷代表只用测量值
  *		       	反之，Q越小代表越信任模型预测值，Q为零则是只用模型预测
  */
void KalmanCreate(extKalman_t *p,float T_Q,float T_R)
{
    p->X_last = (float)0;
    p->P_last = 0;
    p->Q = T_Q;
    p->R = T_R;
    p->A = 1;
		p->B = 0;
    p->H = 1;
    p->X_mid = p->X_last;
}

/**
  * @name   KalmanFilter
  * @brief  卡尔曼滤波器
  * @param  p:  滤波器
  *         dat:待滤波数据
  * @retval 滤波后的数据
  * @attention Z(k)是系统输入,即测量值   X(k|k)是卡尔曼滤波后的值,即最终输出
  *            A=1 B=0 H=1 I=1  W(K)  V(k)是高斯白噪声,叠加在测量值上了,可以不用管
  *            以下是卡尔曼的5个核心公式
  *            一阶H'即为它本身,否则为转置矩阵
  */

float KalmanFilter(extKalman_t* p,float dat)
{
    p->X_mid =p->A*p->X_last;                     //百度对应公式(1)    x(k|k-1) = A*X(k-1|k-1)+B*U(k)+W(K)
    p->P_mid = p->A*p->P_last+p->Q;               //百度对应公式(2)    p(k|k-1) = A*p(k-1|k-1)*A'+Q
    p->kg = p->P_mid/(p->P_mid+p->R);             //百度对应公式(4)    kg(k) = p(k|k-1)*H'/(H*p(k|k-1)*H'+R)
    p->X_now = p->X_mid+p->kg*(dat-p->X_mid);     //百度对应公式(3)    x(k|k) = X(k|k-1)+kg(k)*(Z(k)-H*X(k|k-1))
    p->P_now = (1-p->kg)*p->P_mid;                //百度对应公式(5)    p(k|k) = (I-kg(k)*H)*P(k|k-1)
    p->P_last = p->P_now;                         //状态更新
    p->X_last = p->X_now;
    return p->X_now;							  //输出预测结果x(k|k)
}


/**
 * @brief 一阶低通滤波器更新函数
 * 
 * 使用一阶RC低通滤波器的离散化形式进行滤波计算。
 * 适用于对输入信号进行平滑处理，去除高频噪声。
 * 
 * @param hz  截止频率(Hz)，滤波器-3dB衰减点对应的频率
 * @param time 采样时间间隔(s)，即两次调用本函数的时间差
 * @param in   当前时刻的输入值
 * @param out  指向输出值的指针，既作为输入(上次输出值)，也作为输出(本次输出值)
 * 
 * @note 公式推导：
 *       连续域传递函数：H(s) = 1 / (τs + 1)，其中τ = 1/(2πf_c)
 *       离散化(后向差分)：s ≈ (1 - z^{-1}) / T
 *       推导得：y[n] = y[n-1] + T/(τ+T) * (x[n] - y[n-1])
 *       其中系数α = T/(τ+T) = 1 / (1 + τ/T) = 1 / (1 + 1/(2πf_c*T))
 *       函数中6.28f为2π的近似值
 */
void LPF_1(float hz, float time, float in, float *out)
{
    // 计算滤波器系数α：α = 1 / (1 + 1/(2π*f_c*T))
    // 其中：hz为截止频率，time为采样周期，6.28f≈2π
    float alpha = 1.0f / (1.0f + 1.0f / (hz * 6.28f * time));
    
    // 一阶低通滤波递推公式：
    // y[n] = y[n-1] + α * (x[n] - y[n-1])
    // 等价于：y[n] = α*x[n] + (1-α)*y[n-1]
    *out += alpha * (in - *out);
}


float limit_filter(float T,float hz, float data)
{
    float abs_t;
    float Lim=0;
    LPF_1(hz,T,  data,&Lim);
    abs_t = fabs(Lim);
   return LIMIT(data,-abs_t,abs_t);
}


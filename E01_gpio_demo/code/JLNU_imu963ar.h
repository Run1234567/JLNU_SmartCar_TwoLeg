#ifndef CODE_JLNU_IMU963AR_H_
#define CODE_JLNU_IMU963AR_H_
void IMU963_Init();
void IMU963_Read_Data();
void updateAttitude_2(void);
void initAttitude(void);
// 常量定义
#ifndef M_PI
#define M_PI 3.1415926535f
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD (M_PI / 180.0f)
#endif

#ifndef RAD_TO_DEG  
#define RAD_TO_DEG (180.0f / M_PI)
#endif

#ifndef SAMPLE_TIME_MS
#define SAMPLE_TIME_MS 5.0f  // 默认采样时间10ms
#endif

#endif

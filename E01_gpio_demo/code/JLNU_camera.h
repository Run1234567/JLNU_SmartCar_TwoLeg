#ifndef JLNU_CAMERA_H
#define JLNU_CAMERA_H
#include "zf_common_headfile.h"

extern uint8 Left_boundary_x[];
extern uint8 Right_boundary_x[];
extern uint8 Left_boundary_y[];
extern uint8 Right_boundary_y[];
extern uint8 image_copy[MT9V03X_H][MT9V03X_W];
void Get_Start_Point_image(void);

#define MAX_FIND_POINTS        180  

extern int left_point_cnt;  // 左边界实际找到的点数
extern int right_point_cnt; // 右边界实际找到的点数
void Trace_Left_Boundary();
void Trace_Right_Boundary();

#endif

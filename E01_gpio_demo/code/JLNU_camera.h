/*
 * JLNU_camera.h
 *
 *  摄像头模块头文件
 *  声明了左右边界数组、图像缓冲区、边界追踪函数等
 */

#ifndef JLNU_CAMERA_H
#define JLNU_CAMERA_H
#include "zf_common_headfile.h"

extern uint8 Left_boundary_x[];   /* 左边界X坐标数组 */
extern uint8 Right_boundary_x[];  /* 右边界X坐标数组 */
extern uint8 Left_boundary_y[];   /* 左边界Y坐标数组 */
extern uint8 Right_boundary_y[];  /* 右边界Y坐标数组 */
extern uint8 image_copy[MT9V03X_H][MT9V03X_W];  /* 图像副本缓冲区 */
void Get_Start_Point_image(void);                 /* 从底部行搜索左右边界的起始点 */

#define MAX_FIND_POINTS        180   /* 最大追踪边界点数 */

extern int left_point_cnt;   /* 左边界实际找到的点数 */
extern int right_point_cnt;  /* 右边界实际找到的点数 */
void Trace_Left_Boundary();   /* 左边界追踪函数 */
void Trace_Right_Boundary();  /* 右边界追踪函数 */

#endif
/*
 * JLNU_camera.c
 *
 *  摄像头模块实现
 *  包含差比和边缘检测算法、底部起始点搜索、左/右边界追踪
 */

#include "zf_common_headfile.h"
#include <stdlib.h> /* 包含标准库头文件，以使用 abs() 绝对值函数 */

/* 八方向偏移表，用于边界追踪时的邻域搜索
 * 方向顺序：0=右, 1=右下, 2=下, 3=左下, 4=左, 5=左上, 6=上, 7=右上
 * 每个方向步长为2像素 */
int8_t dir_offset[8][2] = {
    {0, 2},     /* 方向0: 正右方 */
    {2, 2},     /* 方向1: 右下方 */
    {2, 0},     /* 方向2: 正下方 */
    {2, -2},    /* 方向3: 左下方 */
    {0, -2},    /* 方向4: 正左方 */
    {-2, -2},   /* 方向5: 左上方 */
    {-2, 0},    /* 方向6: 正上方 */
    {-2, 2},    /* 方向7: 右上方 */
};

/* 左右边界X/Y坐标数组，初始值设置为图像中间偏左/偏右 */
uint8 Left_boundary_x[200] = {150, 150, 150};
uint8 Right_boundary_x[200] = {100, 100, 100};
uint8 Left_boundary_y[200] = {119, 119, 119};
uint8 Right_boundary_y[200] = {119, 119, 119};

//-------------------------------------------------------------------------------------------------------------------
//  函数名     计算两个像素的差比和
//  参数说明   num1: 参与计算的第一个数 (通常是像素1的灰度值)
//             num2: 参与计算的第二个数 (通常是像素2的灰度值)
//  返回参数   返回差比和的百分比形式值 (0 ~ 100)
//-------------------------------------------------------------------------------------------------------------------
int Calc_Difference_Sum_Ratio(int num1, int num2)
{
    int sum = num1 + num2;
    int diff = (num1 - num2);

    /* 防止除零保护：如果和为0则直接返回0，因为除数为0会导致芯片直接崩溃 */
    if (sum == 0)
    {
        return 0;
    }

    /* 核心计算：先乘100再除以和，尽量避免浮点数 (float)，对单片机运算速度极快 */
    return (diff * 100) / sum;
}

/* 差比和阈值，超过该阈值则认为存在黑白边界(根据实际场景的曝光情况在 15~40 之间调整) */
#define EDGE_RATIO_THRESHOLD 10

/* 定义全局变量，用于存储找到的左右边界起始X坐标 */
int left_start_x = 0;
int right_start_x = MT9V03X_W - 1;

//-------------------------------------------------------------------------------------------------------------------
//  函数名     从底部行图像搜索左右边界的起始点
//  说明       在图像最底行(y=119)，从中间向左右两侧扫描，寻找边缘跳变点作为边界起始
//-------------------------------------------------------------------------------------------------------------------
void Get_Start_Point_image()
{
    /* 获取最后一行 y = 119，防止数组越界 */
    int bottom_y = MT9V03X_H - 1;

    /* 获取图像中点(假设车身正对时，赛道在图像中央) */
    int mid_x = MT9V03X_W / 2;

    /* 每次扫描前先初始化左右边界为图像两端 */
    left_start_x = 0;
    right_start_x = MT9V03X_W - 1;

    /* 对记录数组赋予初值，防止万一没有找到边界时使用上一帧的旧数据 */
    Left_boundary_x[0] = left_start_x;
    Right_boundary_x[0] = right_start_x;

    /* 定义搜索步长为2，每两个像素比较一次 (x 与 x-2 / x 与 x+2 比较) */
    int step = 2;

    /* ========================================== */
    /* 1. 从中间向左扫描，寻找左边界               */
    /* ========================================== */
    /* 注意循环终止条件：x >= step，保证 x - step 不会负数越界 */
    for (int x = mid_x; x >= step; x--)
    {
        /* 计算当前与左侧像素的差比和 (比较 x 与 x-2) */
        int ratio = Calc_Difference_Sum_Ratio(image_copy[bottom_y][x], image_copy[bottom_y][x - step]);

        /* 如果差比和超过设定阈值，说明此处存在边缘的黑白跳变 */
        if (abs(ratio) > EDGE_RATIO_THRESHOLD)
        {
            left_start_x = x - 2; /* 记录左边界坐标 */
            Left_boundary_x[0] = left_start_x;
            break; /* 找到了就立即退出循环，停止继续搜索 */
        }
    }

    /* ========================================== */
    /* 2. 从中间向右扫描，寻找右边界               */
    /* ========================================== */
    /* 注意循环终止条件：x < MT9V03X_W - step，保证 x + step 不会大于等于图像宽度而越界 */
    for (int x = mid_x; x < MT9V03X_W - step; x++)
    {
        /* 计算当前与右侧像素的差比和 (比较 x 与 x+2) */
        int ratio = Calc_Difference_Sum_Ratio(image_copy[bottom_y][x], image_copy[bottom_y][x + step]);

        /* 如果差比和超过设定阈值，说明此处存在边缘的黑白跳变 */
        if (abs(ratio) > EDGE_RATIO_THRESHOLD)
        {
            right_start_x = x + 2; /* 记录右边界坐标 */
            Right_boundary_x[0] = right_start_x;
            break; /* 找到了就立即退出循环，停止继续搜索 */
        }
    }
}

/* 防止死循环的最大迭代次数 (一般图像高度 120，设定180足够) */
#define MAX_FIND_POINTS 180

int left_point_cnt = 1;   /* 左边界实际找到的点数 */
int right_point_cnt = 1;  /* 右边界实际找到的点数 */

//-------------------------------------------------------------------------------------------------------------------
//  函数名    左边界追踪算法 (从起始点向下搜索 + 顺时针方向)
//-------------------------------------------------------------------------------------------------------------------
void Trace_Left_Boundary()
{
    int curr_x = Left_boundary_x[0];  /* 当前追踪点的X坐标 */
    int curr_y = Left_boundary_y[0];  /* 当前追踪点的Y坐标 */

    left_point_cnt = 1;  /* 已有起始点，从1开始计数 */

    for (int i = 1; i < MAX_FIND_POINTS; i++)
    {
        /* 数组安全保护，防止写入越界 */
        if (left_point_cnt >= 199)
        {
            break;
        }

        int found = 0;  /* 标记本轮是否找到下一个边界点 */

        /* 起始方向，从下方开始搜索，方向0 */
        int start_dir = 0;

        int has_last_val = 0;  /* 标记是否已记录上一个像素值 */
        int last_val = 0;      /* 上一个像素的灰度值 */

        /* 扫描 7 个方向，留 1 个方向是防止原地 180度 回头 */
        for (int j = 0; j < 8; j++)
        {
            /* 顺时针扫描 (0 -> 7 -> 6 -> 5 -> 4 -> 3 -> 2) */
            int check_dir = (start_dir - j + 8) % 8;
            int check_x = curr_x + dir_offset[check_dir][0];
            int check_y = curr_y + dir_offset[check_dir][1];

            /* 1. 图像边界越界保护 */
            if (check_x < 0 || check_x >= MT9V03X_W || check_y < 0 || check_y >= MT9V03X_H)
            {
                continue;
            }

            int curr_val = image_copy[check_y][check_x];

            /* 2. 记录起始值 */
            if (!has_last_val)
            {
                last_val = curr_val;
                has_last_val = 1;
                continue;
            }

            /* 3. 计算差比和 */
            int ratio = Calc_Difference_Sum_Ratio(last_val, curr_val);

            /* 4. 判断边界：取反后的ratio大于阈值则认为找到边界 */
            if (-(ratio) > EDGE_RATIO_THRESHOLD)
            {
                curr_x = check_x;
                curr_y = check_y;

                Left_boundary_x[left_point_cnt] = curr_x;
                Left_boundary_y[left_point_cnt] = curr_y;
                left_point_cnt++;

                found = 1;
                break;  /* 找到后退出方向搜索循环 */
            }

            last_val = curr_val;
        }

        /* 如果当前点没有出路（未找到下一个边界点） */
        if (!found)
        {
            break;  /* 退出追踪循环 */
        }
    }

    /* ========================================== */
    /* 填充未用到的数组空间为0                      */
    /* ========================================== */
    for (int i = left_point_cnt; i < 200; i++)
    {
        Left_boundary_x[i] = 0;
        Left_boundary_y[i] = 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  函数名    右边界追踪算法 (从起始点向下搜索 + 逆时针方向)
//-------------------------------------------------------------------------------------------------------------------
void Trace_Right_Boundary()
{
    int curr_x = Right_boundary_x[0];  /* 当前追踪点的X坐标 */
    int curr_y = Right_boundary_y[0];  /* 当前追踪点的Y坐标 */

    right_point_cnt = 1;  /* 已有起始点，从1开始计数 */

    for (int i = 1; i < MAX_FIND_POINTS; i++)
    {
        /* 数组安全保护，防止写入越界 */
        if (right_point_cnt >= 199)
        {
            break;
        }

        int found = 0;  /* 标记本轮是否找到下一个边界点 */

        /* 起始方向，从下方开始搜索，方向0 */
        int start_dir = 0;

        int has_last_val = 0;  /* 标记是否已记录上一个像素值 */
        int last_val = 0;      /* 上一个像素的灰度值 */

        /* 扫描 7 个方向，留 1 个方向是防止原地 180度 回头 */
        for (int j = 0; j < 8; j++)
        {
            /* 逆时针扫描 (0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6) */
            int check_dir = (start_dir + j) % 8;
            int check_x = curr_x + dir_offset[check_dir][0];
            int check_y = curr_y + dir_offset[check_dir][1];

            /* 1. 图像边界越界保护 */
            if (check_x < 0 || check_x >= MT9V03X_W || check_y < 0 || check_y >= MT9V03X_H)
            {
                continue;
            }

            int curr_val = image_copy[check_y][check_x];

            /* 2. 记录起始值 */
            if (!has_last_val)
            {
                last_val = curr_val;
                has_last_val = 1;
                continue;
            }

            /* 3. 计算差比和 */
            int ratio = Calc_Difference_Sum_Ratio(last_val, curr_val);

            /* 4. 判断边界：取反后的ratio大于阈值则认为找到边界 */
            if (-(ratio) > EDGE_RATIO_THRESHOLD)
            {
                curr_x = check_x;
                curr_y = check_y;

                Right_boundary_x[right_point_cnt] = curr_x;
                Right_boundary_y[right_point_cnt] = curr_y;
                right_point_cnt++;

                found = 1;
                break;  /* 找到后退出方向搜索循环 */
            }

            last_val = curr_val;
        }

        if (!found)
        {
            break;  /* 退出追踪循环 */
        }
    }

    /* ========================================== */
    /* 填充未用到的数组空间为0                      */
    /* ========================================== */
    for (int i = right_point_cnt; i < 200; i++)
    {
        Right_boundary_x[i] = 0;
        Right_boundary_y[i] = 0;
    }
}
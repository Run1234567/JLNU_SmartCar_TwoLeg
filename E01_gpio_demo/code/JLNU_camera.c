#include "zf_common_headfile.h"
#include <stdlib.h> // 必须包含此头文件以使用 abs() 绝对值函数
int8_t dir_offset[8][2] = {
    {0, 2},
    {2, 2},
    {2, 0},
    {2, -2},
    {0, -2},
    {-2, -2},
    {-2, 0},
    {-2, 2},
};
uint8 Left_boundary_x[200] = {150, 150, 150};
uint8 Right_boundary_x[200] = {100, 100, 100};
uint8 Left_boundary_y[200] = {119, 119, 119};
uint8 Right_boundary_y[200] = {119, 119, 119};
//----------------------------------------------------- --------------------------------------------------------------
// 函数名称     计算两个数的差比和
// 参数说明     num1: 输入的第一个数 (例如像素点1的灰度值)
// 参数说明     num2: 输入的第二个数 (例如像素点2的灰度值)
// 返回参数     返回差比和的百分比整数值 (0 ~ 100)
//-------------------------------------------------------------------------------------------------------------------
int Calc_Difference_Sum_Ratio(int num1, int num2)
{
    int sum = num1 + num2;
    int diff = (num1 - num2); 

    // 致命错误保护：如果两个数都是 0，相加为 0，作为分母会导致单片机直接卡死重启
    if (sum == 0)
    {
        return 0;
    }

    // 核心计算：先乘 100 再除以和，完美避开浮点数 (float)，单片机运行速度极快
    return (diff * 100) / sum;
}
// 定义差比和阈值，超过此阈值认为存在黑白边界（建议根据实际场地光线在 15~40 之间调参）
#define EDGE_RATIO_THRESHOLD 10

// 定义全局变量，用于存储找到的左右边界起点的X坐标
int left_start_x = 0;
int right_start_x = MT9V03X_W - 1;

void Get_Start_Point_image()
{
    // 获取最底行（y = 119），防止数组越界
    int bottom_y = MT9V03X_H - 1;

    // 获取图像中点（假设车体起步时，赛道大致在画面中央）
    int mid_x = MT9V03X_W / 2;

    // 每次扫线前，先初始化边界点为最左和最右
    left_start_x = 0;
    right_start_x = MT9V03X_W - 1;

    // 给记录数组赋初值防丢（防止本次没找到边界时，数组里留下上一帧的旧脏数据）
    Left_boundary_x[0] = left_start_x;
    Right_boundary_x[0] = right_start_x;

    // 定义检测步长：步长为2，即中间隔1个点 (x 与 x-2 / x 与 x+2 比较)
    int step = 2;

    // ==========================================
    // 1. 从中心向左扫线，寻找左边界
    // ==========================================
    // 注意循环条件：必须限制 x >= step，保证 x - step 不会变成负数越界
    for (int x = mid_x; x >= step; x--)
    {
        // 计算中间隔一个点的两个像素的差比和 (例如比较 x 和 x-2)
        int ratio = Calc_Difference_Sum_Ratio(image_copy[bottom_y][x], image_copy[bottom_y][x - step]);

        // 如果差比和大于设定的阈值，说明出现了明显的黑白交界
        if (abs(ratio) > EDGE_RATIO_THRESHOLD)
        {
            left_start_x = x - 2; // 记录左边界坐标
            Left_boundary_x[0] = left_start_x;
            break; // 找到后立刻跳出循环，停止向左寻找
        }
    }

    // ==========================================
    // 2. 从中心向右扫线，寻找右边界
    // ==========================================
    // 注意循环条件：必须限制 x < MT9V03X_W - step，保证 x + step 不会大于等于图像宽度越界
    for (int x = mid_x; x < MT9V03X_W - step; x++)
    {
        // 计算中间隔一个点的两个像素的差比和 (例如比较 x 和 x+2)
        int ratio = Calc_Difference_Sum_Ratio(image_copy[bottom_y][x], image_copy[bottom_y][x + step]);

        // 如果差比和大于设定的阈值，说明出现了明显的黑白交界
        if (abs(ratio) > EDGE_RATIO_THRESHOLD)
        {
            right_start_x = x + 2; // 记录右边界坐标
            Right_boundary_x[0] = right_start_x;
            break; // 找到后立刻跳出循环，停止向右寻找
        }
    }
}
// 定义最大爬坡点数，防止死循环 (建议根据图像高度 120 设定，180 足够了)
#define MAX_FIND_POINTS 180

int left_point_cnt = 1;  // 左边界实际找到的点数
int right_point_cnt = 1; // 右边界实际找到的点数

//-------------------------------------------------------------------------------------------------------------------
// 函数名称    左边界八邻域爬坡 (绝对起始方向：下方 + 顺时针)
//-------------------------------------------------------------------------------------------------------------------
void Trace_Left_Boundary()
{
    int curr_x = Left_boundary_x[0];
    int curr_y = Left_boundary_y[0];
    
    left_point_cnt = 1; 

    for (int i = 1; i < MAX_FIND_POINTS; i++)
    {
        // 【安全保护】防止数组越界崩溃
        if (left_point_cnt >= 199) 
        {
            break; 
        }

        int found = 0;
        
        // 【修改点】无论如何，绝对从下方（索引0）开始找
        int start_dir = 0; 
        
        int has_last_val = 0; 
        int last_val = 0;     

        // 扫 7 个方向（留 1 个死角防止原地 180° 掉头）
        for (int j = 0; j < 8; j++)
        {
            // 【修改点】顺时针扫描 (0 -> 7 -> 6 -> 5 -> 4 -> 3 -> 2)
            int check_dir = (start_dir - j + 8) % 8; 
            int check_x = curr_x + dir_offset[check_dir][0];
            int check_y = curr_y + dir_offset[check_dir][1];

            // 1. 图像数组越界保护
            if (check_x < 0 || check_x >= MT9V03X_W || check_y < 0 || check_y >= MT9V03X_H)
            {
                continue; 
            }

            int curr_val = image_copy[check_y][check_x];

            // 2. 记录起始点
            if (!has_last_val) 
            {
                last_val = curr_val;
                has_last_val = 1;
                continue; 
            }

            // 3. 滑动计算差比和
            int ratio = Calc_Difference_Sum_Ratio(last_val, curr_val);

            // 4. 判断跳变 (??已修复 -(ratio) 的致命笔误)
            if (-(ratio) > EDGE_RATIO_THRESHOLD)
            {
                curr_x = check_x;
                curr_y = check_y;
                
                Left_boundary_x[left_point_cnt] = curr_x;
                Left_boundary_y[left_point_cnt] = curr_y;
                left_point_cnt++;
                
                found = 1;
                break; 
            }

            last_val = curr_val;
        }

        // 如果前方没路了
        if (!found) 
        {
            break; 
        }
    }
    
    // ==========================================
    // 爬坡结束，将未用到的数组空间全部归0 (??已统一修正为 500)
    // ==========================================
    for (int i = left_point_cnt; i < 200; i++)
    {
        Left_boundary_x[i] = 0;
        Left_boundary_y[i] = 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数名称    右边界八邻域爬坡 (绝对起始方向：下方 + 逆时针)
//-------------------------------------------------------------------------------------------------------------------
void Trace_Right_Boundary()
{
    int curr_x = Right_boundary_x[0];
    int curr_y = Right_boundary_y[0];
    
    right_point_cnt = 1; 

    for (int i = 1; i < MAX_FIND_POINTS; i++)
    {
        // 【安全保护】防止数组越界崩溃
        if (right_point_cnt >= 199) 
        {
            break; 
        }

        int found = 0;
        
        // 【修改点】无论如何，绝对从下方（索引0）开始找
        int start_dir = 0; 
        
        int has_last_val = 0; 
        int last_val = 0;     

        // 扫 7 个方向（留 1 个死角防止原地 180° 掉头）
        for (int j = 0; j < 8; j++)
        {
            // 【修改点】逆时针扫描 (0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6)
            int check_dir = (start_dir + j) % 8; 
            int check_x = curr_x + dir_offset[check_dir][0];
            int check_y = curr_y + dir_offset[check_dir][1];

            // 1. 图像数组越界保护
            if (check_x < 0 || check_x >= MT9V03X_W || check_y < 0 || check_y >= MT9V03X_H)
            {
                continue; 
            }

            int curr_val = image_copy[check_y][check_x];

            // 2. 记录起始点
            if (!has_last_val) 
            {
                last_val = curr_val;
                has_last_val = 1;
                continue; 
            }

            // 3. 滑动计算差比和
            int ratio = Calc_Difference_Sum_Ratio(last_val, curr_val);

            // 4. 判断跳变
            if (-(ratio) > EDGE_RATIO_THRESHOLD)
            {
                curr_x = check_x;
                curr_y = check_y;
                
                Right_boundary_x[right_point_cnt] = curr_x;
                Right_boundary_y[right_point_cnt] = curr_y;
                right_point_cnt++;
                
                found = 1;
                break; 
            }

            last_val = curr_val;
        }

        if (!found) 
        {
            break; 
        }
    }
    
    // ==========================================
    // 爬坡结束，将未用到的数组空间全部归0 (??已统一修正为 500)
    // ==========================================
    for (int i = right_point_cnt; i < 200; i++)
    {
        Right_boundary_x[i] = 0;
        Right_boundary_y[i] = 0;
    }
}
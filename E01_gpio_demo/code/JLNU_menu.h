/*
 * JLNU_menu.h
 *
 * 菜单模块头文件
 * 用于TFT180屏幕的三级菜单显示系统
 *
 *  Created on: 2025年10月21日
 *      Author: Run
 */

#ifndef CODE_JLNU_MENU_H_
#define CODE_JLNU_MENU_H_

/* 菜单显示主函数 */
void tft_show(void);

#define MARGIN 8              /* 屏幕边缘留白像素数 */
#define GPS_SAMPLE_TARGET 50  /* GPS目标采集点数 */

/* 一级菜单页面显示函数 */
void Page_One(void);          /* 一级菜单第1页：Moter_Mode闪烁 */
void Page_Two(void);          /* 一级菜单第2页：PID闪烁 */
void Page_Three(void);        /* 一级菜单第3页：GPS闪烁 */
void Page_Four(void);         /* 一级菜单第4页：Point_Line闪烁 */
void Page_Five(void);         /* 一级菜单第5页：Camera闪烁 */

/* 二级菜单 - Moter_Mode子页面显示函数 */
void Page_One_1(void);        /* 二级菜单：Motor_Zero闪烁 */
void Page_One_2(void);        /* 二级菜单：Motor_IMU_Test闪烁 */
void Page_One_3(void);        /* 二级菜单：Motor_Speed=0闪烁 */
void Page_One_4(void);        /* 二级菜单：Motor_KeMu_1闪烁 */
void Page_One_5(void);        /* 二级菜单：Motor_KM_2闪烁 */
void Page_One_6(void);        /* 二级菜单：Motor_YaoKong闪烁 */
void Page_One_7(void);        /* 二级菜单：Motor_GPS闪烁 */
void Page_One_8(void);        /* 二级菜单：Motor_GPS_KM1闪烁 */

/* 三级菜单 - Moter_Mode子页面详情显示函数 */
void Page_One_1_1(void);      /* 三级菜单：电机模式1 */
void Page_One_2_1(void);      /* 三级菜单：电机模式2 */
void Page_One_3_1(void);      /* 三级菜单：电机模式3 */
void Page_One_4_1(void);      /* 三级菜单：电机模式4（显示角度、坐标、目标点） */
void Page_One_5_1(void);      /* 三级菜单：电机模式5 */
void Page_One_6_1(void);      /* 三级菜单：电机模式6（遥控模式） */
void Page_One_7_1(void);      /* 三级菜单：电机模式7 */
void Page_One_8_1(void);      /* 三级菜单：电机模式8 */

/* 二级菜单 - PID子页面显示函数 */
void Page_Two_1(void);        /* 二级菜单：Angle_V_P闪烁 */
void Page_Two_2(void);        /* 二级菜单：Angle_P闪烁 */
void Page_Two_3(void);        /* 二级菜单：Speed_P闪烁 */
void Page_Two_4(void);        /* 二级菜单：Speed_I闪烁 */
void Page_Two_5(void);        /* 二级菜单：Speed_D闪烁 */

/* 三级菜单 - PID参数调节页面显示函数 */
void Page_Two_1_1(void);      /* 三级菜单：PID参数调节 - 角速度P */
void Page_Two_2_1(void);      /* 三级菜单：PID参数调节 - 角度P */
void Page_Two_3_1(void);      /* 三级菜单：PID参数调节 - 速度P */
void Page_Two_4_1(void);      /* 三级菜单：PID参数调节 - 速度I */
void Page_Two_5_1(void);      /* 三级菜单：PID参数调节 - 速度D */

/* 二级菜单 - GPS子页面显示函数 */
void Page_Three_1(void);      /* 二级菜单：GPS-Show闪烁 */
void Page_Three_2(void);      /* 二级菜单：GPS-Point_Get闪烁 */
void Page_Three_3(void);      /* 二级菜单：GPS-PointNum闪烁 */
void Page_Three_4(void);      /* 二级菜单：IMU_KM1闪烁 */
void Page_Three_5(void);      /* 二级菜单：IMU_KM1-Show闪烁 */
void Page_Three_6(void);      /* 二级菜单：IMU_KM2闪烁 */
void Page_Three_7(void);      /* 二级菜单：IMU_KM2-Show闪烁 */
void Page_Three_8(void);      /* 二级菜单：GPS-Show（最后一项）闪烁 */

/* 三级菜单 - GPS数据详情页面显示函数 */
void Page_Three_1_1(void);    /* 三级菜单：GPS数据显示（经纬度、速度、方向等） */
void Page_Three_2_1(void);    /* 三级菜单：GPS坐标采集与编辑页面 */
void Page_Three_3_1(void);    /* 三级菜单：GPS XY坐标点列表显示 */
void Page_Three_4_1(void);    /* 三级菜单：IMU路径点列表（可编辑） */
void Page_Three_5_1(void);    /* 三级菜单：IMU使用路径点列表 */
void Page_Three_6_1(void);    /* 三级菜单：IMU KM2路径点列表（可编辑） */
void Page_Three_7_1(void);    /* 三级菜单：IMU KM2使用路径点列表 */
void Page_Three_8_1(void);    /* 三级菜单：GPS融合坐标点列表 */

/* 二级菜单 - Point_Line子页面显示函数 */
void Page_Four_1(void);       /* 二级菜单：IMU_Points闪烁 */
void Page_Four_2(void);       /* 二级菜单：IMU_Points_Used闪烁 */
void Page_Four_3(void);       /* 二级菜单：IMU_Points_KM2闪烁 */
void Page_Four_4(void);       /* 二级菜单：IMU_Points_KM2_used闪烁 */
void Page_Four_5(void);       /* 二级菜单：IMU_GPS闪烁 */
void Page_Four_6(void);       /* 二级菜单：IMU_GPS_used闪烁 */

/* 三级菜单 - 轨迹绘制页面显示函数 */
void Page_Four_1_1(void);     /* 三级菜单：GPS轨迹绘制 */
void Page_Four_2_1(void);     /* 三级菜单：IMU使用点轨迹绘制 */
void Page_Four_3_1(void);     /* 三级菜单：IMU KM2轨迹绘制（待实现） */
void Page_Four_4_1(void);     /* 三级菜单：IMU KM2使用点轨迹绘制 */
void Page_Four_5_1(void);     /* 三级菜单：GPS轨迹绘制（待实现） */
void Page_Four_6_1(void);     /* 三级菜单：GPS融合轨迹绘制 */

/* 二级菜单 - Camera子页面显示函数 */
void Page_Five_1(void);       /* 二级菜单：摄像头图像显示 */

/* 菜单初始化函数 */
void menu_init(void);

/* 菜单状态全局变量 */
extern uint8 menu_level;                  /* 当前菜单级别: 1=一级菜单, 2=二级菜单, 3=三级菜单 */
extern uint8 menu_serial_number_One;      /* 一级菜单选中项序号 */
extern uint8 menu_serial_number_Two;      /* 二级菜单选中项序号 */
extern uint8 menu_serial_number_Three;    /* 三级菜单选中项序号 */
extern uint8 selected_index;              /* 路径点列表中当前选中的点索引 */
extern uint8 TFT_XY_Flag;                /* XY坐标修改模式标志: 0=展示模式, 1=修改模式 */
extern uint8 XiuGai_XY;                  /* 修改X或Y坐标的切换标志: 0=修改X, 1=修改Y */

#endif /* CODE_JLNU_MENU_H_ */

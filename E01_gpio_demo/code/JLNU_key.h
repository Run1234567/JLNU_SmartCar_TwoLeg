#ifndef _JLNU_KEY_H_
#define _JLNU_KEY_H_

/* 函数声明 */
void KEY_INIT(void);          /* 按键GPIO初始化 */
uint8_t KEY_SCAN(void);       /* 按键扫描函数，返回按下的按键编号 */
void Key_ISR(void);           /* 按键中断服务函数，处理菜单导航逻辑 */

/* 按键方向常量定义 */
#define KEY_UPWARD   1        /* 向上滚动 */
#define KEY_DOWNWARD 2        /* 向下滚动 */
#define KEY_ENTER    3        /* 确认/进入 */
#define KEY_EXIT     4        /* 退出/返回 */

/* 一级菜单最大项数 */
#define menu_serial_number_1_Max   5

/* 二级菜单各子菜单最大项数 */
#define menu_serial_number_1_1_Max 8   /* Moter_Mode 子菜单最大项数 */
#define menu_serial_number_1_2_Max 5   /* PID 子菜单最大项数 */
#define menu_serial_number_1_3_Max 8   /* GPS 子菜单最大项数 */
#define menu_serial_number_1_4_Max 6   /* Point_Line 子菜单最大项数 */
#define menu_serial_number_1_5_Max 1   /* Camera 子菜单最大项数 */

/* 按键状态全局变量 */
extern uint8 Key_Now;         /* 当前按键状态 */
extern uint8 Key_Before;      /* 上一次按键状态 */
extern uint8 Key_Down;        /* 按键按下边沿检测结果 */

#endif

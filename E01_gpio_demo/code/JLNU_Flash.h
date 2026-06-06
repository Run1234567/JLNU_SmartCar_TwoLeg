/*
 * JLNU_Flash.h
 *
 *  Flash存储模块头文件
 *  用于将路径点数据保存到Flash或从Flash加载
 */

#ifndef JLNU_FLASH_H
#define JLNU_FLASH_H

void Save_IMU_To_Flash(void);           /* 保存IMU坐标数据到Flash */
void Load_IMU_From_Flash(void);         /* 从Flash加载IMU坐标数据 */
void Save_IMU_KM2_To_Flash(void);       /* 保存KM2坐标数据到Flash */
void Load_IMU_KM2_From_Flash(void);     /* 从Flash加载KM2坐标数据 */
void Save_XY_To_Flash(void);            /* 保存XY坐标数据到Flash */
void Load_XY_From_Flash(void);          /* 从Flash加载XY坐标数据 */
void Save_IMU_GPS_To_Flash(void);       /* 保存GPS坐标数据到Flash */
void Load_IMU_GPS_From_Flash(void);     /* 从Flash加载GPS坐标数据 */

/* 通用Flash数组保存函数：将任意数组保存到指定Flash页 */
uint8 Flash_Save_Array(uint32 page_num, void *data_ptr, uint32 item_size, uint8 count, uint8 max_count);

/* 通用Flash数组加载函数：从指定Flash页加载任意数组 */
uint8 Flash_Load_Array(uint32 page_num, void *dest_ptr, uint32 item_size, uint8 *out_count, uint8 max_count);

#endif

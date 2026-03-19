#include "zf_common_headfile.h"

#define SAVE_IMU_PAGE_NUM  94 
#define SAVE_IMU_KM2_PAGE_NUM  93
#define SAVE_XY_PAGE_NUM  92

// ==========================================
// 函数功能：把采好的 IMU 点位存入 Flash
// ==========================================
void Save_IMU_To_Flash(void)
{
    flash_init();             
    flash_buffer_clear();     

    // 1. 把【当前 IMU 采点数量】存进缓冲区。
    // 注意：你的 current_IMU_point_count 是 uint8 类型，我们直接用 uint32_type 强转存入最安全
    flash_union_buffer[0].uint32_type = (uint32)current_IMU_point_count;

    // 2. 把【IMU 坐标数组】存入缓冲区的第 1 个格子之后
    // 你的 IMU_Point_t 包含两个 float(x和y)，共8字节。100个点只有800字节，完全装得下！
    memcpy(&flash_union_buffer[1], IMU_Points, sizeof(IMU_Point_t) * current_IMU_point_count);

    // 3. 计算需要写入多少个 32bit 的字
    // 数量占 1 个字，数组占 (总字节数 / 4) 个字
    uint32 write_len = 1 + (sizeof(IMU_Point_t) * current_IMU_point_count) / 4;

    // 4. 把缓冲区的数据一把刷进第 94 页！
    flash_write_page_from_buffer(0, SAVE_IMU_PAGE_NUM, write_len);
}

// ==========================================
// 函数功能：开机时从 Flash 恢复 IMU 点位数据
// 修改内容：数据存入 IMU_Points_used 数组中
// ==========================================
void Load_IMU_From_Flash(void)
{
    flash_init(); 

    // 1. 检查第 94 页有没有数据
    if(flash_check(0, SAVE_IMU_PAGE_NUM) == 0) 
    {
        current_IMU_point_count_used = 0;
        return;
    }

    // 2. 如果有数据，把第 94 页的内容全部倒腾回全局缓冲区里
    flash_read_page_to_buffer(0, SAVE_IMU_PAGE_NUM, FLASH_PAGE_LENGTH);

    // 3. 提取出【采点数量】
    uint32 saved_count = flash_union_buffer[0].uint32_type;

    // 防爆护盾：如果读出的数量大于 100，说明读取到了未知的乱
    if(saved_count > 100)
    {
        current_IMU_point_count_used = 0;
        return;
    }

    // 4. 数量合法，恢复给系统变量 (强转回 uint8)
    current_IMU_point_count_used = (uint8)saved_count;

    // 5. 【核心修改】把 Flash 里的坐标数组，复制到你新定义的 IMU_Points_used 里面！
    memcpy(IMU_Points_used, &flash_union_buffer[1], sizeof(IMU_Point_t) * current_IMU_point_count_used);
    
}

// ==========================================
// 函数功能：把采好的 科目二 IMU 点位存入 Flash
// ==========================================
void Save_IMU_KM2_To_Flash(void)
{
    flash_init();             
    flash_buffer_clear();     

    // 1. 把【科目二 采点数量】存进缓冲区。
    flash_union_buffer[0].uint32_type = (uint32)current_IMU_point_count_KM2;

    // 2. 把【科目二 坐标数组】存入缓冲区的第 1 个格子之后
    memcpy(&flash_union_buffer[1], IMU_Points_KM2, sizeof(IMU_Point_t) * current_IMU_point_count_KM2);

    // 3. 计算需要写入多少个 32bit 的字
    uint32 write_len = 1 + (sizeof(IMU_Point_t) * current_IMU_point_count_KM2) / 4;

    // 4. 把缓冲区的数据一把刷进第 93 页！
    flash_write_page_from_buffer(0, SAVE_IMU_KM2_PAGE_NUM, write_len);
}

// ==========================================
// 函数功能：开机时从 Flash 恢复 科目二 IMU 点位数据
// 修改内容：数据存入 IMU_Points_used_KM2 数组中
// ==========================================
void Load_IMU_KM2_From_Flash(void)
{
    flash_init(); 

    // 1. 检查第 93 页有没有数据
    if(flash_check(0, SAVE_IMU_KM2_PAGE_NUM) == 0) 
    {
        current_IMU_point_count_used_KM2 = 0;
        return;
    }

    // 2. 如果有数据，把第 93 页的内容全部倒腾回全局缓冲区里
    flash_read_page_to_buffer(0, SAVE_IMU_KM2_PAGE_NUM, FLASH_PAGE_LENGTH);

    // 3. 提取出【科目二采点数量】
    uint32 saved_count = flash_union_buffer[0].uint32_type;

    // 防爆护盾：如果读出的数量大于 100，说明读取到了未知的乱码
    if(saved_count > 100)
    {
        current_IMU_point_count_used_KM2 = 0;
        return;
    }

    // 4. 数量合法，恢复给系统变量 (强转回 uint8)
    current_IMU_point_count_used_KM2 = (uint8)saved_count;

    // 5. 【核心修改】把 Flash 里的坐标数组，复制到你新定义的 科目二寻迹数组 里面！
    memcpy(IMU_Points_used_KM2, &flash_union_buffer[1], sizeof(IMU_Point_t) * current_IMU_point_count_used_KM2);
}
// ==========================================
// 函数功能：把采好的 平面坐标 XY_Points 存入 Flash
// ==========================================
void Save_XY_To_Flash(void)
{
    flash_init();             
    flash_buffer_clear();     

    // 1. 把【采点数量】存进缓冲区
    flash_union_buffer[0].uint32_type = (uint32)current_point_count;

    // 2. 把【XY平面坐标数组】存入缓冲区的第 1 个格子之后
    // 你的 Local_Point_t 包含两个 float(x和y)，共8字节。100个点也是800字节，完全够存！
    memcpy(&flash_union_buffer[1], XY_Points, sizeof(Local_Point_t) * current_point_count);

    // 3. 计算需要写入多少个 32bit 的字
    uint32 write_len = 1 + (sizeof(Local_Point_t) * current_point_count) / 4;

    // 4. 把缓冲区的数据一把刷进第 92 页！
    flash_write_page_from_buffer(0, SAVE_XY_PAGE_NUM, write_len);
}

// ==========================================
// 函数功能：开机时从 Flash 恢复 XY 平面点位数据
// 核心逻辑：读取出的数据专门放进 XY_Points_used 数组里！
// ==========================================
void Load_XY_From_Flash(void)
{
    flash_init(); 

    // 1. 检查第 92 页有没有数据
    if(flash_check(0, SAVE_XY_PAGE_NUM) == 0) 
    {
        current_point_count_used = 0; // 如果没数据，寻迹点数设为0
        return;
    }

    // 2. 如果有数据，把第 92 页的内容全部倒腾回全局缓冲区
    flash_read_page_to_buffer(0, SAVE_XY_PAGE_NUM, FLASH_PAGE_LENGTH);

    // 3. 提取出【采点数量】
    uint32 saved_count = flash_union_buffer[0].uint32_type;

    // 4. 防爆护盾：如果读出的数量大于 100，说明读取到了空页面的乱码 (0xFFFFFFFF)
    if(saved_count > 100)
    {
        current_point_count_used = 0;
        return;
    }

    // 5. 数量合法，恢复给你指定的寻迹变量
    current_point_count_used = (uint8)saved_count;

    // 6. 【核心操作】把 Flash 缓冲区的坐标数组，精准复制到你的 XY_Points_used 里！
    memcpy(XY_Points_used, &flash_union_buffer[1], sizeof(Local_Point_t) * current_point_count_used);
}

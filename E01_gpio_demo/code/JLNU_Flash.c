#include "zf_common_headfile.h"

#define SAVE_IMU_PAGE_NUM  94 
#define SAVE_IMU_KM2_PAGE_NUM  93
#define SAVE_XY_PAGE_NUM  92

// ==========================================
// 函数功能：把采好的 IMU 点位存入 Flash
// ==========================================
void Save_IMU_To_Flash(void)
{
    // --------------------------------------------------------
    // 【第一道防线】：点数合法性检查
    // --------------------------------------------------------
    // 1. 如果根本没采到点，直接退出，不折腾 Flash
    if (current_IMU_point_count == 0) 
    {
        return; 
    }
    
    // 2. 假设你的最大采点数是 100，防止点数超标导致 memcpy 内存溢出（致命错误）
    if (current_IMU_point_count > 100) 
    {
        current_IMU_point_count = 100; // 截断处理，或者你也可以直接 return 放弃保存
    }

    // --------------------------------------------------------
    // 【第二道防线】：数据有效性检查（防浮点数爆炸）
    // --------------------------------------------------------
    for (uint8 i = 0; i < current_IMU_point_count; i++)
    {
        // 检查坐标是否在合理范围内（比如这台车不可能跑出 正负 10000 米）
        // 如果数据跑飞变成了几百万，下次读取出来放进 PID 计算直接会让轮子疯转
        if (IMU_Points[i].x > 10000.0f || IMU_Points[i].x < -10000.0f ||
            IMU_Points[i].y > 10000.0f || IMU_Points[i].y < -10000.0f)
        {
            // 发现一颗老鼠屎（脏数据），为了安全，整个数组都不要存了！
            return; 
        }
    }

    // --------------------------------------------------------
    // 校验通过，开始正式写入
    // --------------------------------------------------------
    flash_init();             
    flash_buffer_clear();     

    // 1. 存数量
    flash_union_buffer[0].uint32_type = (uint32)current_IMU_point_count;

    // 2. 存数组
    memcpy(&flash_union_buffer[1], IMU_Points, sizeof(IMU_Point_t) * current_IMU_point_count);

    // 3. 计算字数
    uint32 write_len = 1 + (sizeof(IMU_Point_t) * current_IMU_point_count) / 4;

    // --------------------------------------------------------
    // 【第三道防线】：写入长度检查
    // --------------------------------------------------------
    // 假设你的一页 Flash 大小是 1024 字节（也就是 256 个 32bit 的字）
    // 必须确保我们要写的内容不会撑爆这一页
    if (write_len > 256) 
    {
        return; // 数据太大，拒绝写入
    }

    // 4. 刷入 Flash 第 94 页
    flash_write_page_from_buffer(0, SAVE_IMU_PAGE_NUM, write_len);
    
    // 5. 【可选交互】保存成功后，用刚才的非阻塞蜂鸣器滴一下，给你个心安！
    // Buzzer_Beep_ms(200); 
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


void Save_IMU_GPS_To_Flash(void)
{
    flash_init();             
    flash_buffer_clear();     

    // 1. 把【采点数量】存进缓冲区 (强制转换成32位存进去)
    flash_union_buffer[0].uint32_type = (uint32)current_IMU_GPS_Num;

    // 2. 把【IMU_GPS坐标数组】存入缓冲区的第 1 个格子之后
    memcpy(&flash_union_buffer[1], IMU_GPS, sizeof(IMU_Point_t) * current_IMU_GPS_Num);

    // 3. 计算需要写入多少个 32bit 的字
    uint32 write_len = 1 + (sizeof(IMU_Point_t) * current_IMU_GPS_Num) / 4;

    // 4. 把缓冲区的数据一把刷进第 90 页！
    flash_write_page_from_buffer(0, 90, write_len);
}

// 假设你外部定义了：extern uint8 current_IMU_GPS_Num_Used;

void Load_IMU_GPS_From_Flash(void)
{
    flash_init(); 

    // 1. 检查第 90 页有没有数据
    if(flash_check(0, 90) == 0) 
    {
        current_IMU_GPS_Num_Used = 0; // 如果没数据，寻迹点数设为0
        return;
    }

    // 2. 如果有数据，把第 90 页的内容全部倒腾回全局缓冲区
    flash_read_page_to_buffer(0, 90, FLASH_PAGE_LENGTH);

    // 3. 提取出【采点数量】
    uint32 saved_count = flash_union_buffer[0].uint32_type;

    // 4. 防爆护盾：如果读出的数量大于 100，说明读取到了空页面的乱码
    if(saved_count > 100)
    {
        current_IMU_GPS_Num_Used = 0;
        return;
    }

    // 5. 数量合法，恢复给你指定的【寻迹专用】计数量
    current_IMU_GPS_Num_Used = (uint8)saved_count;

    // 6. 【核心操作】把 Flash 缓冲区的坐标数组，精准复制到你的 IMU_GPS_Used 里！
    memcpy(IMU_GPS_Used, &flash_union_buffer[1], sizeof(IMU_Point_t) * current_IMU_GPS_Num_Used);
}

// =========================================================================
// 函数功能：通用的 Flash 数组保存函数
// 参数说明：
//   page_num  - 要存入的 Flash 页码 (如 94)
//   data_ptr  - 要保存的数组首地址 (强转为 void *)
//   item_size - 单个元素占用字节数 (用 sizeof 获取)
//   count     - 当前需要保存的元素个数
//   max_count - 允许保存的最大个数 (防爆防溢出)
// 返回值：1 成功，0 失败
// =========================================================================
uint8 Flash_Save_Array(uint32 page_num, void *data_ptr, uint32 item_size, uint8 count, uint8 max_count)
{
    // 1. 点数合法性检查
    if (count == 0) return 0; 
    if (count > max_count) count = max_count; // 截断处理

    // 2. 准备 Flash
    flash_init();            
    flash_buffer_clear();    

    // 3. 填装缓冲区：第0个格子存数量，后面存数据
    flash_union_buffer[0].uint32_type = (uint32)count;
    memcpy(&flash_union_buffer[1], data_ptr, item_size * count);

    // 4. 计算写入字数 (加3再除以4，实现向上取整，防止漏掉末尾字节)
    uint32 write_len = 1 + (item_size * count + 3) / 4;

    // 5. 写入长度限制检查 (一页 1024 字节 = 256 字)
    if (write_len > 256) return 0; 

    // 6. 刷入 Flash
    flash_write_page_from_buffer(0, page_num, write_len);
    
    return 1; // 保存成功
}

// =========================================================================
// 函数功能：通用的 Flash 数组读取函数
// 参数说明：
//   page_num  - 要读取的 Flash 页码
//   dest_ptr  - 要接收数据的目标数组首地址 (强转为 void *)
//   item_size - 单个元素占用字节数 (用 sizeof 获取)
//   out_count - 用于接收“读出了多少个点”的变量指针
//   max_count - 允许读取的最大个数 (防爆盾)
// 返回值：1 成功，0 失败或无数据
// =========================================================================
uint8 Flash_Load_Array(uint32 page_num, void *dest_ptr, uint32 item_size, uint8 *out_count, uint8 max_count)
{
    flash_init(); 

    // 1. 检查页面有没有数据
    if(flash_check(0, page_num) == 0) 
    {
        *out_count = 0;
        return 0; // 无数据
    }

    // 2. 读取到缓冲区
    flash_read_page_to_buffer(0, page_num, FLASH_PAGE_LENGTH);

    // 3. 提取数量并校验
    uint32 saved_count = flash_union_buffer[0].uint32_type;
    if(saved_count > max_count)
    {
        *out_count = 0;
        return 0; // 数据异常
    }

    // 4. 恢复数据
    *out_count = (uint8)saved_count;
    memcpy(dest_ptr, &flash_union_buffer[1], item_size * (*out_count));
    
    return 1; // 读取成功
}
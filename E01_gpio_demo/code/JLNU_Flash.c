/*
 * JLNU_Flash.c
 *
 *  Flash存储模块实现
 *  使用Flash第90-94页存储不同类型的路径点数据
 *  包含数据合法性校验，防止写入异常数据
 */

#include "zf_common_headfile.h"

#define SAVE_IMU_PAGE_NUM  94               /* IMU坐标数据存储页 */
#define SAVE_IMU_KM2_PAGE_NUM  93           /* KM2坐标数据存储页 */
#define SAVE_XY_PAGE_NUM  92                /* XY坐标数据存储页 */
/* GPS坐标数据存储页使用90页，直接在函数中写入 */

// ==========================================
// 功能：把采好的 IMU 坐标数据写入 Flash
// ==========================================
void Save_IMU_To_Flash(void)
{
    // --------------------------------------------------------
    // 第一步：基本合法性校验
    // --------------------------------------------------------
    // 1. 如果没有已采点，直接退出，不写 Flash
    if (current_IMU_point_count == 0)
    {
        return;
    }

    // 2. 如果已采点的数量超过 100，防止越界 memcpy 内存踩踏
    if (current_IMU_point_count > 100)
    {
        current_IMU_point_count = 100; // 截断处理（也可以直接 return 不写）
    }

    // --------------------------------------------------------
    // 第二步：数据有效性检查（防止数据爆炸）
    // --------------------------------------------------------
    for (uint8 i = 0; i < current_IMU_point_count; i++)
    {
        // 检查坐标是否在合理范围内（赛场坐标不可能超过 ±10000 米）
        // 异常数据可能导致下次读取后 PID 输出直接饱和或反转
        if (IMU_Points[i].x > 10000.0f || IMU_Points[i].x < -10000.0f ||
            IMU_Points[i].y > 10000.0f || IMU_Points[i].y < -10000.0f)
        {
            // 发现一条脏数据，为了安全，整组数据都不写了
            return;
        }
    }

    // --------------------------------------------------------
    // 校验通过，开始正式写入
    // --------------------------------------------------------
    flash_init();
    flash_buffer_clear();

    // 1. 存点数
    flash_union_buffer[0].uint32_type = (uint32)current_IMU_point_count;

    // 2. 存数据
    memcpy(&flash_union_buffer[1], IMU_Points, sizeof(IMU_Point_t) * current_IMU_point_count);

    // 3. 计算长度
    uint32 write_len = 1 + (sizeof(IMU_Point_t) * current_IMU_point_count) / 4;

    // --------------------------------------------------------
    // 第三步：写入长度检查
    // --------------------------------------------------------
    // 一页 Flash 大小 1024 字节，即 256 个 32bit 字
    // 确保本次要写入的数据不超过容纳上限
    if (write_len > 256)
    {
        return; // 数据太大，拒绝写入
    }

    // 4. 刷写到 Flash 第 94 页
    flash_write_page_from_buffer(0, SAVE_IMU_PAGE_NUM, write_len);

    // 5. (可选)写入成功后可以给蜂鸣器提醒，用刚才的蜂鸣器函数响一下，当提示的
    // Buzzer_Beep_ms(200);
}

// ==========================================
// 功能：上电时从 Flash 恢复 IMU 坐标数据
// 修改：数据存入 IMU_Points_used 数组
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

    // 2. 读取数据：把第 94 页内容全部读进缓存/全局缓冲区
    flash_read_page_to_buffer(0, SAVE_IMU_PAGE_NUM, FLASH_PAGE_LENGTH);

    // 3. 读取已存的点数量
    uint32 saved_count = flash_union_buffer[0].uint32_type;

    // 基本校验：点数量不应该超过 100，否则说明读取到未初始化的脏数据
    if(saved_count > 100)
    {
        current_IMU_point_count_used = 0;
        return;
    }

    // 4. 合法性校验后，恢复系统变量 (强转成 uint8)
    current_IMU_point_count_used = (uint8)saved_count;

    // 5. 只复制有用的！从 Flash 缓冲区数组，复制到新鲜出炉的 IMU_Points_used 去！
    memcpy(IMU_Points_used, &flash_union_buffer[1], sizeof(IMU_Point_t) * current_IMU_point_count_used);
}

// ==========================================
// 功能：把采好的 目标点 IMU 坐标数据写入 Flash
// ==========================================
void Save_IMU_KM2_To_Flash(void)
{
    flash_init();
    flash_buffer_clear();

    // 1. 先存目标点 已采点数量写进缓存开头
    flash_union_buffer[0].uint32_type = (uint32)current_IMU_point_count_KM2;

    // 2. 先存目标点 坐标数据写进缓存的第 1 个字之后
    memcpy(&flash_union_buffer[1], IMU_Points_KM2, sizeof(IMU_Point_t) * current_IMU_point_count_KM2);

    // 3. 计算需要写入多少个 32bit 字
    uint32 write_len = 1 + (sizeof(IMU_Point_t) * current_IMU_point_count_KM2) / 4;

    // 4. 把缓冲区数据一次性刷写到第 93 页去！
    flash_write_page_from_buffer(0, SAVE_IMU_KM2_PAGE_NUM, write_len);
}

// ==========================================
// 功能：上电时从 Flash 恢复 目标点 IMU 坐标数据
// 修改：数据存入 IMU_Points_used_KM2 数组
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

    // 2. 读取数据：把第 93 页内容全部读进缓存/全局缓冲区
    flash_read_page_to_buffer(0, SAVE_IMU_KM2_PAGE_NUM, FLASH_PAGE_LENGTH);

    // 3. 读取已存目标点已采的点数量
    uint32 saved_count = flash_union_buffer[0].uint32_type;

    // 基本校验：点数量不应该超过 100，否则说明读取到了未初始化的脏数据
    if(saved_count > 100)
    {
        current_IMU_point_count_used_KM2 = 0;
        return;
    }

    // 4. 合法性校验后，恢复系统变量 (强转成 uint8)
    current_IMU_point_count_used_KM2 = (uint8)saved_count;

    // 5. 只复制有用的！从 Flash 缓冲区数组，复制到新鲜出炉的 目标点寻路数组 去！
    memcpy(IMU_Points_used_KM2, &flash_union_buffer[1], sizeof(IMU_Point_t) * current_IMU_point_count_used_KM2);
}

// ==========================================
// 功能：把采好的 平面坐标 XY_Points 数据写入 Flash
// ==========================================
void Save_XY_To_Flash(void)
{
    flash_init();
    flash_buffer_clear();

    // 1. 先存已采点数量写进缓存开头
    flash_union_buffer[0].uint32_type = (uint32)current_point_count;

    // 2. 先存XY平面坐标数据写进缓存的第 1 个字之后
    // 因为 Local_Point_t 只包含 float(x、y)，共8字节，100个点才800字节，完全够放！
    memcpy(&flash_union_buffer[1], XY_Points, sizeof(Local_Point_t) * current_point_count);

    // 3. 计算需要写入多少个 32bit 字
    uint32 write_len = 1 + (sizeof(Local_Point_t) * current_point_count) / 4;

    // 4. 把缓冲区数据一次性刷写到第 92 页去！
    flash_write_page_from_buffer(0, SAVE_XY_PAGE_NUM, write_len);
}

// ==========================================
// 功能：上电时从 Flash 恢复 XY 平面坐标数据
// 注意：逻辑是读取数据专门放进 XY_Points_used 数组里！
// ==========================================
void Load_XY_From_Flash(void)
{
    flash_init();

    // 1. 检查第 92 页有没有数据
    if(flash_check(0, SAVE_XY_PAGE_NUM) == 0)
    {
        current_point_count_used = 0; // 如果没有数据，寻路点数为0
        return;
    }

    // 2. 读取数据：把第 92 页内容全部读进缓存/全局缓冲区
    flash_read_page_to_buffer(0, SAVE_XY_PAGE_NUM, FLASH_PAGE_LENGTH);

    // 3. 读取已存的点数量
    uint32 saved_count = flash_union_buffer[0].uint32_type;

    // 4. 基本校验：点数量不应该超过 100，否则说明读取到了空页脏数据 (0xFFFFFFFF)
    if(saved_count > 100)
    {
        current_point_count_used = 0;
        return;
    }

    // 5. 合法性校验后，恢复给指针的寻路点数
    current_point_count_used = (uint8)saved_count;

    // 6. 最后一步！从 Flash 缓冲区把坐标数据复制出来，准备拷贝到 XY_Points_used 里！
    memcpy(XY_Points_used, &flash_union_buffer[1], sizeof(Local_Point_t) * current_point_count_used);
}


// ==========================================
// 功能：把采好的 IMU_GPS 数据写入 Flash (第90页)
// ==========================================
void Save_IMU_GPS_To_Flash(void)
{
    flash_init();
    flash_buffer_clear();

    // 1. 先存已采点数量写进缓存开头 (强制转换成32位存进去)
    flash_union_buffer[0].uint32_type = (uint32)current_IMU_GPS_Num;

    // 2. 先存IMU_GPS坐标数据写进缓存的第 1 个字之后
    memcpy(&flash_union_buffer[1], IMU_GPS, sizeof(IMU_Point_t) * current_IMU_GPS_Num);

    // 3. 计算需要写入多少个 32bit 字
    uint32 write_len = 1 + (sizeof(IMU_Point_t) * current_IMU_GPS_Num) / 4;

    // 4. 把缓冲区数据一次性刷写到第 90 页去！
    flash_write_page_from_buffer(0, 90, write_len);
}

// 外部声明：extern uint8 current_IMU_GPS_Num_Used;

// ==========================================
// 功能：上电时从 Flash 恢复 IMU_GPS 坐标数据
// ==========================================
void Load_IMU_GPS_From_Flash(void)
{
    flash_init();

    // 1. 检查第 90 页有没有数据
    if(flash_check(0, 90) == 0)
    {
        current_IMU_GPS_Num_Used = 0; // 如果没有数据，寻路点数为0
        return;
    }

    // 2. 读取数据：把第 90 页内容全部读进缓存/全局缓冲区
    flash_read_page_to_buffer(0, 90, FLASH_PAGE_LENGTH);

    // 3. 读取已存的点数量
    uint32 saved_count = flash_union_buffer[0].uint32_type;

    // 4. 基本校验：点数量不应该超过 100，否则说明读取到了空页脏数据
    if(saved_count > 100)
    {
        current_IMU_GPS_Num_Used = 0;
        return;
    }

    // 5. 合法性校验后，恢复给指针的"寻路专用"点数量
    current_IMU_GPS_Num_Used = (uint8)saved_count;

    // 6. 最后一步！从 Flash 缓冲区把坐标数据复制出来，准备拷贝到 IMU_GPS_Used 里！
    memcpy(IMU_GPS_Used, &flash_union_buffer[1], sizeof(IMU_Point_t) * current_IMU_GPS_Num_Used);
}

// =========================================================================
// 功能：把菜单修改后的 IMU_Points_used_KM2 保存到 Flash 第93页
// 说明：菜单里通过上下键微调坐标后，退出修改模式时调用此函数持久化
// =========================================================================
void Save_IMU_Used_KM2_To_Flash(void)
{
    Flash_Save_Array(SAVE_IMU_KM2_PAGE_NUM, IMU_Points_used_KM2,
                     sizeof(IMU_Point_t), current_IMU_point_count_used_KM2, 100);
}

// =========================================================================
// 功能：通用的 Flash 数组保存函数
// 参数说明：
//   page_num  - 要写入的 Flash 页号 (如 94)
//   data_ptr  - 要保存的数据的首地址 (强转为 void *)
//   item_size - 每个元素占用的字节数 (用 sizeof 获取)
//   count     - 当前需要保存的元素个数
//   max_count - 数组最大容量 (防止越界)
// 返回值：1 成功，0 失败
// =========================================================================
uint8 Flash_Save_Array(uint32 page_num, void *data_ptr, uint32 item_size, uint8 count, uint8 max_count)
{
    // 1. 基本合法性检查
    if (count == 0) return 0;
    if (count > max_count) count = max_count; // 截断处理

    // 2. 准备 Flash
    flash_init();
    flash_buffer_clear();

    // 3. 封装数据：缓存第0个字存放数量，紧跟着拷贝实际数据
    flash_union_buffer[0].uint32_type = (uint32)count;
    memcpy(&flash_union_buffer[1], data_ptr, item_size * count);

    // 4. 计算写入长度 (加3再除以4，实际就是向上取整防止漏掉末尾字节)
    uint32 write_len = 1 + (item_size * count + 3) / 4;

    // 5. 写入长度限制 (一页 1024 字节 = 256 个字)
    if (write_len > 256) return 0;

    // 6. 刷写 Flash
    flash_write_page_from_buffer(0, page_num, write_len);

    return 1; // 写入成功
}

// =========================================================================
// 功能：通用的 Flash 数组读取函数
// 参数说明：
//   page_num  - 要读取的 Flash 页号
//   dest_ptr  - 要存入数据的目标数组首地址 (强转为 void *)
//   item_size - 每个元素占用的字节数 (用 sizeof 获取)
//   out_count - 用于接收"读了多少个点"的变量指针
//   max_count - 数组最大读取容量 (防溢出)
// 返回值：1 成功，0 失败或无数据
// =========================================================================
uint8 Flash_Load_Array(uint32 page_num, void *dest_ptr, uint32 item_size, uint8 *out_count, uint8 max_count)
{
    flash_init();

    // 1. 检查该页是否有数据
    if(flash_check(0, page_num) == 0)
    {
        *out_count = 0;
        return 0; // 无数据
    }

    // 2. 读取整页数据
    flash_read_page_to_buffer(0, page_num, FLASH_PAGE_LENGTH);

    // 3. 读取数据计数
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

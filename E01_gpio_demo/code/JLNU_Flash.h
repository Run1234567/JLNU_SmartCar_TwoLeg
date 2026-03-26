#ifndef JLNU_FLASH_H
#define JLNU_FLASH_H
void Save_IMU_To_Flash(void);
void Load_IMU_From_Flash(void);
void Save_IMU_KM2_To_Flash(void);
void Load_IMU_KM2_From_Flash(void);
void Save_XY_To_Flash(void);
void Load_XY_From_Flash(void);
void Save_IMU_GPS_To_Flash(void);
void Load_IMU_GPS_From_Flash(void);
uint8 Flash_Save_Array(uint32 page_num, void *data_ptr, uint32 item_size, uint8 count, uint8 max_count);
uint8 Flash_Load_Array(uint32 page_num, void *dest_ptr, uint32 item_size, uint8 *out_count, uint8 max_count);
#endif
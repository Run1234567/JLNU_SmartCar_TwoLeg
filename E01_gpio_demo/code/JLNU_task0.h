/*
 * JLNU_task0.h
 *
 *  Created on: 2025Äê10ÔÂ21ÈÕ
 *      Author: ¿­
 */

#ifndef CODE_JLNU_TASK0_H_
#define CODE_JLNU_TASK0_H_

void task0(void);
void task1(void);
void set_target_motion(int16_t speed, int16_t turn);
float limit_value_float(float value, float min_val, float max_val);
int16_t limit_value(int16_t value, int16_t min_val, int16_t max_val);

extern int16 Speed_Left;
extern int16 Speed_Right;
extern int16 Moter_L;
extern int16 Moter_R;
extern float motion_output;

extern int16 Angle_Out_Left;
extern float balance_output;
extern int16_t final_left_duty;
extern int16_t final_right_duty;
#endif /* CODE_JLNU_TASK0_H_ */

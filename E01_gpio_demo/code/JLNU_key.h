#ifndef _JLNU_KEY_H_
#define _JLNU_KEY_H_

void KEY_INIT(void);

uint8_t KEY_SCAN(void);
void Key_ISR(void);
#define KEY_UPWARD 1
#define KEY_DOWNWARD 2
#define KEY_ENTER 3
#define KEY_EXIT 4 

#define menu_serial_number_1_Max 4
#define menu_serial_number_1_1_Max 8
#define menu_serial_number_1_2_Max 5
#define menu_serial_number_1_3_Max 8
#define menu_serial_number_1_4_Max 6
extern uint8 Key_Now;
extern uint8 Key_Before;
extern uint8 Key_Down;

#endif
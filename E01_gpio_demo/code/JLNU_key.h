#ifndef _JLNU_KEY_H_
#define _JLNU_KEY_H_

void KEY_INIT(void);

uint8_t KEY_SCAN(void);
void Key_ISR(void);
#define KEY_UPWARD 1
#define KEY_DOWNWARD 2
#define KEY_ENTER 3
#define KEY_EXIT 4 

extern uint8 Key_Now;
extern uint8 Key_Before;
extern uint8 Key_Down;

#endif
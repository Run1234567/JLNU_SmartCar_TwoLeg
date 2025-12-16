#ifndef _JLNU_KEY_H_
#define _JLNU_KEY_H_

void KEY_INIT(void);

uint8_t KEY_SCAN(void);
void Key_ISR(void);


extern uint8 Key_Now;
extern uint8 Key_Before;
extern uint8 Key_Down;

#endif
#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_
#include <Arduino.h>
#include <avr/interrupt.h>
#include <button.h>
#include <scheduler.h>
extern int timer1_flag;
extern int timer2_flag;
void setTimer1(uint64_t duration);
void setTimer2(uint64_t duration);
void timerRun();
void SWTimer_Init(void);

#endif /* INC_SOFTWARE_TIMER_H_ */
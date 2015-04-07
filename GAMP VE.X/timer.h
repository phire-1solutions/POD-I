#ifndef __TIMER_H
#define __TIMER_H

#include <Timers.h>



#ifndef __TIMEROFFSETS
extern unsigned int timer0Offset;
extern unsigned int timer1Offset;
#endif

#ifndef __lowLevelTimers
unsigned int littleTick;
unsigned int secondsTick;
#endif

typedef unsigned char task(unsigned char slot);

typedef struct
{
    unsigned char nextTime;
    unsigned char timeQuantum;
    task    *ptask;
} taskRecord;
typedef taskRecord *ptaskRecord;

#define SETUPTIMER0OFFSET(offset) { timer0Offset = offset; };
#define SETUPTIMER1OFFSET(offset) { timer1Offset = offset; };

#define TIMER0OFFSET { writeTimer0(timer0Offset); }
#define TIMER1OFFSET { writeTimer1(timer1Offset); }

#define TICKSPERSECOND          256
// 32768 / (1 * (2^16-2^8) // initialise to 256 each reset of the timer
// Crystal / Prescaler * bits resolution

void openTimer0(unsigned char config);
void closeTimer0(void);
void writeTimer0(unsigned int timer0);
unsigned int readTimer0(void);

void openTimer1(unsigned char config, unsigned char config1);
void closeTimer1(void);
void writeTimer1(unsigned int timer1);
unsigned int readTimer1(void);


#endif//__TIMERS_H


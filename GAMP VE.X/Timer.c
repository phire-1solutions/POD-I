#include "p18f47j53.h"

#define __TIMEROFFSETS
#include "Timer.h"

unsigned int timer0Offset; // offset used to tune the timer
unsigned int timer1Offset; // offset used to tune the timer


// Execute SETUPTIMER0OFFSET(TIMER0OFFSET); before executing openTimer0()

void openTimer0(unsigned char config)
{
  T0CON = (0x7f & config);      // Configure timer, but don't start it yet
  TMR0H = 0;                    // Reset Timer0 to 0x0000
  TMR0L = 0;
  INTCONbits.TMR0IF = 0;        // Clear Timer0 overflow flag
  if(config&0x80)               // If interrupts enabled
  {
      INTCONbits.TMR0IE = 1;    // Enable Timer0 overflow interrupt
      INTCON2bits.T0IP = 1;     // Set the Timer0 interupt priority to high
  }
  else
    INTCONbits.TMR0IE = 0;

  TIMER0OFFSET
  T0CONbits.TMR0ON = 1;     // Enable the timer
}


void openTimer1(unsigned char config, unsigned char config1)
{
  T1GCON = (0XF3 & config1);

//  T1CON = (0x7F & config) << 1 ;
  unsigned char x = (0x7F & config) << 1;
  T1CON = x;

  TMR1H=0;                      // clear out timer registers
  TMR1L=0;

  PIR1bits.TMR1IF=0;            // clear the interupt flag
  IPR1bits.TMR1IP = 1;          // set priority of Timer 1 interupts to high

  if(config & 0x80)               // enable interrupts if selected
    PIE1bits.TMR1IE=1;          // enable
  else
    PIE1bits.TMR1IE=0;          // disable

//  PIR5bits.TMR1GIF =0;          // clear the gate interupt flag

//  if(config1 & 0x04)            // enable gate interupts if required
//    PIE5bits.TMR1GIE=1;         // enable
//  else
//    PIE5bits.TMR1GIE=0;         // disable

//  if(T1GCONbits.TMR1GE)         //
//    T1GCONbits.T1GGO = 1;

    TIMER1OFFSET
    T1CONbits.TMR1ON = 1;  // Start Timer1
}


void writeTimer0(unsigned int timer0)
{
  union Timers timer;

  timer.lt = timer0;    // Copy timer value into union

  TMR0H = timer.bt[1];  // Write high byte to Timer0
  TMR0L = timer.bt[0];  // Write low byte to Timer0
}

void writeTimer1(unsigned int timer1)
{
  union Timers timer;

  timer.lt = timer1;    // Save the 16-bit value in local

  TMR1H = timer.bt[1];  // Write high byte to Timer1 High byte
  TMR1L = timer.bt[0];  // Write low byte to Timer1 Low byte
}

unsigned int readTimer0(void)
{
  union Timers timer;

  timer.bt[0] = TMR0L;  // Copy Timer0 low byte into union
  timer.bt[1] = TMR0H;  // Copy Timer0 high byte into union

  return (timer.lt);    // Return the int
}

unsigned int readTimer1(void)
{
  union Timers timer;

  timer.bt[0] = TMR1L;    // Read Lower byte
  timer.bt[1] = TMR1H;    // Read upper byte

  return (timer.lt);      // Return the 16-bit value
}


void closeTimer0(void)
{
  T0CONbits.TMR0ON = 0;  // Disable Timer1
  INTCONbits.TMR0IE = 0;   // Disable Timer1 overflow interrupts
}

void closeTimer1(void)
{
  T1CONbits.TMR1ON = 0;  // Disable Timer1
  PIE1bits.TMR1IE = 0;   // Disable Timer1 overflow interrupts
}


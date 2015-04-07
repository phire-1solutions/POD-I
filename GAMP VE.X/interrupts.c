/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif
//
//#define __INTVARS
#include "Timer.h"
//#undef __INTVARS

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/* High-priority service */

#if defined(__XC) || defined(HI_TECH_C)
void interrupt high_isr(void)
#elif defined (__18CXX)
#pragma code high_isr=0x08
#pragma interrupt high_isr
void high_isr(void)
#else
#error "Invalid compiler selection for implemented ISR routines"
#endif

{
      /* Determine which flag generated the interrupt */

      if(PIR1bits.TMR1IF == 1) // Timer 1
      {  // Timer 1 at 128 Hz

          LATDbits.LATD6 = ~LATDbits.LATD6;  // for the time being just toggle a port
//          LATBbits.LATB1 = ~LATBbits.LATB1;  // for the time being just toggle a LED
          littleTick++;
          TIMER1OFFSET

          if(littleTick == TICKSPERSECOND)
          {
              secondsTick++; // Seconds
              littleTick = 0;
          }
          PIR1bits.TMR1IF = 0; // Clear Interrupt Flag
          return;
      }
/*      else if (<Interrupt Flag 2>)
      {
          <Interrupt Flag 2=0>; // Clear Interrupt Flag 2
      }
      else
      {
          // Unhandled interrupts
      }
*/

}

/* Low-priority interrupt routine */
#if defined(__XC) || defined(HI_TECH_C)
void low_priority interrupt low_isr(void)
#elif defined (__18CXX)
#pragma code low_isr=0x18
#pragma interruptlow low_isr
void low_isr(void)
#else
#error "Invalid compiler selection for implemented ISR routines"
#endif
{

      /* This code stub shows general interrupt handling.  Note that these
      conditional statements are not handled within 3 seperate if blocks.
      Do not use a seperate if block for each interrupt flag to avoid run
      time errors. */
 }

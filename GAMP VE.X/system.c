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

#include "system.h"

/* Refer to the device datasheet for information about available
oscillator configurations. */
void ConfigureOscillator(void)
{
    // Both these need to change when moving to the external clock.
    //OSCCONbits.IRCF = 0b111;    // Set Internal Oscillator to 8MHz
    OSCCONbits.SCS = 0b00;        // Primary Clock Source
    //TRISAbits.TRISA6 = 0;         // While external OSC is not in use, enable RA6 to allow monitoring of the clock/4
    //TRISAbits.TRISA7 = 1;          // While external OSC is not in use, enable RA6 to allow monitoring of the clock/4
    // Dont forget to change the config bit CONFIG2L/CLKOEC to 0 when changiong to oscillator


}

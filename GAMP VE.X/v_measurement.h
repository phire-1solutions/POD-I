/* 
 * File:   v_measurement.h
 * Author: Paul
 *
 * Created on 25 February 2015, 8:54 PM
 */

#ifndef V_MEASUREMENT_H
#define	V_MEASUREMENT_H


#include "p18f47j53.h"


#define OVERFLOWINPUT PORTBbits.RB0

#define CALIIBRATEADC { ADCON1bits.ADCAL = 1; }
#define STARTADC { ADCON0bits.GODONE = 1; }
#define CHECKADC (ADCON0bits.GODONE == 0 )
#define ADCRUNNING (ADCON0bits.GODONE != 0 )

unsigned char getOverFlow(void);
unsigned int getADC(void);

#endif	/* V_MEASUREMENT_H */


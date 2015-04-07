/* 
 * File:   led.h
 * Author: Paul
 *
 * Created on 25 February 2015, 9:44 PM
 */

#ifndef LED_H
#define	LED_H

#include "p18f47j53.h"
#include "lowlevelmacros.h"


#define LEDCOUNT 3
#define LED1MASK 0b00000010
#define LED2MASK 0b00000100
#define LED3MASK 0b00001000

#define LED1_LAT LATBbits.LATB1
#define LED2_LAT LATBbits.LATB2
#define LED3_LAT LATBbits.LATB3

#define LED1_ON LED1_LAT = 0;
#define LED2_ON LED2_LAT = 0;
#define LED3_ON LED3_LAT = 0;

#define LED1_OFF LED1_LAT = 1;
#define LED2_OFF LED2_LAT = 1;
#define LED3_OFF LED3_LAT = 1;

#define ALLLEDSMASK (LED1MASK | LED2MASK | LED3MASK)
#define ALLLEDSOFFMASK ALLLEDSMASK
#define ALLLEDSONMASK ~(ALLLEDSMASK)
#define SETALLLEDS { LATBbits.lb &= ALLLEDSONMASK; }
#define CLEARALLLEDS { LATBbits.lb |= ALLLEDSOFFMASK; }
#define FLASHTOSTART { flashLEDN( ALLLEDSONMASK,2,2); }
#define DISPLAYHEXDIGIT(bits) { LATBbits.lb &= ~(bits<<1); }

void flashLEDN(unsigned char leds, unsigned int count,unsigned char d);
void startLEDSequence(unsigned char n);
void flashLEDSuccess(void);
void flashLEDFailure(void);
void dumpHEXdigit(unsigned char in);
void longDelay(unsigned char d);

#endif	/* LED_H */


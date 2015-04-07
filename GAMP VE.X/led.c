
#include "led.h"

void longDelay(unsigned char d)
{
    unsigned char m;
    for(m = 0; m < d; m++)
        Delay1KTCYx(255);
}

void flashLEDN(unsigned char leds, unsigned int count,unsigned char d)
{
    LATBbits_t preserve = LATBbits;
    for(unsigned int n=0; n<count;n++)
    {
      // Need to be careful as this is not interupt safe.
      LATBbits.lb = ~leds;
      longDelay(d);
      CLEARALLLEDS
      longDelay(d);
    }
    LATBbits = preserve;
}

void startLEDSequence(unsigned char n)
{
    FLASHTOSTART
    for(unsigned char count=0;count < n; count++)
    {
        flashLEDN(LED1MASK,1,1);
        flashLEDN(LED2MASK,1,1);
        flashLEDN(LED3MASK,1,1);
        flashLEDN(LED2MASK,1,1);
    }
}


void flashLEDSuccess(void)
{
    for(unsigned char count=0;count < 5; count++)
    {
        flashLEDN(LED1MASK | LED3MASK,1,2);
        flashLEDN(LED2MASK,1,2);
    }
}

void flashLEDFailure(void)
{
    for(unsigned char count=0;count < 5; count++)
    {
        flashLEDN(LED1MASK | LED3MASK,1,4);
    }
}

void dumpHEXdigit(unsigned char in)
{
    SETALLLEDS // A quick flash then display the number
    CLEARALLLEDS

    for (unsigned char n=0;n<8;n++)
    {
        if(in & (0b00000001 << n))
            LED1_ON
        longDelay(4);
        LED1_OFF
        longDelay(4);
    }
    SETALLLEDS // A quick flash then finish
    CLEARALLLEDS
}

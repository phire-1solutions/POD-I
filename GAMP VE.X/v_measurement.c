
#include "v_measurement.h"

unsigned char getOverFlow(void)
{
    return (unsigned char)OVERFLOWINPUT;
}

unsigned int getADC(void)
{
    STARTADC
    while (ADCRUNNING)
    {
    }
    return ((int)(ADRESH*256)) + (int)(ADRESL);
}

#include "miscellaneous.h"

unsigned char fromBCD(unsigned char in)
{
    convBCD bcd;
    bcd.uc = in;
    return bcd.hiNib*10 + bcd.loNib;
}



unsigned char toBCD(unsigned char in)
{
    // Only for values 0 - 99

    unsigned char out = 0;
    while(in >= 10)
    {
        out += 0x10;
        in -= 10;
    }
    out |= in;
    return out;
}

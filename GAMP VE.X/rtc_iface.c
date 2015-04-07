
#include <i2c.h>
#include "p18f47j53.h"
#include <string.h>
#include "Application.h"
#include "Miscellaneous.h"
#include "LowLevelMacros.h"
#include "rtc_iface.h"
#include "spi_iface.h"
#include "i2c_iface.h"


typedef struct
{
    unsigned char   BCDsec;
    unsigned char   BCDmin;
    unsigned char   BCDhrs;
    unsigned char   BCDday;
    unsigned char   BCDdte;
    unsigned char   BCDmth;
    unsigned char   BCDyrs;
} RTC_REGISTERS;




void RTCgetTime(pUTC out)
{
    RTC_REGISTERS rtcr;
    memset(&rtcr,0x00,sizeof(rtcr));
    
    RTCReadRegister(RTC_REG_SEC, (unsigned char *)&rtcr,sizeof(RTC_REGISTERS));
    out->sec = fromBCD(rtcr.BCDsec);
    out->min = fromBCD(rtcr.BCDmin);
    out->hrs = fromBCD(rtcr.BCDhrs);
    out->dte = fromBCD(rtcr.BCDdte);
    out->mth = fromBCD(rtcr.BCDmth);
    out->yrs = fromBCD(rtcr.BCDyrs);
}

void RTCsetTime(pUTC putc)
{
    RTC_REGISTERS rtcr;

    rtcr.BCDsec = toBCD(putc->sec);
    rtcr.BCDmin = toBCD(putc->min);
    rtcr.BCDhrs = toBCD(putc->hrs);
    rtcr.BCDday = 0;
    rtcr.BCDdte = toBCD(putc->dte);
    rtcr.BCDmth = toBCD(putc->mth);
    rtcr.BCDyrs = toBCD(putc->yrs);
    RTCWriteRegister(RTC_REG_SEC,(unsigned char *)&rtcr,sizeof(RTC_REGISTERS));
}

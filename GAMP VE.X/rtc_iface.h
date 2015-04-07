/* 
 * File:   RTC_iface.h
 * Author: Paul
 *
 * Created on 7 March 2015, 3:30 PM
 */

#ifndef RTC_IFACE_H
#define	RTC_IFACE_H

#include "i2c_iface.h"

#define RTC_REG_SEC     0x00
#define RTC_REG_MIN     0x01
#define RTC_REG_HRS     0x02
#define RTC_REG_DAY     0x03
#define RTC_REG_DATE    0x04
#define RTC_REG_MTH     0x05
#define RTC_REG_YRS     0x06
#define RTC_REG_CFG1    0x07
#define RTC_REG_TCH2    0x08
#define RTC_REG_CFG2    0x09

#define RTCADDRESS      (0b11010000)

typedef struct
{
    unsigned char   yrs;
    unsigned char   mth;
    unsigned char   dte;
    unsigned char   hrs;
    unsigned char   min;
    unsigned char   sec;
//    unsigned int    ms;  // only useful when we have GPS
} UTC;

typedef UTC *pUTC;

#define RTCWriteRegister(reg, buffer, len) I2CWriteRegister(RTCADDRESS, reg, buffer, len)
#define RTCReadRegister(reg, buffer, len) I2CReadRegister(RTCADDRESS, reg, buffer, len)

// Returns a UTC structure.  
void RTCgetTime(pUTC out);


// Sets the time given to taht con tained in the utc pointed to by putc
void RTCsetTime(pUTC putc);

// Copy tool for moving between structures
void copyUTC(pUTC out, pUTC in);

//unsigned char RTCReadRegister(unsigned char reg, unsigned char *buffer, unsigned len);
//unsigned char RTCWriteRegister(unsigned char reg, unsigned char *buffer, unsigned len);

#endif	/* RTC_IFACE_H */

